/*
 *
 *  s t k l o s . h			-- STklos support
 *
 * Copyright © 1993-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
 * 
 *
 * Permission to use, copy, modify, distribute,and license this
 * software and its documentation for any purpose is hereby granted,
 * provided that existing copyright notices are retained in all
 * copies and that this notice is included verbatim in any
 * distributions.  No written agreement, license, or royalty fee is
 * required for any of the authorized uses.
 * This software is provided ``AS IS'' without express or implied
 * warranty.
 *
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  9-Feb-1994 15:56
 * Last file update:  3-Sep-1999 20:58 (eg)
 */

#define STKLOS_VERSION	STK_VERSION /* Keep STklos version in sync with STk now */

typedef struct stklos_instance {
  SCM classe;	  		/* Name is choosed to avoid conflicts with C++ */
  SCM accessors;
  int number_of_slots;
  SCM slots[1];			/* Must be last !! */
} Instance;


#define TYPE_INSTANCE	    0x00
#define TYPE_GENERIC	    0x01
#define TYPE_SIMPLE_METHOD  0x02
#define TYPE_ACCESSOR	    0x04


#define INST(x)	    	    ((x)->storage_as.instance.data)
#define INST_TYPE(x)	    ((x)->storage_as.instance.type_flags)
#define CLASS_OF(x) 	    (INST(x)->classe)
#define ACCESSORS_OF(x)     (INST(x)->accessors)
#define NUMBER_OF_SLOTS(x)  (INST(x)->number_of_slots)
#define INSTANCEP(x)	    (TYPEP(x, tc_instance))
#define NINSTANCEP(x)	    (NTYPEP(x, tc_instance))

#define PUREGENERICP(x)	    (INST_TYPE(x) & TYPE_GENERIC)
#define SIMPLEMETHODP(x)    (INST_TYPE(x) & TYPE_SIMPLE_METHOD)
#define ACCESSORP(x)	    (INST_TYPE(x) & TYPE_ACCESSOR)
#define FASTMETHODP(x)	    (INST_TYPE(x) & (TYPE_ACCESSOR|TYPE_SIMPLE_METHOD))

#define THE_SLOT_OF(x, i)   (INST(x)->slots[i])
#define SUBCLASSP(c1, c2)   (STk_memq(c2, THE_SLOT_OF(c1, S_cpl)) != Ntruth)


#define S_name 			0 	/* a symbol */
#define S_direct_supers 	1 	/* (class ...) */
#define S_direct_slots		2 	/* ((name . options) ...) */
#define S_direct_subclasses	3	/* (class ...) */
#define S_direct_methods	4	/* (methods ...) */
#define S_cpl			5 	/* (class ...) */
#define S_slots			6	/* ((name . options) ...) */
#define S_nfields		7	/* an integer */
#define S_getters_n_setters	8	/* ((slot getter setter) ...) */
#define S_redefined		9	/* the class to which class was redefined */
#define S_environment		10	/* The environme in which class is built  */
#define NUMBER_OF_CLASS_SLOTS	11


#define S_methods		1	/* offset of methods slot in a <generic> */

#define S_generic_function	0	/* offset of gf    slot in a <method> */
#define S_specializers		1	/* offset of spec. slot in a <method> */
#define S_procedure 		2	/* offset of proc. slot in a <method> */

#define STk_tc_instance tc_instance	/* for compatibility with older versions */
#define clath		classe		/* for compatibility with older versions */

/* Low level functions exported */
void STk_free_instance(SCM o);
SCM STk_STklos_value(SCM symbol);
SCM STk_make_next_method(SCM methods, SCM args, SCM gf);
SCM STk_basic_make_class(SCM classe, SCM name, SCM dsupers, SCM dslots);
SCM STk_apply_generic(SCM gf, SCM args);

SCM STk_make_extended_class(char *name);

/* Primitives exported */
PRIMITIVE STk_allocate_instance(SCM classe);
PRIMITIVE STk_slot_ref(SCM obj, SCM slot_name);
PRIMITIVE STk_slot_set(SCM obj, SCM slot_name, SCM value);
PRIMITIVE STk_class_of(SCM obj);

SCM STk_compute_applicable_methods(SCM gf, SCM args, int len, int find_method);
SCM STk_apply_user_generic(SCM gf, SCM args);
SCM STk_apply_next_method(SCM next, SCM provided_args);
int STk_methodp(SCM obj);

