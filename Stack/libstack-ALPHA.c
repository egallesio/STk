/******************************************************************************
 *
 * l i b s t a c k - A L P H A . c
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

/*
  The first 6 arguments are passed in registers $16-$21 for integers,
  in registers $f16-$f21 for floats. From then on, everything is passed
  on the stack.

  Everything on the stack is word-aligned.
*/

#include "libstack.h"

fake_type stk_argv[MAX_EXTERNAL_PARAM];
int stk_argc = 0;

extern void stk_copy_float_registers(void);


int stk_push_float(float f)
{
  if (stk_argc < 6) {
    /* Value must goes in a register, store it as a double */
    stk_argv[stk_argc++].d=(double)f;
    return 0;
  }
  if (stk_argc < MAX_EXTERNAL_PARAM) {
    /* Value will be stacked as a float */
    stk_argv[stk_argc++].f[0]=f;
    return 0;
  }
  return -1;
}


/* Since we do not know how many floats are passed (and where), we set all
 * float  registers before calling the function 
 */

#define alpha_big_switch {stk_copy_float_registers(); big_switch;}
  
long stk_call_long(void *func)
{
  long (*f)() = func;
  alpha_big_switch;
}

double stk_call_double(void *func)
{
  double (*f)() = func;
  alpha_big_switch;
}


float stk_call_float(void *func)
{
  float (*f)() = func;
  alpha_big_switch;
}


