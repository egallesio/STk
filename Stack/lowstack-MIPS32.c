/******************************************************************************
 *
 * l o w s t a c k - M I P S 3 2 . c
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
 *    Creation date: 29-Sep-1997 15:10
 * Last file update: 29-Sep-1997 17:56
 *
 ******************************************************************************/

/*
  Doubles are 2 words aligned on even boundaries.
  If the first two args are floats or doubles, they are also passed in $f12
  and $f14. But varargs functions will expect them in the integer registers
  and we can't tell whether the function is varargs so we pass them both ways.
*/

extern double stk_double_register1, stk_double_register2;

void stk_copy_float_registers(void)
{
  /* Values which must be passed as regsister are kept in globals */
  register double f0 /* asm ("$f12") */ = stk_double_register1;
  register double f1 /* asm ("$f14") */ = stk_double_register2;
}
