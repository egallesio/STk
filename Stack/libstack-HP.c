/******************************************************************************
 *
 * l i b s t a c k - H P . c
 *
 * Copyright © 1997-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 29-Sep-1997 12:18
 * Last file update:  3-Sep-1999 21:05 (eg)
 *
 ******************************************************************************/

#include "libstack.h"
/* 
   All arguments, except the first 4 words, are passed on the stack
   with word alignment. Doubles take two words and force double
   alignment. 
   The first 2 words are passed like this:
       %r26 = first integer arg, %r25 = second integer arg, or
       %r26 = high word of double arg, %r25 = low word of double arg.
   Similarly for the next 2 words, passed in %r24 and %r23.
*/

fake_type stk_argv[MAX_EXTERNAL_PARAM];
int stk_argc = 0;


int stk_push_double(double d)
{
  if (stk_argc & 1) /* align */ stk_argc++;

  if ((stk_argc < MAX_EXTERNAL_PARAM-1)) {
    stk_argv[stk_argc+1].d  = d;
    stk_argv[stk_argc].i[1]= stk_argv[stk_argc].i[0];
    stk_argc +=2;
    return 0;
  }
  return -1;
}

long stk_call_long(void *func)
{
  long (*f)() = func;
  big_switch;
}

double stk_call_double(void *func)
{
  double (*f)() = func;
  big_switch;
}

float stk_call_float(void *func)
{
  float (*f)() = func;
  big_switch;
}
