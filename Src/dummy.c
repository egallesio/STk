/******************************************************************************
 *
 * d u m m y . c		-- this file contains functions referenced
 *				   (but not used) by the Tcl library.
 *				   It is useful for the snow interpreter only.
 *				   Dummy functions are defined here to make the 
 *				   loader happy
 *
 * Copyright © 1993-1998 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *    Creation date:  1-Feb-1995 22:43
 * Last file update: 11-Nov-1998 10:53
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

