/******************************************************************************
 *
 * w s t k . c
 *
 * Copyright © 1993-1996 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 * Last file update: 21-Jul-1996 11:50
 *
 ******************************************************************************/
#define STK_MAIN

#include <dos.h>
#include <locale.h>
#include "stk.h"
#include "tkWinInt.h"

void WishPanic _ANSI_ARGS_(TCL_VARARGS(char *,format));

int APIENTRY WinMain(HINSTANCE hInstance, 
			  HINSTANCE hPrevInstance, 
			  LPSTR lpszCmdLine, 
			  int nCmdShow)
{
  char **argv;
  int argc;
 
  /* Set up the default locale to be Windows ANSI character set.  */
  setlocale(LC_ALL, "");
  
  Tcl_SetPanicProc(STk_panic);

  argv = (char **) STk_Win32_make_argc_argv(lpszCmdLine, &argc);

  TkWinXInit(hInstance);
  
  /*
   * Increase the application queue size from default value of 8.
   * At the default value, cross application SendMessage of WM_KILLFOCUS
   * will fail because the handler will not be able to do a PostMessage!
   * This is only needed for Windows 3.x, since NT dynamically expands
   * the queue.
   */
  SetMessageQueue(64);
  
  STk_toplevel(argc, argv);
  return 0;
}


