/******************************************************************************
 *
 * s t k . c
 *
 * Copyright © 1993-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *    Creation date: 12-May-1993 10:34
 * Last file update:  3-Sep-1999 20:23 (eg)
 *
 ******************************************************************************/
#define STK_MAIN
#include "stk.h"

int main(int argc,char **argv)
{
  STk_toplevel(argc, argv);
  return 0;
}
