/******************************************************************************
 *
 * l i b s t a c k - M I P S . c
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
 *    Creation date: 29-Sep-1997 14:56
 * Last file update:  3-Sep-1999 21:06 (eg)
 *
 ******************************************************************************/

#include "libstack.h"

/*
  Doubles are 2 words aligned on even boundaries.
  If the first two args are floats or doubles, they are also passed in $f12
  and $f14. But varargs functions will expect them in the integer registers
  and we can't tell whether the function is varargs so we pass them both ways.
*/

fake_type stk_argv[MAX_EXTERNAL_PARAM];
int stk_argc = 0;

double stk_double_register1, stk_double_register2;

int stk_push_float(float f)
{
  switch (stk_argc) {
    case 0: stk_double_register1 = (double) f; 
      	    stk_argv[stk_argc++].f[1]=f;
	    return 0;
    case 1: stk_double_register2 = (double) f; 
      	    stk_argv[stk_argc++].f[1]=f;
	    return 0;
    default: if (stk_argc < MAX_EXTERNAL_PARAM) {
	      /* Value will be stacked as a float */
      	      stk_argv[stk_argc++].f[0]=f;
	      return 0;
    	     }
  }
  return -1;
}

int stk_push_double(double d)
{
  if (stk_argc < 4) {
    stk_double_register1 = stk_argv[stk_argc].d= d; 
    stk_argv[stk_argc+1].f[1]= stk_argv[stk_argc].f[0];
    stk_argc += 2;
    return 0;
  }
  else if (stk_argc < MAX_EXTERNAL_PARAM) {
    /* Value will be stacked as a float */
    stk_argv[stk_argc].d= d;
    stk_argv[stk_argc+1].f[1]= stk_argv[stk_argc].f[0];
    stk_argc += 2;
    return 0;
  }
  return -1;
}


/* Since we do not know how many floats are passed (and where), we set all
 * float  registers before calling the function 
 */

#define mips_big_switch {stk_copy_float_registers(); big_switch;}
  
long stk_call_long(void *func)
{
  long (*f)() = func;
  mips_big_switch;
}

double stk_call_double(void *func)
{
  double (*f)() = func;
  mips_big_switch;
}

float stk_call_float(void *func)
{
  float (*f)() = func;
  mips_big_switch;
}
