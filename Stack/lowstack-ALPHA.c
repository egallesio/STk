/******************************************************************************
 *
 * l o w s t a c k - A L P H A . c
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
 * Last file update: 29-Sep-1997 11:29
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
