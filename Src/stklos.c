/*
 *
 *  s t k l o s . c			-- STklos support
 *
 * Copyright © 1993-1998 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 * $Id: stklos.c 1.14 Thu, 21 May 1998 20:00:04 +0000 eg $
 *
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  9-Feb-1994 15:56
 * Last file update: 19-May-1998 16:24 
 */

#ifdef USE_STKLOS

#include "stk.h"
#include "stklos.h"
#include "extend.h"
#include "module.h"

#define CLASSP(x)	    	(INSTANCEP(x) && SUBCLASSP(CLASS_OF(x), Class))
#define GENERICP(x)  	    	(INSTANCEP(x) && SUBCLASSP(CLASS_OF(x), Generic))
#define METHODP(x)  	    	(INSTANCEP(x) && SUBCLASSP(CLASS_OF(x), Method))

#define NCLASSP(x)	    	(!CLASSP(x))
#define NGENERICP(x)	    	(!GENERICP(x))
#define NMETHODP(x)  	    	(!METHODP(x))

#define SPEC_OF(x)	    	THE_SLOT_OF(x, S_specializers)


#define DEFVAR(v,val)		{STk_define_public_var(STklos,(v),(val)); \
				 STk_export_symbol(v, STklos);}
#define GETVAR(v)		(*(STk_varlookup((v), MOD_ENV(STklos),FALSE)))

#define CALL_GF1(name,a)	(Apply(GETVAR(Intern(name)), LIST1(a)))
#define CALL_GF2(name,a,b)	(Apply(GETVAR(Intern(name)), LIST2(a, b)))
#define CALL_GF3(name,a,b,c)	(Apply(GETVAR(Intern(name)), LIST3(a, b, c)))
#define CALL_GF4(name,a,b,c,d)	(Apply(GETVAR(Intern(name)), LIST4(a, b, c, d)))

#define CLASS_REDEF(c) THE_SLOT_OF(c, S_redefined)
#define TEST_CHANGE_CLASS(obj, classe) 						\
	{ 									\
	  classe = CLASS_OF(obj);						\
          if (CLASS_REDEF(classe) != Ntruth) 					\
	    CALL_GF3("change-object-class", obj, classe, CLASS_REDEF(classe));	\
	}

#define NXT_MTHD_METHODS	CAR
#define NXT_MTHD_ARGS		CDR

static char initialized	     = 0;

static SCM STklos;
static SCM Top, Object, Class, Generic, Method, Simple_method, Accessor, 
  	   Procedure_class, Entity_class;
static SCM Boolean, Char, Pair, Procedure, String, Symbol, Vector, Number, 
	   Liste, Null, Real, Integer, Keyword, Unknown;
#ifdef USE_TK
static SCM Widget;
#endif

static void set_slot_value_if_unbound(SCM classe, SCM obj, SCM slot_name, SCM form);

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

void STk_free_instance(SCM o)
{
  free(INST(o));
}
  
SCM STk_STklos_value(SCM symbol)
{
  return GETVAR(symbol);
}


/******************************************************************************
 *
 * Compute-cpl
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
    res = STk_append2(THE_SLOT_OF(CAR(cpl), S_direct_slots), res);

  /* res contains a list of slots. Remove slots which appears more than once */
  return remove_duplicate_slots(Reverse(res), NIL, NIL);
}


static PRIMITIVE compute_slots(SCM classe)
{
  if (NCLASSP(classe)) Err("%compute-slots: bad class", classe);
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
    res = Cons(Cons(CAR(slots),
		    Cons(Ntruth, 
			 STk_makeinteger(i++))),
	       res);
  return res;
}

/******************************************************************************
 *
 * initialize-object
 *
 ******************************************************************************/

static PRIMITIVE initialize_object(SCM obj, SCM initargs)
{
  static char k_init_keyword[] = ":init-keyword";
  SCM tmp, get_n_set, slots;
  SCM init_keyword = STk_makekey(k_init_keyword);
  SCM classe       = CLASS_OF(obj);

  ENTER_PRIMITIVE("%initialize-object");

  if (NINSTANCEP(obj)) 			    Serror("bad object", obj);
  if (NCONSP(initargs) && NNULLP(initargs)) Serror("bad init list", initargs);
  
  get_n_set = THE_SLOT_OF(classe, S_getters_n_setters);
  slots     = THE_SLOT_OF(classe, S_slots);
  
  /* See for each slot how it must be initialized */
  for ( ; NNULLP(slots); get_n_set=CDR(get_n_set), slots=CDR(slots)) {
    SCM slot_name  = CAR(slots);
    SCM slot_value = NULL;
    
    if (CONSP(slot_name)) {
      /* This slot admits (perhaps) to be initialized at creation time */
      tmp 	= STk_get_keyword(init_keyword,CDR(slot_name), NULL);
      slot_name = CAR(slot_name);
      if (tmp) {
	/* an initarg was provided for this slot */
	if (NKEYWORDP(tmp)) Serror("initarg must be a keyword. It was", tmp);
	slot_value = STk_get_keyword(tmp, initargs, NULL);
      }
    }

    if (slot_value)
      /* set slot to provided value */
      STk_slot_set(obj, slot_name, slot_value);
    else {
      /* set slot to its :initform if it exists */
      tmp = CAR(CDR(CAR(get_n_set)));
      if (tmp != Ntruth)
	set_slot_value_if_unbound(classe, obj, slot_name, tmp);
    }
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
  THE_SLOT_OF(z, S_direct_subclasses) = NIL;
  THE_SLOT_OF(z, S_direct_methods)    = NIL;
  THE_SLOT_OF(z, S_cpl)		      = cpl;
  THE_SLOT_OF(z, S_slots)	      = slots;
  THE_SLOT_OF(z, S_nfields)	      = STk_makeinteger(STk_llength(slots));
  THE_SLOT_OF(z, S_getters_n_setters) = g_n_s;
  THE_SLOT_OF(z, S_redefined) 	      = Ntruth;
  THE_SLOT_OF(z, S_environment)	      = MOD_ENV(STklos);

  /* Don't forget to set the accessors list of the object */
  ACCESSORS_OF(z) = THE_SLOT_OF(classe, S_getters_n_setters);
  
  /* Add this class in the direct-subclasses slot of dsupers */
  {
    SCM tmp;
    for (tmp = dsupers; NNULLP(tmp); tmp = CDR(tmp)) {
      THE_SLOT_OF(CAR(tmp), S_direct_subclasses) = 
			         Cons(z, 
				      THE_SLOT_OF(CAR(tmp), S_direct_subclasses));
    }
  }
  return z;
}

/******************************************************************************/

static void create_Top_Object_Class(void)
{
  SCM tmp, slots_of_class = Cons(Intern("name"),
			    Cons(Intern("direct-supers"),
			    Cons(Intern("direct-slots"),
			    Cons(Intern("direct-subclasses"),
			    Cons(Intern("direct-methods"),
			    Cons(Intern("cpl"),
			    Cons(Intern("slots"),
			    Cons(Intern("nfields"),
			    Cons(Intern("getters-n-setters"),
			    Cons(Intern("redefined"),
			    Cons(Intern("environment"),
				 NIL)))))))))));

  /* protect Top, Object and Class against garbage collection */
  STk_gc_protect(&Top); STk_gc_protect(&Object); STk_gc_protect(&Class);

  /**** <Class> ****/
  tmp 	= Intern("<class>");
  Class = make_instance(NULL, NUMBER_OF_CLASS_SLOTS, TYPE_INSTANCE);

  CLASS_OF(Class)     = Class;
  ACCESSORS_OF(Class) = compute_getters_n_setters(slots_of_class);

  THE_SLOT_OF(Class, S_name) 		 = tmp;
  THE_SLOT_OF(Class, S_direct_supers)	 = NIL;  /* will be changed */
  THE_SLOT_OF(Class, S_direct_slots)	 = slots_of_class;
  THE_SLOT_OF(Class, S_direct_subclasses)= NIL;
  THE_SLOT_OF(Class, S_direct_methods)	 = NIL;  
  THE_SLOT_OF(Class, S_cpl)		 = NIL;  /* will be changed */
  THE_SLOT_OF(Class, S_slots)		 = slots_of_class;
  THE_SLOT_OF(Class, S_nfields)		 = STk_makeinteger(NUMBER_OF_CLASS_SLOTS);
  THE_SLOT_OF(Class, S_getters_n_setters)= ACCESSORS_OF(Class);
  THE_SLOT_OF(Class, S_redefined) 	 = Ntruth;
  THE_SLOT_OF(Class, S_environment) 	 = MOD_ENV(STklos);

  DEFVAR(tmp, Class);

  /**** <Top> ****/
  tmp = Intern("<top>");
  Top = STk_basic_make_class(Class, tmp, NIL, NIL);

  DEFVAR(tmp, Top);
  
  /**** <Object> ****/
  tmp	 = Intern("<object>");
  Object = STk_basic_make_class(Class, tmp, LIST1(Top), NIL);

  DEFVAR(tmp, Object);

  /* <top> <object> and <class> were partially initialized. Correct them here */
  THE_SLOT_OF(Object, S_direct_subclasses) = LIST1(Class);

  THE_SLOT_OF(Class, S_direct_supers)      = LIST1(Object);
  THE_SLOT_OF(Class, S_cpl)		   = LIST3(Class, Object, Top);
}

/******************************************************************************/

static PRIMITIVE instancep(SCM obj)
{
  return INSTANCEP(obj)? Truth: Ntruth;
}

PRIMITIVE class_of(SCM obj)
{
  if (INSTANCEP(obj)) { 	
    SCM classe;
    
    TEST_CHANGE_CLASS(obj, classe);
    return CLASS_OF(obj);
  }
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
    default: 		if (EXTENDEDP(obj))
      			  return STk_extended_class_of(obj);
    		        else 
			  return (STk_procedurep(obj) == Truth)? Procedure: Unknown;
  }
}

/******************************************************************************
 * 
 * Meta object accessors
 *
 ******************************************************************************/
static PRIMITIVE class_name(SCM obj)
{
  if (NCLASSP(obj)) Err("class-name: bad class", obj);
  return STk_slot_ref(obj, Intern("name"));
}
static PRIMITIVE class_direct_supers(SCM obj)
{
  if (NCLASSP(obj)) Err("class-direct-supers: bad class", obj);
  return STk_slot_ref(obj, Intern("direct-supers"));
}
static PRIMITIVE class_direct_slots(SCM obj)
{
  if (NCLASSP(obj)) Err("class-direct-slots: bad class", obj);
  return STk_slot_ref(obj, Intern("direct-slots"));
}
static PRIMITIVE class_direct_subs(SCM obj)
{
  if (NCLASSP(obj)) Err("class-direct-subclasses: bad class", obj);
  return STk_slot_ref(obj, Intern("direct-subclasses"));
}
static PRIMITIVE class_direct_methods(SCM obj)
{
  if (NCLASSP(obj)) Err("class-direct-methods: bad class", obj);
  return STk_slot_ref(obj, Intern("direct-methods"));
}
static PRIMITIVE class_cpl(SCM obj)
{
  if (NCLASSP(obj)) Err("class-precedence-list: bad class", obj);
  return STk_slot_ref(obj, Intern("cpl"));
}
static PRIMITIVE class_slots(SCM obj)
{
  if (NCLASSP(obj)) Err("class-slots: bad class", obj);
  return STk_slot_ref(obj, Intern("slots"));
}
static PRIMITIVE class_environment(SCM obj)
{
  if (NCLASSP(obj)) Err("class-environment: bad class", obj);
  return STk_slot_ref(obj, Intern("environment"));
}


static PRIMITIVE gf_name(SCM obj)
{
  if (NGENERICP(obj)) Err("generic-function-name: bad function", obj);
  return STk_slot_ref(obj, Intern("name"));
}
static PRIMITIVE gf_methods(SCM obj)
{
  if (NGENERICP(obj)) Err("generic-function-methods: bad function", obj);
  return STk_slot_ref(obj, Intern("methods"));
}


static PRIMITIVE method_gf(SCM obj)
{
  if (NMETHODP(obj)) Err("method-generic-function: bad method", obj);
  return STk_slot_ref(obj, Intern("generic-function"));
}
static PRIMITIVE method_specializers(SCM obj)
{
  if (NMETHODP(obj)) Err("method-specializers: bad method", obj);
  return STk_slot_ref(obj, Intern("specializers"));
}
static PRIMITIVE method_procedure(SCM obj)
{
  if (NMETHODP(obj)) Err("method-procedure: bad method", obj);
  return STk_slot_ref(obj, Intern("procedure"));
}

/******************************************************************************
 *
 * S l o t   a c c e s s
 *
 ******************************************************************************/

static PRIMITIVE fast_slot_ref(SCM obj, SCM index)
{
  register long i = STk_integer_value(index);

  ENTER_PRIMITIVE("%fast-slot-ref");

  if (NINSTANCEP(obj))    		  Serror("bad object", obj);
  if (i < 0 || i >= NUMBER_OF_SLOTS(obj)) Serror("bad index", index);

  return THE_SLOT_OF(obj, i);
}

static PRIMITIVE fast_slot_set(SCM obj, SCM index, SCM value)
{
  register long i = STk_integer_value(index);

  ENTER_PRIMITIVE("%fast-slot-set!");
  if (NINSTANCEP(obj))    		  Serror("bad object", obj);
  if (i < 0 || i >= NUMBER_OF_SLOTS(obj)) Serror("bad index", index);

  THE_SLOT_OF(obj, i) = value;
  return UNDEFINED;
}

/** Utilities **/

static SCM get_slot_value(SCM classe, SCM obj, SCM slot_name)
{
  register SCM l;

  for (l=ACCESSORS_OF(obj); NNULLP(l); l=CDR(l)) {
    if (EQ(CAR(CAR(l)), slot_name)) {
      l = CDR(CDR(CAR(l)));
      /* Two cases here:
       *	- l is an integer (the offset of this slot in the slots vector)
       *	- otherwise (car l) is the getter function to apply
       */
      if (INTEGERP(l))
	return THE_SLOT_OF(obj, INTEGER(l));
      else {	/* We must evaluate (apply (car l) (list obj)) 
		 * where (car l) is known to be a closure of arity 1  */
	register SCM code, env;

	code = CAR(l);
	env  = add_frame(CLOSPARAMS(code), LIST1(obj), CLOSENV(code));
	/* Evaluate the closure body */
	for(code=CLOSBODY(code); NNULLP(code); code=CDR(code))
	  l = EVALCAR(code);
	return l;
      }
    }
  }
  return CALL_GF3("slot-missing", classe, obj, slot_name);
}

static SCM set_slot_value(SCM classe, SCM obj, SCM slot_name, SCM value)
{
  register SCM l;

  for (l=ACCESSORS_OF(obj); NNULLP(l); l=CDR(l)) {
    if (EQ(CAR(CAR(l)), slot_name)) {
      l = CDR(CDR(CAR(l)));
      /* Two cases here:
       *	- l is an integer (the offset of this slot in the slots vector)
       *	- otherwise (cadr l) is the setter function to apply
       */
      if (INTEGERP(l))
	THE_SLOT_OF(obj, INTEGER(l)) = value;
      else {  /* We must evaluate (apply (cadr l) (list obj value))
	       * where (cadr l) is known to be a closure of arity 2  */
	register SCM code, env;

	code = CAR(CDR(l));
	env  = add_frame(CLOSPARAMS(code), LIST2(obj, value), CLOSENV(code));
	/* Evaluate the closure body */
	for(code=CLOSBODY(code); NNULLP(code); code=CDR(code))
	  EVALCAR(code);
      }
      return UNDEFINED;
    }
  }
  return CALL_GF4("slot-missing", classe, obj, slot_name, value);
}

static void set_slot_value_if_unbound(SCM classe, SCM obj, SCM slot_name, SCM form)
{
  SCM old_val = get_slot_value(classe, obj, slot_name);
  
  if (old_val == UNBOUND)
    set_slot_value(classe, obj, slot_name, Apply(form, NIL));
}


static SCM test_slot_existence(SCM classe, SCM obj, SCM slot_name)
{
  register SCM l;

  for (l=ACCESSORS_OF(obj); NNULLP(l); l=CDR(l))
    if (EQ(CAR(CAR(l)), slot_name)) return Truth;

  return Ntruth;
}

		/* ======================================== */

static PRIMITIVE slot_ref_using_class(SCM classe, SCM obj, SCM slot_name)
{
  SCM res;

  ENTER_PRIMITIVE("slot-ref-using-class");

  if (NCLASSP(classe)) Serror("bad class",  classe);
  if (NINSTANCEP(obj)) Serror("bad object", obj);

  res = get_slot_value(classe, obj, slot_name);
  if (res == UNBOUND) 
    return CALL_GF3("slot-unbound", classe, obj, slot_name);
  return res;
}
 
static PRIMITIVE slot_set_using_class(SCM l, int len)
{
  ENTER_PRIMITIVE("slot-set-using-class!");

  if (len != 4)  Serror("bad number of parameters", NIL);
  {
    SCM classe = CAR(l);
    SCM obj    = CAR(CDR(l));
    if (NCLASSP(classe)) Serror("bad class",  classe);
    if (NINSTANCEP(obj)) Serror("bad object", obj);
    return set_slot_value(classe, obj, CAR(CDR(CDR(l))), CAR(CDR(CDR(CDR(l)))));
  }
}

static PRIMITIVE slot_boundp_using_class(SCM classe, SCM obj, SCM slot_name)
{
  ENTER_PRIMITIVE("slot-bound-using-class?");

  if (NCLASSP(classe))     Serror("bad class",  classe);
  if (NSYMBOLP(slot_name)) Serror("bad slot name",slot_name);
  if (NINSTANCEP(obj))     Serror("bad object", obj);

  return (get_slot_value(classe, obj, slot_name) == UNBOUND) ? Ntruth : Truth;
}

static PRIMITIVE slot_exists_using_class(SCM classe, SCM obj, SCM slot_name)
{
  ENTER_PRIMITIVE("slot-exists-using-class?");
  
  if (NCLASSP(classe))     Serror("bad class",    classe);
  if (NSYMBOLP(slot_name)) Serror("bad slot name",slot_name);
  if (NINSTANCEP(obj))     Serror("bad object",   obj);
  return test_slot_existence(classe, obj, slot_name);
}


		/* ======================================== */

PRIMITIVE STk_slot_ref(SCM obj, SCM slot_name)
{
  SCM res, classe;

  if (NINSTANCEP(obj)) Err("slot-ref: bad object", obj);
  TEST_CHANGE_CLASS(obj, classe);

  res = get_slot_value(classe, obj, slot_name);
  if (res == UNBOUND)
    return CALL_GF3("slot-unbound", classe, obj, slot_name);
  return res;
}

PRIMITIVE STk_slot_set(SCM obj, SCM slot_name, SCM value)
{
  SCM classe;

  if (NINSTANCEP(obj)) Err("slot-set!: bad object", obj);
  TEST_CHANGE_CLASS(obj, classe);

  return set_slot_value(classe, obj, slot_name, value);
}

static PRIMITIVE slot_boundp(SCM obj, SCM slot_name)
{
  SCM classe;

  if (NINSTANCEP(obj)) Err("slot-bound?: bad object", obj);
  TEST_CHANGE_CLASS(obj, classe);

  return (get_slot_value(classe, obj, slot_name) == UNBOUND) ? Ntruth : Truth;
}

static PRIMITIVE slot_existsp(SCM obj, SCM slot_name)
{
  SCM classe;

  ENTER_PRIMITIVE("slot-exists?");

  if (NSYMBOLP(slot_name)) Serror("bad slot name", slot_name);
  if (NINSTANCEP(obj))     Serror("bad object",    obj);
  TEST_CHANGE_CLASS(obj, classe);

  return test_slot_existence(classe, obj, slot_name);
}


/******************************************************************************
 *
 * %allocate-instance (the low level instance allocation primitive)
 *
 ******************************************************************************/

PRIMITIVE STk_allocate_instance(SCM classe)
{
  int type;

  if (NCLASSP(classe)) Err("%allocate-instance: bad class", classe);
 
  if (EQ(classe, Generic)) type= TYPE_GENERIC;
  else
    if (EQ(classe, Accessor)) type = TYPE_ACCESSOR;
    else
      if (EQ(classe, Simple_method)) type = TYPE_SIMPLE_METHOD;
      else type = TYPE_INSTANCE;
  return make_instance(classe, 
		       STk_integer_value(THE_SLOT_OF(classe, S_nfields)),
		       type);
}

/******************************************************************************
 *
 * %modify-instance (used by change-class to modify in place)
 * 
 ******************************************************************************/
static PRIMITIVE modify_instance(SCM old, SCM new)
{
  struct obj tmp;

  if (NINSTANCEP(old) || NINSTANCEP(new)) 
    Err("%modify-instance: both parameters must be instances", NIL);

  /* Exchange the data contained in old and new. We exchange rather than 
   * scratch the old value with new to be correct with GC
   */
  tmp = *old;
  *old = *new;
  *new = tmp;
  return UNDEFINED;
}

static PRIMITIVE stklos_version(void)
{
  return STk_makestring(STKLOS_VERSION);
}

/******************************************************************************
 *
 *   GGGG                FFFFF          
 *  G                    F    
 *  G  GG                FFF    
 *  G   G                F      
 *   GGG  E N E R I C    F    U N C T I O N S
 *
 * This implementation provides
 *	- generic functions (with class specializers)
 *	- multi-methods
 *	- next-method 
 *	- a hard-coded MOP for standard gf, which can be overloaded for non-std gf
 *
 ******************************************************************************/

SCM STk_make_next_method(SCM methods, SCM args, SCM gf)
{
  register SCM z, l;

  /* if gf is NIL, args already contains the GF in front of args.
   * This saves the cost of a a cons. This situation is very frequent since
   * most of the time next-method is called without parameter and we just have 
   * to propagate the previous args.
   */
  l = (NULLP(gf))? args : Cons(gf, args);

  NEWCELL(z, tc_next_method);
  CAR(z) = methods;
  CDR(z) = l;
 
  return z;
}

SCM STk_apply_next_method(SCM next, SCM provided_args)
{
  SCM methods = NXT_MTHD_METHODS(next);
  SCM args, tmp;
  
  if (NULLP(provided_args)) {
    tmp   = NXT_MTHD_ARGS(next);
    args  = CDR(tmp);
  }
  else {
    tmp  = Cons(CAR(NXT_MTHD_ARGS(next)), provided_args);
    args = provided_args;
  }
  
  if (NULLP(methods)) {
    SCM gf = CAR(NXT_MTHD_ARGS(next));
    
    return CALL_GF2("no-next-method", gf, args);    
  }
  else {
    SCM m        = CAR(methods);
    SCM new_next = STk_make_next_method(CDR(methods),tmp, NIL);
    
    /* m is the function to call with args. */
    return Apply(THE_SLOT_OF(m, S_procedure), Cons(new_next, args));
  }
}

/******************************************************************************
 * 
 * Protocol for calling a generic fumction
 * This protocol is roughly equivalent to (parameter are a little bit different 
 * for efficiency reasons):
 *
 * 	+ apply-generic (gf args)
 *		+ compute-applicable-methods (gf args ...)
 *			+ sort-applicable-methods (methods args)
 *		+ apply-methods (gf methods args)
 *				
 * apply-methods calls make-next-method to build the "continuation" of a a 
 * method.  Applying a next-method will call apply-next-method which in
 * turn will call  apply again to call effectively the following method.
 *
 ******************************************************************************/

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
    *p++ = class_of(CAR(args));
 
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
    CALL_GF2("no-applicable-method", gf, save);
    /* if we are here, it's because no-applicable-method hasn't signaled an error */
    return NIL;
  }
  return (count == 1) ? applicable : 
    			sort_applicable_methods(applicable, count, types);
}

static SCM apply_methods(SCM gf, SCM methods, SCM args)
{
  SCM m, next;
  
  if (NULLP(methods)) {
    /* 
     * methods can be NIL if we have a no-applicable-method handler which 
     * doesn't signal an error (or dont ends with a call to next-method)
     * In this case return an undefined value
     */
    return UNDEFINED;
  }
  
  m    = CAR(methods);
  next = FASTMETHODP(m) ? UNBOUND : STk_make_next_method(CDR(methods), args, gf);

  /* Next-method is set to UNBOUND for simple_method and accessors */
  return Apply(THE_SLOT_OF(m, S_procedure), Cons(next, args));
}

SCM STk_apply_generic(SCM gf, SCM args)
{
  if (NGENERICP(gf)) 			 Err("apply: bad generic function", gf);
  if (NULLP(THE_SLOT_OF(gf, S_methods))) CALL_GF2("no-method", gf, args);

  return 
    apply_methods(gf,
		  STk_compute_applicable_methods(gf,args,STk_llength(args),FALSE),
		  args);
}

SCM STk_apply_user_generic(SCM gf, SCM args)
{
  if (NGENERICP(gf)) Err("apply: bad generic function", gf);
  return CALL_GF2("apply-generic", gf, args);
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

    if (classe == Method || classe == Simple_method || classe == Accessor) {
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

  ENTER_PRIMITIVE("find-method");

  if (len == 0) Serror("no parameter list", NIL);

  gf = CAR(l); l = CDR(l);
  if (NGENERICP(gf)) 			 Serror("bad generic function",   gf);
  if (NULLP(THE_SLOT_OF(gf, S_methods))) Serror("no methods for generic", gf);

  return STk_compute_applicable_methods(gf, l, len-1, TRUE);
}

static PRIMITIVE user_more_specificp(SCM m1, SCM m2, SCM targs)
{
  SCM l, v;
  int i, len;

  ENTER_PRIMITIVE("%method-more-specific?");

  if (NMETHODP(m1)) 	            Serror("bad method",   m1);
  if (NMETHODP(m2)) 	            Serror("bad method",   m2);
  if ((len=STk_llength(targs)) < 0) Serror("bad argument", targs);

  /* Verify that all the arguments of targs are classes and place them in a vector*/
  v = STk_makevect(len, NIL);

  for (i=0, l=targs; NNULLP(l); i++, l=CDR(l)) {
    if (NCLASSP(CAR(l))) Serror("bad argument", targs);
    VECT(v)[i] = CAR(l);
  }
  return more_specificp(m1, m2, VECT(v)) ? Truth: Ntruth;
}
  
  

/******************************************************************************
 *
 * Initializations 
 *
 ******************************************************************************/

static void select_STklos_module(void)
{
  STk_selected_module = STklos = STk_make_module(Intern("stklos"));
}


static void make_stdcls(SCM *var, char *name, SCM meta, SCM super, SCM slots)
{
   SCM tmp = Intern(name);
   
   *var = STk_basic_make_class(meta, tmp, LIST1(super), slots);
   STk_gc_protect(var);
   DEFVAR(tmp, *var);
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
  make_stdcls(&Simple_method,	"<simple-method>",   Class, Method,	     NIL);
  make_stdcls(&Accessor,	"<accessor-method>", Class, Simple_method,   NIL);
  make_stdcls(&Generic,		"<generic>",	     Entity_class, Object,   tmp2);

  /* Primitive types classes */
  make_stdcls(&Boolean, 	"<boolean>",	Class, 		 Top, 	    NIL);
  make_stdcls(&Char,		"<char>",	Class, 		 Top,	    NIL);
  make_stdcls(&Liste,		"<list>",	Class, 		 Top,	    NIL);
  make_stdcls(&Pair,		"<pair>",	Class, 		 Liste,	    NIL);
  make_stdcls(&Null,		"<null>", 	Class, 		 Liste,	    NIL);
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

/**
 *
 * Definition of Extended type classes
 *
 **/

SCM STk_make_extended_class(char *name)
{
  /* 
   * This function is called when a new extended type is defined and returns 
   * a new class for it. Furthermore, it defines a global variable for this class.
   * If STklos is not initialized, it returns NIL. 
   * Extended types defined before initialization of STklos will be done
   * when STklos is inited by "define_extended_type_classes"
   */
  if (initialized) {
    SCM tmp1, tmp2;
    char buffer[200];

    sprintf(buffer, "<%s>", name);
    tmp1 	= Intern(buffer);
    tmp2 	= STk_basic_make_class(Class, tmp1, LIST1(Top), NIL);
    DEFVAR(tmp1, tmp2);
    return tmp2;
  }
  return NIL;
}

static void define_extended_type_classes(void)
{
  /* 
   * This function is called when STklos is initialized. It performs
   * the definition of classes for extended types defined before STklos
   * loading
   */
  int i;

  initialized = TRUE;
  for (i = tc_start_extd; ; i++) {
    char *name = STk_get_extended_name(i);
    
    if (!name) return;
    STk_register_extended_class(STk_make_extended_class(name), i);
  }
}

static void add_primitive(char *name, int type, void *fct_ptr)
{
  STk_add_new_primitive(name, type, fct_ptr);
  STk_export_symbol(Intern(name), STklos);
}


/*===========================================================================*/

PRIMITIVE STk_init_STklos(void)
{
  SCM curmod = STk_selected_module;

  STk_disallow_sigint();

  Top = Object = Class = Generic = Method = Simple_method = Accessor = NIL;

  select_STklos_module();
  create_Top_Object_Class();
  make_standard_classes();

  /* Define new primitives and export them */
  add_primitive("stklos-version",	    tc_subr_0, stklos_version);
  add_primitive("instance?", 	            tc_subr_1, instancep);
  add_primitive("slot-ref-using-class",	    tc_subr_3, slot_ref_using_class);
  add_primitive("slot-set-using-class!",    tc_lsubr,  slot_set_using_class);
  add_primitive("slot-bound-using-class?",  tc_subr_3, slot_boundp_using_class);
  add_primitive("slot-exists-using-class?", tc_subr_3, slot_exists_using_class);
  add_primitive("slot-ref",		    tc_subr_2, STk_slot_ref);
  add_primitive("slot-set!",	            tc_subr_3, STk_slot_set);
  add_primitive("slot-bound?",	            tc_subr_2, slot_boundp);

  add_primitive("class-of",		    tc_subr_1, class_of); 
  add_primitive("class-name",	            tc_subr_1, class_name);
  add_primitive("class-direct-supers",      tc_subr_1, class_direct_supers);
  add_primitive("class-direct-subclasses",  tc_subr_1, class_direct_subs);
  add_primitive("class-direct-methods",	    tc_subr_1, class_direct_methods);
  add_primitive("class-direct-slots",       tc_subr_1, class_direct_slots);
  add_primitive("class-precedence-list",    tc_subr_1, class_cpl);
  add_primitive("class-slots",	            tc_subr_1, class_slots);
  add_primitive("class-environment",	    tc_subr_1, class_environment);
  add_primitive("generic-function-name",    tc_subr_1, gf_name);
  add_primitive("generic-function-methods", tc_subr_1, gf_methods);
  add_primitive("method-generic-function",  tc_subr_1, method_gf);
  add_primitive("method-specializers",	    tc_subr_1, method_specializers);
  add_primitive("method-procedure",	    tc_subr_1, method_procedure);
  add_primitive("slot-exists?",	            tc_subr_2, slot_existsp);

  add_primitive("make",	  		    tc_lsubr,  lmake);
  add_primitive("find-method",		    tc_lsubr,  find_method);

  /* Unexported primitives */
  STk_add_new_primitive("%allocate-instance",     tc_subr_1, STk_allocate_instance);
  STk_add_new_primitive("%initialize-object",     tc_subr_2, initialize_object);
  STk_add_new_primitive("%compute-slots",         tc_subr_1, compute_slots);
  STk_add_new_primitive("%modify-instance",       tc_subr_2, modify_instance);
  STk_add_new_primitive("%method-more-specific?", tc_subr_3, user_more_specificp);
  STk_add_new_primitive("%fast-slot-ref",         tc_subr_2, fast_slot_ref);
  STk_add_new_primitive("%fast-slot-set!",        tc_subr_3, fast_slot_set);

  /* Define classes for already defined extended type */
  define_extended_type_classes();

  STk_allow_sigint();

  /* Reset the default module we have to the one we have when starting inits */
  STk_selected_module = curmod;

  return UNDEFINED;
}

#else 
  /* Don't produce an empty object file since some compiler (loader?) don't 
   * manage them correctly 
   */
  static char not_used = '?';
#endif
