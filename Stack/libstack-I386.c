/******************************************************************************
 *
 * l i b s t a c k - I 3 8 6 . c
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
 *    Creation date: 28-Sep-1997 17:30
 * Last file update:  3-Sep-1999 21:05 (eg)
 *
 ******************************************************************************/

/* All arguments are passed on the stack with word alignment. Doubles use 2 words */


#include "libstack.h"

fake_type stk_argv[MAX_EXTERNAL_PARAM];
int stk_argc = 0;

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
