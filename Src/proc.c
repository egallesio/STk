/*
 *
 * p r o c . c			-- 
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 15-Nov-1993 22:02
 * Last file update: 13-May-1996 22:41
 */

#include "stk.h"
#include "extend.h"

/**** Section 6.9 ****/

int STk_is_thunk(SCM obj)
{
  switch (TYPE(obj)) {
    case tc_closure:	/* We can be more clever here.... */
#ifdef USE_STKLOS
    case tc_instance:
#endif
    case tc_subr_0:
    case tc_subr_0_or_1: return TRUE;
  }
  return FALSE;
}


PRIMITIVE STk_procedurep(SCM obj)
{
  switch (TYPE(obj)) {
    case tc_subr_0: 	
    case tc_subr_1: 	
    case tc_subr_2:  
    case tc_subr_3:	
    case tc_subr_0_or_1:	
    case tc_subr_1_or_2:
    case tc_subr_2_or_3:
    case tc_lambda:	
    case tc_lsubr:		
    case tc_ssubr:
    case tc_closure:
    case tc_cont:
    case tc_apply:
    case tc_call_cc:
    case tc_dynwind: 
#ifdef USE_STKLOS
    case tc_instance:
    case tc_next_method:
#endif
#ifdef USE_TK
    case tc_tkcommand:
#endif
			return Truth;
    default: 		if (EXTENDEDP(obj))
      			  return STk_extended_procedurep(obj) ? Truth : Ntruth;
			else 
			  return Ntruth;
  }
}


static SCM general_map(SCM l, int map, int len)
{
  register int i;
  SCM res = NIL,*tmp = &res;
  SCM fct, args;

  if (NCONSP(l)) goto error;

  fct  = CAR(l);
  len -= 1;
  args = STk_vector(CDR(l), len);

  for ( ; ; ) {
    /* Build parameter list */
    for (l=NIL, i=len-1; i >= 0; i--) {
      if (NULLP(VECT(args)[i])) return res;
      if (NCONSP(VECT(args)[i])) goto error;

      l             = Cons(CAR(VECT(args)[i]), l);
      VECT(args)[i] = CDR(VECT(args)[i]);
    }

    /* See if it's a map or a for-each call */
    if (map) {
      *tmp = Cons(Apply(fct, l), NIL);
      tmp  = &CDR(*tmp);
    }
    else Apply(fct, l);
  }
error:
  { 
    char buff[50];
    sprintf(buff, "%s: malformed list", map? "map" : "for-each");
    Err(buff, l);
  }
}

PRIMITIVE STk_map(SCM l, int len)
{
  return general_map(l, 1, len);
}

PRIMITIVE STk_for_each(SCM l, int len)
{
  return general_map(l, 0, len);
}

PRIMITIVE STk_procedure_body(SCM proc)
{
  return TYPEP(proc, tc_closure) ? Cons(Sym_lambda, proc->storage_as.closure.code)
    				 : Ntruth;
}

PRIMITIVE STk_procedure_environment(SCM proc)
{
  return TYPEP(proc, tc_closure) ? STk_makeenv(proc->storage_as.closure.env,0)
    				 : Ntruth;    
}
