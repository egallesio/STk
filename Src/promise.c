/*
 *
 *  p r o m i s e . c		-- Promises management
 *
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
 *            Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date:  2-Jun-1993 12:27
 * Last file update: 10-Dec-1995 22:31
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
