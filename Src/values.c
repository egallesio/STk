/*
 *
 *  v a l u e s . c			-- R5RS values
 *
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
 * $Id: values.c 1.1 Fri, 10 Apr 1998 07:13:18 +0000 eg $
 *
 *            Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date:  8-Mar-1998 10:19
 * Last file update:  8-Mar-1998 11:54
 *
 */
#include "stk.h"

PRIMITIVE STk_values(SCM l, int len)
{
  SCM z;
  
  if (len == 1)
    return CAR(l);
  else {
    NEWCELL(z, tc_values);
    CAR(z) = l;
    return z;
  }
}


PRIMITIVE STk_call_with_values(SCM producer, SCM consumer)
{
  SCM res;

  ENTER_PRIMITIVE("call-with-values");

  if (!STk_procedurep(producer)) Serror("bad producer", producer);
  if (!STk_procedurep(consumer)) Serror("bad consumer", consumer);

  res = Apply(producer, NIL);
  return Apply(consumer, VALUESP(res) ? CAR(res) : LIST1(res));
}

  
  
