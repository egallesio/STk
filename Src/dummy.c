/******************************************************************************
 *
 * d u m m y . c		-- this file contains functions referenced
 *				   (but not used) by the Tcl library.
 *				   It is useful for the snow interpreter only.
 *				   Dummy functions are defined here to make the 
 *				   loader happy
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
 *    Creation date:  1-Feb-1995 22:43
 * Last file update: 21-Sep-1996 22:38
 *
 ******************************************************************************/

#include "stk.h"


#ifndef USE_TK

static Error(char *msg)
{
  char buffer[100];

  sprintf(buffer, "Error: call of '%s' in Snow !!!. This should not occur\n", msg);
}

char *Tcl_SetVar2(interp, name1, name2, val, flags)
     Tcl_Interp *interp;
     char *name1, *name2, *val;
     int flags;
{
  Error("Tcl_SetVar2");
}

Tcl_File Tcl_GetFile(ClientData fileData, int type)
{
  Error("Tcl_GetFile");
  return (Tcl_File) TCL_ERROR;
}

void Tcl_DeleteFileHandler(Tcl_File file)
{
  Error("Tcl_DeleteFileHandler");
}

void Tcl_CreateFileHandler(Tcl_File file, int mask, Tcl_FileProc *proc,
			    ClientData clientData)
{
  Error("Tcl_CreateFileHandler");
}
#endif

