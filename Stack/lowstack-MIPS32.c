/******************************************************************************
 *
 * l o w s t a c k - M I P S 3 2 . c
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
 *    Creation date: 29-Sep-1997 15:10
 * Last file update:  3-Sep-1999 21:06 (eg)
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
