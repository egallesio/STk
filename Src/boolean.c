/*
 *
 * b o o l e a n . c			-- Booleans and Equivalence predicates
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
 *    Creation date: 23-Oct-1993 21:37
 * Last file update:  3-Sep-1999 20:18 (eg)
 */

#include "stk.h"
#include "extend.h"
#ifdef USE_STKLOS
#  include "stklos.h"
#endif

PRIMITIVE STk_not(SCM x)
{
  return EQ(x, Ntruth) ? Truth : Ntruth;
}

PRIMITIVE STk_booleanp(SCM x)
{
  return BOOLEANP(x) ? Truth: Ntruth;
}

PRIMITIVE STk_eq(SCM x,SCM y)
{
  return EQ(x,y) ? Truth : Ntruth;
}

PRIMITIVE STk_eqv(SCM x, SCM y)
{
  if (EQ(x,y)) return(Truth);
 
  switch (TYPE(x)) {
    case tc_symbol: if (SYMBOLP(y) && strcmp(PNAME(x), PNAME(y)) == 0)
      		      return Truth;
    		    break;
    case tc_integer:
    case tc_flonum:
    case tc_bignum: if (NUMBERP(y)) {
      		      if ((EXACTP(x) && FLONUMP(y)) || (FLONUMP(x) && EXACTP(y))) 
			return Ntruth;
		      return (STk_equal_numbers(x,  y)) ? Truth : Ntruth;
    		    }
    		    break;
#ifndef COMPACT_SMALL_CST
    case tc_char:   if (CHARP(y) && CHAR(x) == CHAR(y)) return Truth;
		    break;
#endif		      
    case tc_keyword: if (KEYWORDP(y) && strcmp(KEYVAL(x), KEYVAL(y)) == 0)
      		     return Truth;
    		     break;
    case tc_env:     if (EQ(x->storage_as.env.data, y->storage_as.env.data))
		       return Truth;
		     break;
#ifdef USE_STKLOS
    case tc_instance: {
      			SCM fct = STk_STklos_value(Intern("object-eqv?"));

			/* Test for UNBOUND, cause gf are not defined during boot */
			return (fct == UNBOUND) ? Ntruth : Apply2(fct, x, y);
    		      }
#endif
    default: 	     if (EXTENDEDP(x) && EXTENDEDP(y) && TYPE(x) == TYPE(y)) 
      			return STk_extended_compare(x, y, FALSE);
  }
  /* What can we do else? */
  return Ntruth;
}


PRIMITIVE STk_equal(SCM x, SCM y)
{
Top:
  if (STk_eqv(x, y) == Truth) return Truth;

  switch (TYPE(x)) {
    case tc_cons:   if (CONSP(y)) {
      		      if (STk_equal(CAR(x), CAR(y)) == Ntruth) return Ntruth;
		      x = CDR(x); y = CDR(y);
		      goto Top;
		    }
		    break;
    case tc_string: if (STRINGP(y)) 
                      return (strcmp(CHARS(x), CHARS(y))==0) ? Truth: Ntruth;
                    break;
    case tc_vector: if (VECTORP(y)) {
      		      long lx, ly, i;
		      SCM *vx, *vy;
		      
		      lx = VECTSIZE(x); ly = VECTSIZE(y);
 		      if (lx != ly) return Ntruth;

		      for (i=0, vx=VECT(x), vy=VECT(y); i < lx;  i++)
			if (STk_equal(vx[i], vy[i]) == Ntruth) return Ntruth;
		      return Truth;
    		    }
    		    break;
#ifdef USE_STKLOS
    case tc_instance: {
      			SCM fct = STk_STklos_value(Intern("object-equal?"));

			/* Test for UNBOUND, cause gf are not defined during boot */
			return (fct == UNBOUND) ? Ntruth : Apply2(fct, x, y);
    		      }
#endif
    default:	    if (EXTENDEDP(x) && EXTENDEDP(y) && TYPE(x) == TYPE(y)) 
      		      return STk_extended_compare(x, y, TRUE);
  }
  return Ntruth;
}


