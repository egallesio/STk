/******************************************************************************
 *
 * s t k . c
 *
 * Copyright © 1993-1997 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *    Creation date: 12-May-1993 10:34
 * Last file update:  1-Jan-1997 11:43
 *
 ******************************************************************************/
#define STK_MAIN
#include "stk.h"

int main(int argc,char **argv)
{
  STk_toplevel(argc, argv);
  return 0;
}
