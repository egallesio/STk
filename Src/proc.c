/*
 *
 * p r o c . c			-- 
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 15-Nov-1993 22:02
 * Last file update:  3-Sep-1999 20:22 (eg)
 */

#include "stk.h"
#include "extend.h"
#ifdef USE_STKLOS
#  include "stklos.h"
#endif

/**** Section 6.9 ****/

int STk_is_thunk(SCM obj)
{
  switch (TYPE(obj)) {
#ifdef USE_STKLOS
    case tc_instance: return STk_methodp(obj);
#endif
    case tc_closure:	/* We can be more clever here.... */
    case tc_lsubr:
    case tc_subr_0:
    case tc_subr_0_or_1: return TRUE;
  }
  return FALSE;
}


SCM STk_makeclosure(SCM code, SCM env)
{
  SCM z, tmp;
  register int arity = 0;

  /* Find procedure arity */
  for (tmp = CAR(code); CONSP(tmp); tmp = CDR(tmp))
    arity += 1;
  if (NNULLP(tmp)) arity = -(arity+1);

  NEWCELL(z, tc_closure);
  CLOSCODE(z)  = code;
  CLOSENV(z)   = env;
  CLOSARITY(z) = arity;
  return z;
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
    case tc_dynwind:    return Truth;
#ifdef USE_STKLOS
    case tc_instance:   return STk_methodp(obj) ? Truth: Ntruth;
    case tc_next_method:return Truth;
#endif
#ifdef USE_TK
    case tc_tkcommand:  return Truth;
#endif
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

  if (len <= 1) goto error;

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
  return UNDEFINED; /* never reached */
}

PRIMITIVE STk_map(SCM l, int len)
{
  return general_map(l, 1, len);
}

PRIMITIVE STk_for_each(SCM l, int len)
{
  general_map(l, 0, len);
  return UNDEFINED;
}

PRIMITIVE STk_procedure_body(SCM proc)
{
  return TYPEP(proc, tc_closure) ? Cons(Sym_lambda, CLOSCODE(proc)) : Ntruth;
}

PRIMITIVE STk_procedure_environment(SCM proc)
{
  return TYPEP(proc, tc_closure) ? STk_makeenv(CLOSENV(proc), 0) : Ntruth;
}

PRIMITIVE STk_procedure_arity(SCM proc)
{
  if (NTYPEP(proc, tc_closure)) Err("%procedure-arity: bad closure", proc);
  return STk_makeinteger(CLOSARITY(proc));
}
