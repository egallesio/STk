/*
 *
 *  p r o m i s e . c		-- Promises management
 *
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
 *
 *            Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date:  2-Jun-1993 12:27
 * Last file update:  3-Sep-1999 20:22 (eg)
 *
 */

#include "stk.h"

/*
 * Delay is implemented by syntax_delay in syntax.c
 */

PRIMITIVE STk_force(SCM promise)
{
  SCM z;

  if (NPROMISEP(promise)) return promise;

  if (promise->storage_as.promise.resultknown) 
    /* promise was alraedy evaluated. Simply return expr field */
    return promise->storage_as.promise.expr;

  z = Apply(promise->storage_as.promise.expr, NIL);

  if (promise->storage_as.promise.resultknown)
    /* R4RS: "A promise may refer to its own value.... Forcing such
     * a promise may cause the promise to be forced a second time before
     * the first value has been computed.
     */
    return promise->storage_as.promise.expr;
  else {
    promise->storage_as.promise.expr 	     = z;
    promise->storage_as.promise.resultknown  = 1;
    return z;
  }
}

PRIMITIVE STk_promisep(SCM promise)
{
  return PROMISEP(promise)? Truth: Ntruth;
}
