/*
 *
 *  s t k l o s . h			-- STklos support
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
 * Last file update: 24-Jul-1996 17:04 
 */

typedef struct stklos_instance {
  char instance_type;
  SCM classe;	  		/* Name is choosed to avoid conflicts with C++ */
  SCM accessors;
  int number_of_slots;
  SCM slots[1];			/* Must be last !! */
} Instance;


#define TYPE_INSTANCE	    00
#define TYPE_GENERIC	    01


#define INST(x)	    	    ((x)->storage_as.instance.data)
#define INSTID(x)	    ((x)->storage_as.instance.id)
#define CLASS_OF(x) 	    (INST(x)->classe)
#define INST_TYPE(x)	    (INST(x)->instance_type)
#define ACCESSORS_OF(x)     (INST(x)->accessors)
#define NUMBER_OF_SLOTS(x)  (INST(x)->number_of_slots)
#define INSTANCEP(x)	    (TYPEP(x, tc_instance))
#define NINSTANCEP(x)	    (NTYPEP(x, tc_instance))
#define PUREGENERICP(x)	    (INSTANCEP(x) && (INST(x)->instance_type&TYPE_GENERIC))

#define THE_SLOT_OF(x, i)   (INST(x)->slots[i])
#define SUBCLASSP(c1, c2)   (STk_memq(c2, THE_SLOT_OF(c1, S_cpl)) != Ntruth)

#define NXT_GF(x)	    (CAR(CAR(x))
#define NXT_METHODS(x)	    (CDR(CAR(x))
#define NXT_ARGS(x)	    (CDR(x))


#define S_name 			0 	/*a symbol*/
#define S_direct_supers 	1 	/* (class ...) */
#define S_direct_slots		2 	/* ((name . options) ...) */
#define S_cpl			3 	/* (class ...) */
#define S_slots			4	/* ((name . options) ...) */
#define S_nfields		5	/* an integer */
#define S_initializers		6	/* (proc ...) */
#define S_getters_n_setters	7	/* ((slot getter setter) ...) */
#define NUMBER_OF_CLASS_SLOTS	8	


#define S_methods		1	/* offset of methods slot in a <generic> */

#define S_generic_function	0	/* offset of gf    slot in a <method> */
#define S_specializers		1	/* offset of spec. slot in a <method> */
#define S_procedure 		2	/* offset of proc. slot in a <method> */

#define STk_tc_instance tc_instance	/* for compatibility with older versions */
#define clath		classe		/* for compatibility with older versions */

/* Low level functions exported */
SCM STk_make_instance(SCM classe, long  size, int type);
SCM STk_make_next_method(SCM methods, SCM args);
SCM STk_basic_make_class(SCM classe, SCM name, SCM dsupers, SCM dslots);
SCM STk_apply_generic(SCM gf, SCM args);


PRIMITIVE STk_allocate_instance(SCM classe);
PRIMITIVE STk_slot_ref(SCM obj, SCM slot_name);
PRIMITIVE STk_slot_set(SCM obj, SCM slot_name, SCM value);
PRIMITIVE STk_class_of(SCM obj);

SCM STk_compute_applicable_methods(SCM gf, SCM args, int len, int find_method);
SCM STk_apply_user_generic(SCM gf, SCM args);
SCM STk_apply_next_method(SCM next, SCM provided_args);
