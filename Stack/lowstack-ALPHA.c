/******************************************************************************
 *
 * l o w s t a c k - A L P H A . c
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
 * Last file update:  3-Sep-1999 21:06 (eg)
 *
 ******************************************************************************/

/*
  The first 6 arguments are passed in registers $16-$21 for integers,
  in registers $f16-$f21 for floats. From then on, everything is passed
  on the stack.

  Everything on the stack is word-aligned.
*/

#include "libstack.h"

void stk_copy_float_registers(void)
{
  register double f0 asm ("$f16") = stk_argv[0].d;
  register double f1 asm ("$f17") = stk_argv[1].d;
  register double f2 asm ("$f18") = stk_argv[2].d;
  register double f3 asm ("$f19") = stk_argv[3].d;
  register double f4 asm ("$f20") = stk_argv[4].d;
  register double f5 asm ("$f21") = stk_argv[5].d;
}
