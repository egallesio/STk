/******************************************************************************
 *
 * d u m m y . c		-- this file contains functions referenced
 *				   (but not used) by the Tcl library.
 *				   It is useful for the snow interpreter only.
 *				   Dummy functions are defined here to make the 
 *				   loader happy
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
 *    Creation date:  1-Feb-1995 22:43
 * Last file update:  3-Sep-1999 20:19 (eg)
 *
 ******************************************************************************/

#include "stk.h"


#ifndef USE_TK

static void Error(char *msg)
{
  Fprintf(STk_curr_eport,
	  "Error: call of '%s' in Snow !!!. This should not occur\n",
	  msg);
}

char *Tcl_SetVar2(interp, name1, name2, val, flags)
     Tcl_Interp *interp;
     char *name1, *name2, *val;
     int flags;
{
  Error("Tcl_SetVar2");
}

void Tcl_DeleteFileHandler(int fd)
{
  Error("Tcl_DeleteFileHandler");
}

void Tcl_CreateFileHandler(int fd, int mask, Tcl_FileProc *proc,
			    ClientData clientData)
{
  Error("Tcl_CreateFileHandler");
}
#endif

