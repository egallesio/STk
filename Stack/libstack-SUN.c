/******************************************************************************
 *
 * l i b s t a c k - S U N . c
 *
 * Copyright © 1997 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 28-Sep-1997 17:30
 * Last file update: 28-Sep-1997 18:28
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

float stk_call_float(void *func)
{
  float (*f)() = func;
  big_switch;
}
