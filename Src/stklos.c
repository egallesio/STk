/*
 *
 *  s t k l o s . c			-- STklos support
 *
 * Copyright © 1993-1996 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
 * 
 *
 * Permission to use, copy, and/or distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that both the above copyright notice and this permission notice appear in
 * all copies and derived works.  Fees for distribution or use of this
 * software or derived works may only be charged with express written
 * permission of the copyright holder.  
 * This software is provided ``as is'' without express or implied warranty.
 *
 * This software is a derivative work of other copyrighted softwares; the
 * copyright notices of these softwares are placed in the file COPYRIGHTS
 *
 *
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  9-Feb-1994 15:56
 * Last file update: 24-Jul-1996 17:03 
 */

#ifdef USE_STKLOS

#include "stk.h"
#include "stklos.h"

#define CLASSP(x)	    (INSTANCEP(x) && SUBCLASSP(CLASS_OF(x), Class))
#define GENERICP(x)  	    (INSTANCEP(x) && SUBCLASSP(CLASS_OF(x), Generic))
#define METHODP(x)  	    (INSTANCEP(x) && SUBCLASSP(CLASS_OF(x), Method))

#define NCLASSP(x)	    (!CLASSP(x))
#define NGENERICP(x)	    (!GENERICP(x))
#define NMETHODP(x)  	    (!METHODP(x))

#define SPEC_OF(x)	    THE_SLOT_OF(x, S_specializers)


static char k_initform[]     = ":initform";     /* In a var so it can be */
static char k_init_keyword[] = ":init-keyword"; /* patched by STk_makekey */

static SCM Top, Object, Class, Generic, Method, Procedure_class, Entity_class;
static SCM Boolean, Char, Pair, Procedure, String, Symbol, Vector, Number, 
	   List, Null, Real, Integer, Keyword, Unknown;
#ifdef USE_TK
static SCM Widget;
#endif

static void slot_set_if_unbound(SCM obj, SCM slot_name, SCM value);

/*****************************************************************************/

static SCM make_instance(SCM classe, long  size, int type)
{
  register SCM z;
  register long i;

  NEWCELL(z, STk_tc_instance);
  INST(z) 	     = must_malloc(sizeof(Instance) + (size-1)*sizeof(SCM));

  CLASS_OF(z)        = classe;
  INST_TYPE(z)	     = type;
  ACCESSORS_OF(z)    = classe? THE_SLOT_OF(classe, S_getters_n_setters) : NIL;
  NUMBER_OF_SLOTS(z) = size;

  /* Set all the slots to unbound */
  for (i = 0; i < size; i++)
    THE_SLOT_OF(z, i) = UNBOUND;

  return z;
}

/******************************************************************************
 *
 * compute-cpl
 *
 *   This version doesn't handle multiple-inheritance. It serves only for
 * booting classes and will be overaloaded in Scheme
 *
 ******************************************************************************/

static SCM compute_cpl(SCM supers, SCM res)
{
  return NULLP(supers)? Reverse(res)
    		      : compute_cpl(THE_SLOT_OF(CAR(supers), S_direct_supers),
					 Cons(CAR(supers), res));
}

/******************************************************************************
 *
 * compute-slots
 *
 ******************************************************************************/

static SCM remove_duplicate_slots(SCM l, SCM res, SCM slots_already_seen)
{
  SCM tmp;

  if (NULLP(l)) return res;

  tmp = CONSP(CAR(l)) ? CAR(CAR(l)) : CAR(l);
  if (NSYMBOLP(tmp)) Err("%compute-slots: bad slot name", tmp);
  
  if (STk_memq(tmp, slots_already_seen) == Ntruth) {
    res 	       = Cons(CAR(l), res);
    slots_already_seen = Cons(tmp, slots_already_seen);
  }
  
  return remove_duplicate_slots(CDR(l), res, slots_already_seen);
}

static SCM build_slots_list(SCM dslots, SCM cpl)
{
  register SCM res = dslots;

  for (cpl = CDR(cpl); NNULLP(cpl); cpl = CDR(cpl))
    res = STk_append(LIST2(THE_SLOT_OF(CAR(cpl), S_direct_slots), res), 2);

  /* res contains a list of slots. Remove slots which appears more than once */
  return remove_duplicate_slots(Reverse(res), NIL, NIL);
}


static PRIMITIVE compute_slots(SCM classe)
{
  if (NCLASSP(classe)) Err("compute-class: bad class", classe);
  return build_slots_list(THE_SLOT_OF(classe, S_direct_slots),
			  THE_SLOT_OF(classe, S_cpl));
}

/******************************************************************************
 *
 * compute-getters-n-setters
 *  
 *   This version doesn't handle slot options. It serves only for booting 
 * classes and will be overaloaded in Scheme.
 *
 ******************************************************************************/

static SCM compute_getters_n_setters(SCM slots)
{
  SCM  res = NIL;
  long i   = 0;

  for (  ; NNULLP(slots); slots = CDR(slots)) 
    res = Cons(Cons(CAR(slots),STk_makeinteger(i++)), res);

  return res;
}

/******************************************************************************
 *
 * compute-initializers
 *
 ******************************************************************************/

static SCM build_initializers(SCM slots)
{
  SCM initform, tmp, curr_slot, res = NIL;

  for ( ; NNULLP(slots); slots = CDR(slots)) {
    tmp       = NIL;
    curr_slot = CAR(slots);

    if (CONSP(curr_slot)) {	
      /* slot is a pair. See if an :initform is provided */
      if (STk_llength(curr_slot) > 1) {
	initform = STk_get_keyword(STk_makekey(k_initform),CDR(curr_slot),NULL);
	if (initform)
	  tmp = STk_eval(LIST3(Sym_lambda, NIL, initform), NIL);
      }
    }
    res = Cons(tmp, res);
  }
  return Reverse(res);
}

/******************************************************************************
 *
 * initialize-object
 *
 ******************************************************************************/

static PRIMITIVE initialize_object(SCM obj, SCM initargs)
{
  SCM tmp, initializers, slots;

  if (NINSTANCEP(obj))
    Err("%initialize-object: bad instance", obj);
  if (NCONSP(initargs) && NNULLP(initargs)) 
    Err("%initialize-object: bad init list", initargs);
  
  initializers = THE_SLOT_OF(CLASS_OF(obj), S_initializers);
  slots        = THE_SLOT_OF(CLASS_OF(obj), S_slots);
  
  /* See for each slot how it must be initialized */
  for ( ; NNULLP(initializers); initializers=CDR(initializers), slots=CDR(slots)) {
    SCM slot_name  = CAR(slots);
    SCM slot_value = NULL;
    
    if (CONSP(slot_name)) {
      /* This slot admits (perhaps) to be initialized at creation time */
      tmp 	= STk_get_keyword(STk_makekey(k_init_keyword),CDR(slot_name), NULL);
      slot_name = CAR(slot_name);
      if (tmp) {
	/* an initarg was provided for this slot */
	if (NKEYWORDP(tmp))
	  Err("%initialize-object: initarg must be a keyword. It was", tmp);
	slot_value = STk_get_keyword(tmp, initargs, NULL);
      }
    }

    if (slot_value)
      /* set slot to provided value */
      STk_slot_set(obj, slot_name, slot_value);
    else 
      /* set slot to its :initform if it exists */
      if (NNULLP(CAR(initializers)))
	slot_set_if_unbound(obj, slot_name, Apply(CAR(initializers), NIL));
  }
  
  return obj;
}

/******************************************************************************/

SCM STk_basic_make_class(SCM classe, SCM name, SCM dsupers, SCM dslots)
{
  SCM z, cpl, slots, g_n_s;

  /* Allocate one instance */
  z     = make_instance(classe, NUMBER_OF_CLASS_SLOTS, TYPE_INSTANCE);

  /* Initialize its slots */
  cpl   = compute_cpl(dsupers, LIST1(z));
  slots = build_slots_list(dslots, cpl);
  g_n_s = compute_getters_n_setters(slots);

  THE_SLOT_OF(z, S_name)	      = name;
  THE_SLOT_OF(z, S_direct_supers)     = dsupers;
  THE_SLOT_OF(z, S_direct_slots)      = dslots;
  THE_SLOT_OF(z, S_cpl)		      = cpl;
  THE_SLOT_OF(z, S_slots)	      = slots;
  THE_SLOT_OF(z, S_nfields)	      = STk_makeinteger(STk_llength(slots));
  THE_SLOT_OF(z, S_getters_n_setters) = g_n_s;
  THE_SLOT_OF(z, S_initializers)      = build_initializers(slots);

  /* Don't forget to set the accessors list of the object */
  ACCESSORS_OF(z) = THE_SLOT_OF(classe, S_getters_n_setters);
  
  return z;
}

/******************************************************************************/

static void create_Top_Object_Class(void)
{
  SCM tmp, slots_of_class = LIST8(Intern("name"), 
				  Intern("direct-supers"),
				  Intern("direct-slots"),
				  Intern("cpl"),
				  Intern("slots"),
				  Intern("nfields"),
				  Intern("initializers"),
				  Intern("getters-n-setters"));

  /**** <Class> ****/
  tmp 	= Intern("<class>");
  Class = make_instance(NULL, NUMBER_OF_CLASS_SLOTS, TYPE_INSTANCE);

  CLASS_OF(Class)     = Class;
  ACCESSORS_OF(Class) = compute_getters_n_setters(slots_of_class);

  THE_SLOT_OF(Class, S_name) 		  = tmp;
  THE_SLOT_OF(Class, S_direct_supers)	  = NIL; /* will be changed */
  THE_SLOT_OF(Class, S_direct_slots)	  = slots_of_class;
  THE_SLOT_OF(Class, S_cpl)		  = NIL;  /* will be changed */
  THE_SLOT_OF(Class, S_slots)		  = slots_of_class;
  THE_SLOT_OF(Class, S_nfields)		  = STk_makeinteger(NUMBER_OF_CLASS_SLOTS);
  THE_SLOT_OF(Class, S_initializers)      = build_initializers(slots_of_class);
  THE_SLOT_OF(Class, S_getters_n_setters) = ACCESSORS_OF(Class);

  VCELL(tmp) = Class;

  /**** <Top> ****/
  tmp = Intern("<top>");
  Top = STk_basic_make_class(Class, tmp, NIL, NIL);

  VCELL(tmp) = Top;
  
  /**** <Object> ****/
  tmp	 = Intern("<object>");
  Object = STk_basic_make_class(Class, tmp, LIST1(Top), NIL);

  VCELL(tmp) = Object;

  /* <top> <object> and <class> were partly uninitialized. Correct them here */
  THE_SLOT_OF(Class, S_direct_supers)   = LIST1(Object);
  THE_SLOT_OF(Class, S_cpl)		= LIST3(Class, Object, Top);

  /* protect Top, Object and Class  against garbage collection */
  STk_gc_protect(&Top);
  STk_gc_protect(&Object);
  STk_gc_protect(&Class);
}

/******************************************************************************/

static PRIMITIVE instancep(SCM obj)
{
  return INSTANCEP(obj)? Truth: Ntruth;
}

PRIMITIVE STk_class_of(SCM obj)
{
  if (INSTANCEP(obj)) return CLASS_OF(obj);

  switch (TYPE(obj)) {
    case tc_boolean:	return Boolean;
    case tc_char:	return Char;
    case tc_cons:	return Pair;
    case tc_nil:	return Null;
    case tc_string:	return String;
    case tc_symbol:	return Symbol;
    case tc_vector:	return Vector;
    case tc_flonum:	return Real;
    case tc_integer:
    case tc_bignum:	return Integer;
    case tc_keyword:	return Keyword;
#ifdef USE_TK
    case tc_tkcommand:	return Widget;
#endif
    default: 		return (STk_procedurep(obj) == Truth)? Procedure: Unknown;
  }
}
static PRIMITIVE class_name(SCM obj)
{
  if (NINSTANCEP(obj)) Err("class-name: bad class", obj);
  return STk_slot_ref(obj, Intern("name"));
}
static PRIMITIVE class_direct_supers(SCM obj)
{
  if (NINSTANCEP(obj)) Err("class-direct-supers: bad class", obj);
  return STk_slot_ref(obj, Intern("direct-supers"));
}
static PRIMITIVE class_direct_slots(SCM obj)
{
  if (NINSTANCEP(obj)) Err("class-direct-slots: bad class", obj);
  return STk_slot_ref(obj, Intern("direct-slots"));
}
static PRIMITIVE class_cpl(SCM obj)
{
  if (NINSTANCEP(obj)) Err("class-precedence-list: bad class", obj);
  return STk_slot_ref(obj, Intern("cpl"));
}
static PRIMITIVE class_slots(SCM obj)
{
  if (NINSTANCEP(obj)) Err("class-slots: bad class", obj);
  return STk_slot_ref(obj, Intern("slots"));
}

static PRIMITIVE slot_existsp(SCM obj, SCM slot_name)
{
  if (NSYMBOLP(slot_name)) Err("slot-exists?: bad slot name", slot_name);
  if (NINSTANCEP(obj))     Err("slot-exists?: bad object", obj);
  return STk_assq(slot_name, ACCESSORS_OF(obj)) == Ntruth ? Ntruth : Truth;
}


/******************************************************************************
 *
 * slot-ref, slot-set! and slot-bound?
 *
 ******************************************************************************/

PRIMITIVE STk_slot_ref(SCM obj, SCM slot_name)
{
  register SCM entry;
  SCM res;

  if (NINSTANCEP(obj)) Err("slot-ref: bad instance", obj);
  
  entry = STk_assq(slot_name, ACCESSORS_OF(obj));
  if (entry == Ntruth) 
    Err("slot-ref: no slot with name", slot_name);

  /* Two cases here:
   *	- if (cdr entry) is an integer (the offset of this slot in the slots vector
   *	- otherwise (cadr entry) is the reader function to apply
   */
  res = INTEGERP(CDR(entry)) ? THE_SLOT_OF(obj, INTEGER(CDR(entry)))
    			     : Apply(STk_cadr(entry), LIST1(obj));
  if (res == UNBOUND) Err("slot-ref: slot unbound", slot_name);

  return res;
}

PRIMITIVE STk_slot_set(SCM obj, SCM slot_name, SCM value)
{
  register SCM entry;

  if (NINSTANCEP(obj)) Err("slot-set!: bad instance", obj);
  
  entry = STk_assq(slot_name, ACCESSORS_OF(obj));
  if (entry == Ntruth) 
    Err("slot-set!: no slot with name", slot_name);

  /* Two cases here:
   *	- if (cdr entry) is an integer (the offset of this slot in the slots vector)
   *	- otherwise (caddr entry) is the writer function to apply
   */
  if (INTEGERP(CDR(entry)))
    THE_SLOT_OF(obj, INTEGER(CDR(entry))) = value;
  else
    Apply(STk_caddr(entry), LIST2(obj, value));

  return UNDEFINED;
}

static PRIMITIVE slot_boundp(SCM obj, SCM slot_name)
{
  register SCM entry;
  SCM res;

  if (NINSTANCEP(obj)) Err("slot-bound?: bad instance", obj);
  
  entry = STk_assq(slot_name, ACCESSORS_OF(obj));
  if (entry == Ntruth) 
    Err("slot-bound?: no slot with name", slot_name);

  res = INTEGERP(CDR(entry)) ? THE_SLOT_OF(obj, INTEGER(CDR(entry)))
    			     : Apply(STk_cadr(entry), LIST1(obj));

  return (res == UNBOUND) ? Ntruth : Truth;
}

static void slot_set_if_unbound(SCM obj, SCM slot_name, SCM value)
{
  register SCM entry;

  if ((entry = STk_assq(slot_name, ACCESSORS_OF(obj))) == Ntruth) return;

  if (INTEGERP(CDR(entry))) {
    if (THE_SLOT_OF(obj, INTEGER(CDR(entry))) == UNBOUND)
      THE_SLOT_OF(obj, INTEGER(CDR(entry))) = value;
  }
  else {
    if (Apply(STk_cadr(entry), LIST1(obj)) == UNBOUND)
      Apply(STk_caddr(entry), LIST2(obj, value));
  }
}

/******************************************************************************
 *
 * %allocate-instance (the low level instance allocation primitive)
 *
 ******************************************************************************/
 
PRIMITIVE STk_allocate_instance(SCM classe)
{
  if (NCLASSP(classe)) Err("%allocate-instance: bad class", classe);
  return make_instance(classe, 
		       STk_integer_value(THE_SLOT_OF(classe, S_nfields)),
		       EQ(classe, Generic) ? TYPE_GENERIC : TYPE_INSTANCE);
}

/******************************************************************************
 *
 * %modify-instance (used by change-class to modify in place)
 * 
 ******************************************************************************/
static PRIMITIVE modify_instance(SCM old, SCM new)
{
  Instance *old_inst;

  if (NINSTANCEP(old) || NINSTANCEP(new)) 
	Err("%modify-instance: both parameters must be instances", NIL);

  /* Exchange the data contained in old and new */
  old_inst  = INST(old);
  INST(old) = INST(new);
  INST(new) = old_inst;
    
  return old;
}

static PRIMITIVE stklos_version(void)
{
  return STk_makestring(STKLOS_VERSION);
}

/******************************************************************************
 *
 *  GGGGG                FFFFF          
 *  G                    F    
 *  G  GG                FFF    
 *  G   G                F      
 *  GGGGG E N E R I C    F    U N C T I O N S
 *
 * This implementation provides
 *	- generic functions (with class specializers)
 *	- multi-methods
 *	- next-method 
 *	- a hard-coded MOP for standard gf, which can be overloaded for non-std gf
 *
 ******************************************************************************/

SCM STk_make_next_method(SCM methods, SCM args)
{
  register SCM z;
  
  NEWCELL(z, tc_next_method);
  CAR(z) = methods;
  CDR(z) = args;
  return z;
}

SCM STk_apply_next_method(SCM next, SCM provided_args)
{
  SCM methods = CAR(next);
  SCM args    = NULLP(provided_args)? CDR(next) : provided_args;

  if (NULLP(methods)) {
    /* not very useful, but we cannot do bette for now*/
    STk_err("no next-method  available", next); 
#ifdef NYI
    /* Ideal code */
    SCM gf = ????? /* Hard to find the gf at minimal cost. Need more work */
    return Apply(VCELL(Intern("no-next-method")), LIST2(gf, args));
#endif
  }
  {
    SCM m        = CAR(methods);
    SCM new_next = STk_make_next_method(CDR(methods), args);

    /* m is the function to call with args. */
    return Apply(THE_SLOT_OF(m, S_procedure), Cons(new_next, args));
  }
}

#ifdef NYI

This comment must be updated 

/******************************************************************************
 * 
 * Protocol for calling a generic fumction
 *
 * 	+ apply-generic (gf args)
 *		+ compute-applicable-methods (gf args)
 *		+ apply-methods (methods args)
 *			+ apply-method (method args next-methods)
 *				
 * apply-method calls make-next-method to build the "continuation" of a a method
 * Calling a next-method will call apply-next-method which in turn will call 
 * apply-method again to call effectively the following method.
 *
 ******************************************************************************/
#endif

static int applicablep(SCM actual, SCM formal)
{
  register SCM ptr;

  /* We test that (memq formal (slot-ref actual 'cpl))
   * However, we don't call memq here since we already know that
   * the list is well formed 
   */
  for (ptr=THE_SLOT_OF(actual, S_cpl); NNULLP(ptr); ptr = CDR(ptr)) { 
    if (CONSP(ptr)) {
      if (EQ(CAR(ptr), formal)) return TRUE;
    }
    else 
      Err("Internal error in applicable: bad list", actual);
  }
  return FALSE;
}

static int more_specificp(SCM m1, SCM m2, SCM *targs)
{
  register SCM s1, s2;
  register int i;
  /* 
   * Note: 
   *   m1 and m2 can have != length (i.e. one can be one element longer than the 
   * other when we have a dotted parameter list). For instance, with the call
   *   (M 1)
   * with
   *   (define-method M (a . l) ....)
   *   (define-method M (a) ....) 
   *
   * we consider that the second method is more specific.
   *
   * BTW, targs is an array of types. We don't need it's size since
   * we already know that m1 and m2 are applicable (no risk to go past
   * the end of this array).
   *
   */
  for (i=0,s1=SPEC_OF(m1),s2=SPEC_OF(m2); ; i++,s1=CDR(s1),s2=CDR(s2)) {
    if (NULLP(s1)) return 1;
    if (NULLP(s2)) return 0;
    if (CAR(s1) != CAR(s2)) {
      register SCM l, cs1 = CAR(s1), cs2 = CAR(s2);
      
      for (l = THE_SLOT_OF(targs[i], S_cpl);   ; l = CDR(l)) {
	if (EQ(cs1, CAR(l))) return 1;
	if (EQ(cs2, CAR(l))) return 0;
      }
      return 0;/* should not occur! */
    }
  }
  return 0; /* should not occur! */
}

#define BUFFSIZE 32		/* big enough for most uses */

static SCM sort_applicable_methods(SCM method_list, int size, SCM *targs)
{
  int i, j, incr;
  SCM *v, vector;
  SCM buffer[BUFFSIZE];
  SCM save = method_list;

  /* For reasonably sized method_lists we can try to avoid all the
   * consing and reorder the list in place...
   * This idea is due to David McClain <Dave_McClain@msn.com>
   */
  if (size <= BUFFSIZE) {
    for(i=0;  i < size; i++) {
      buffer[i]   = CAR(method_list);
      method_list = CDR(method_list);
    }
    v = buffer;
  } 
  else {
    /* Too many elements in method_list to keep everything locally */
    vector = STk_vector(save, size);
    v      = VECT(vector);
  }

  /* Use a simple shell sort since it is generally faster than qsort on 
   * small vectors (which is probably mostly the case when we have to
   * sort a list of applicable methods).
   */
  for (incr = size / 2; incr; incr /= 2) {
    for (i = incr; i < size; i++) {
      for (j = i-incr ;j >= 0; j -= incr) {
	if (more_specificp(v[j], v[j+incr], targs)) break;
	else {
	  SCM tmp   = v[j+incr];
	  v[j+incr] = v[j];
	  v[j]	    = tmp;
	}
      }
    }
  }

  if (size <= BUFFSIZE) {
    /* We did it in locally, so restore the original list (reordered) in-place */
    for(i=0, method_list=save; i < size; i++, v++) {
      CAR(method_list) = *v;
      method_list      = CDR(method_list);
    }
    return save;
  }
  /* If we are here, that's that we did it the hard way... */ 
  return STk_vector2list(vector);
}

SCM STk_compute_applicable_methods(SCM gf, SCM args, int len, int find_method)
{
  register int i;
  int count = 0;
  SCM l, fl, applicable = NIL;
  SCM save = args;
  SCM buffer[BUFFSIZE], *types, *p;
  SCM tmp;
 
  /* Build the list of arguments types */
  if (len >= BUFFSIZE) {
    tmp   = STk_makevect(len, NULL);
    types = p = VECT(tmp);
  }
  else
    types = p = buffer;
  
  for (  ; NNULLP(args); args = CDR(args)) 
    *p++ = STk_class_of(CAR(args));
 
  /* Build a list of all applicable methods */
  for (l = THE_SLOT_OF(gf, S_methods); NNULLP(l); l = CDR(l)) {
    for (i=0, fl=SPEC_OF(CAR(l));  ; i++, fl=CDR(fl)) {
      if (INSTANCEP(fl) || 	  	/* We have a dotted argument list */
	  (i >= len && NULLP(fl))) {	/* both list exhausted */
	applicable = Cons(CAR(l), applicable);
	count     += 1;
	break;
      }
      if (i >= len || NULLP(fl) || !applicablep(types[i], CAR(fl))) break;
    }
  }

  if (count == 0) {
    if (find_method) return Ntruth;
    Apply(VCELL(Intern("no-applicable-method")), LIST2(gf, save));
    /* if we are here, it's because no-applicable-method hasn't signaled an error */
    return NIL;
  }
  return (count == 1) ? applicable : 
    			sort_applicable_methods(applicable, count, types);
}



static SCM apply_method(SCM m, SCM args, SCM next_methods)
{
  return Apply(THE_SLOT_OF(m, S_procedure),
	       Cons(STk_make_next_method(next_methods, args), args));
}

SCM STk_apply_methods(SCM methods, SCM args)
{
  if (NULLP(methods)) {
    /* 
     * methods can be NIL if we have a no-applicable-method handler which 
     * doesn't signal an error (or dont ends with a call to next-method)
     * In this case return an undefined value
     */
    return UNDEFINED;
  }

  return apply_method(CAR(methods), args, CDR(methods));
}

SCM STk_apply_generic(SCM gf, SCM args)
{
  if (NGENERICP(gf)) Err("apply: bad generic function", gf);
  if (NULLP(THE_SLOT_OF(gf, S_methods))) 
    Err("apply: no method for generic", gf);

  return STk_apply_methods(
           STk_compute_applicable_methods(gf, args, STk_llength(args), FALSE),
	   args);
}


SCM STk_apply_user_generic(SCM gf, SCM args)
{
  if (NGENERICP(gf)) STk_err("apply: bad generic function", gf);
  return Apply(VCELL(Intern("apply-generic")), LIST2(gf, args));
}


/******************************************************************************
  *
  * add-method
  *
  *******************************************************************************/

static SCM compute_new_list_of_methods(SCM gf, SCM new)
{
  SCM l1, l2, l;
  SCM new_spec = SPEC_OF(new);
  SCM methods  = THE_SLOT_OF(gf, S_methods);

  for (l = methods; NNULLP(l); l = CDR(l)) {
    for (l1=new_spec, l2=SPEC_OF(CAR(l));    ; l1=CDR(l1), l2=CDR(l2)) {
      if (NULLP(l1) && NULLP(l2)) {
	/* The spec. list of new method already exists in the gf mehods list */	
	CAR(l) = new;
	return methods;
      }
      if (NULLP(l1) || NULLP(l2) || NEQ(CAR(l1), CAR(l2))) break;
    }
  }

  /* If we are here, we have not encountered a method with same specializers */
  return Cons(new, methods);
}


static PRIMITIVE add_method(SCM gf, SCM method)
{
  if (NGENERICP(gf))    Err("add-method: bad generic function", gf); 
  if (NMETHODP(method)) Err("add-method: bad method", method);
  
  THE_SLOT_OF(gf, S_methods) = compute_new_list_of_methods(gf, method); 
  return method;
}

/******************************************************************************
 *
 * A simple make (which will be redefined later in Scheme)
 * This version handles only creation of gf, methods and classes (no instances)
 *
 * Since this code will disappear when Stklos will be fully booted, 
 * no precaution is taken to be efficient.
 *
 ******************************************************************************/

static char k_name[] 	     = ":name";		/* Use vars since makekey patches */
static char k_specializers[] = ":specializers"; /* its argument. This avoids the */
static char k_procedure[]    = ":procedure";	/* -fwritable_string */
static char k_dsupers[]	     = ":dsupers";
static char k_slots[]	     = ":slots";
static char k_gf[]	     = ":generic-function";

static PRIMITIVE lmake(SCM args, int len)
{
  SCM classe, z;

  if (len == 0) Err("make: parameter list is null", NIL);

  classe = CAR(args); args  = CDR(args); 
  
  if (classe == Generic) {
    z = make_instance(classe,
		      STk_llength(THE_SLOT_OF(classe, S_slots)), 
		      TYPE_GENERIC);

    THE_SLOT_OF(z, S_name)    = STk_get_keyword(STk_makekey(k_name), args, 
						Intern("???"));
    THE_SLOT_OF(z, S_methods) = NIL;
  }
  else {
    z = make_instance(classe, 
		      STk_llength(THE_SLOT_OF(classe, S_slots)), TYPE_INSTANCE);

    if (classe == Method) {
      THE_SLOT_OF(z, S_generic_function) =  
			STk_get_keyword(STk_makekey(k_gf), args, Ntruth);
      THE_SLOT_OF(z, S_specializers) =  
			STk_get_keyword(STk_makekey(k_specializers), args, NIL);
      THE_SLOT_OF(z, S_procedure) =
			STk_get_keyword(STk_makekey(k_procedure), args, NIL);
    }
    else {
      /* In all the others case, make a new class .... No instance here */
      THE_SLOT_OF(z, S_name) = 
			STk_get_keyword(STk_makekey(k_name), args, Intern("???"));
      THE_SLOT_OF(z, S_direct_supers) = 
			STk_get_keyword(STk_makekey(k_dsupers), args, NIL);
      THE_SLOT_OF(z, S_direct_slots)  = 
			STk_get_keyword(STk_makekey(k_slots), args, NIL);
    }
  }
  return z;
}

static PRIMITIVE find_method(SCM l, int len)
{
  SCM gf;
  
  if (len == 0) Err("find-method: no parameter list", NIL);

  gf = CAR(l); l = CDR(l);
  if (NGENERICP(gf)) Err("find-method: bad generic function", gf);
  if (NULLP(THE_SLOT_OF(gf, S_methods))) 
    Err("find-method: no methods for generic", gf);

  return STk_compute_applicable_methods(gf, l, len-1, TRUE);
}

static PRIMITIVE user_more_specificp(SCM m1, SCM m2, SCM targs)
{
  char *msg1 = "%method-more-specific?: bad method";
  char *msg2 = "%method-more-specific?: bad argument";
  SCM l, v;
  int i, len;

  if (NMETHODP(m1)) 	      STk_err(msg1, m1);
  if (NMETHODP(m2)) 	      STk_err(msg1, m2);
  if ((len=STk_llength(targs)) < 0) STk_err(msg2, targs);

  /* Verify that all the arguments of targs are classes and place them in a vector*/
  v = STk_makevect(len, NULL);

  for (i=0, l=targs; NNULLP(l); i++, l=CDR(l)) {
    if (NCLASSP(CAR(l))) STk_err(msg2, targs);
    VECT(v)[i] = CAR(l);
  }
  return more_specificp(m1, m2, VECT(v)) ? Truth: Ntruth;
}
  
  

/******************************************************************************
 *
 * Initializations 
 *
 ******************************************************************************/

static void make_stdcls(SCM *var, char *name, SCM meta, SCM super, SCM slots)
{
   SCM tmp = Intern(name);
   
   *var = STk_basic_make_class(meta, tmp, LIST1(super), slots);
   STk_gc_protect(var);
   VCELL(tmp) = *var;
}

static void make_standard_classes(void)
{
  SCM tmp1 = LIST3(Intern("generic-function"), 
		   Intern("specializers"), 
		   Intern("procedure"));
  SCM tmp2 = LIST2(Intern("name"), 
		   Intern("methods"));

  /* Generic functions classes */
  make_stdcls(&Procedure_class, "<procedure-class>", Class, Class, 	     NIL);
  make_stdcls(&Entity_class,    "<entity-class>",    Class, Procedure_class, NIL);
  make_stdcls(&Method,		"<method>",	     Class, Object,	     tmp1);
  make_stdcls(&Generic,		"<generic>",	     Entity_class, Object,   tmp2);

  /* Primitive types classes */
  make_stdcls(&Boolean, 	"<boolean>",	Class, 		 Top, 	    NIL);
  make_stdcls(&Char,		"<char>",	Class, 		 Top,	    NIL);
  make_stdcls(&List,		"<list>",	Class, 		 Top,	    NIL);
  make_stdcls(&Pair,		"<pair>",	Class, 		 List,	    NIL);
  make_stdcls(&Null,		"<null>", 	Class, 		 List,	    NIL);
  make_stdcls(&String,		"<string>",	Class, 		 Top,	    NIL);
  make_stdcls(&Symbol,		"<symbol>",	Class, 		 Top,	    NIL);
  make_stdcls(&Vector,		"<vector>",	Class, 		 Top,	    NIL);
  make_stdcls(&Number,		"<number>",	Class,		 Top,	    NIL);
  make_stdcls(&Real,		"<real>",	Class, 		 Number,    NIL);
  make_stdcls(&Integer,		"<integer>",	Class, 		 Real,	    NIL);
  make_stdcls(&Keyword,		"<keyword>",	Class, 		 Top,	    NIL);
  make_stdcls(&Unknown,		"<unknown>",	Class, 		 Top,	    NIL);
  make_stdcls(&Procedure,	"<procedure>",	Procedure_class, Top, 	    NIL);
#ifdef USE_TK
  make_stdcls(&Widget,		"<widget>",	Procedure_class, Procedure, NIL);
#endif
}  

PRIMITIVE STk_init_STklos(void)
{
  STk_disallow_sigint();

  Top = Object = Class = Generic = Method = NIL;
  create_Top_Object_Class();
  make_standard_classes();

  /* Define new primitives */
  STk_add_new_primitive("stklos-version",	 tc_subr_0, stklos_version);
  STk_add_new_primitive("instance?", 	         tc_subr_1, instancep);
  STk_add_new_primitive("slot-ref",		 tc_subr_2, STk_slot_ref);
  STk_add_new_primitive("slot-set!",	         tc_subr_3, STk_slot_set);
  STk_add_new_primitive("slot-bound?",	         tc_subr_2, slot_boundp);

  STk_add_new_primitive("class-of",		 tc_subr_1, STk_class_of); 
  STk_add_new_primitive("class-name",	         tc_subr_1, class_name);
  STk_add_new_primitive("class-direct-supers",   tc_subr_1, class_direct_supers);
  STk_add_new_primitive("class-direct-slots",    tc_subr_1, class_direct_slots);
  STk_add_new_primitive("class-precedence-list", tc_subr_1, class_cpl);
  STk_add_new_primitive("class-slots",	         tc_subr_1, class_slots);
  STk_add_new_primitive("slot-exists?",	         tc_subr_2, slot_existsp);

  STk_add_new_primitive("%allocate-instance",    tc_subr_1, STk_allocate_instance);
  STk_add_new_primitive("%initialize-object",    tc_subr_2, initialize_object);
  STk_add_new_primitive("%compute-slots",	 tc_subr_1, compute_slots);
  STk_add_new_primitive("%compute-initializers", tc_subr_1, build_initializers);
  STk_add_new_primitive("%modify-instance",	 tc_subr_2, modify_instance);
  

  STk_add_new_primitive("add-method", 		  tc_subr_2, add_method);
  STk_add_new_primitive("make",	  		  tc_lsubr,  lmake);
  STk_add_new_primitive("find-method",		  tc_lsubr,  find_method);
  STk_add_new_primitive("%method-more-specific?", tc_subr_3, user_more_specificp);

  STk_allow_sigint();
  
  return UNDEFINED;
}
#else 
  /* Don't produce an empty object file since some compiler (loader?) don't 
   * manage them corrctly 
   */
  static char not_used = '?';
#endif
