/* 
 * t k - m a i n . c 			-- Initialization of Tk
 *
 * This code initializes the Tk library. It corresponds to a part of the 
 * file main.c of the wish interpreter. 
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 13-May-1993 10:59
 * Last file update: 15-Sep-1998 14:45
 *
 *
 * Code used here was originally copyrigthed as shown below:
 *      Copyright 1990-1992 Regents of the University of California.
 *
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
 */
#ifdef USE_TK
#include <signal.h>
#include "stk.h"
#include <tkInt.h>


/*
 * Global variables used by the main program:
 */

Tcl_Interp *STk_main_interp= NULL;	/* Interpreter for this application. */
SCM STk_Tk_module;			/* The Tk module */
int Tk_initialized = 0;			/* 1 when Tk is fully initialized */


/*
 *
 * Tcl-main
 *
 *  Perhaps, this procedure should be made available outside and the
 *  USE_TK conditionnal flags should be split in USE_TCL and USE_TK
 *
 */

static void Tcl_main(void)
{
  STk_main_interp = Tcl_CreateInterp();
  STk_init_tracevar(); 	/* Initialize the variable tracing mechanism */
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_main
 *
 *----------------------------------------------------------------------
 */

void Tk_main(int synchronize, char *name, char *fileName, char *Xdisplay,
	     char *geometry)
{
  char *p, *Class;
  int argc, code;
  char *args[20];  

  Tcl_main();

  /* First, create the Tk module */
  STk_Tk_module = STk_make_module(Intern("tk"));
  
  /* Find the name of the application */
  if (name == NULL) {
    p    = (fileName != NULL) ? fileName: STk_Argv0;
    name = strrchr(p, '/');
    if (name != NULL) 
      name++;
    else 
      name = p;
  }

  /*
   * Initialize the Tk application and arrange to map the main window
   * after the startup script has been executed, if any.  This way
   * the script can withdraw the window so it isn't ever mapped
   * at all.
   */
#ifdef FIXME
  Class = (char *) ckalloc((unsigned) (strlen(name) + 1));
  strcpy(Class, name);
  Class[0] = toupper((unsigned char) Class[0]); 
#else
  Class = "STk";
#endif

#ifdef FIXME
  ckfree(Class);
#endif

  args[0] = "toplevel";
  args[1] = ".";
  args[2] = "-class";
  args[3] = Class;
  argc = 4;

  if (Xdisplay != NULL) {
    args[argc] = "-screen";
    args[argc+1] = Xdisplay;
    argc += 2;
  }

  if (STk_arg_colormap) {
    args[argc]   = "-colormap";
    args[argc+1] = "new";
    argc += 2;
  }
  if (STk_arg_visual != NULL) {
      args[argc] = "-visual";
      args[argc+1] = STk_arg_visual;
      argc += 2;
  }
  args[argc] = NULL;
  
  Tcl_ResetResult(STk_main_interp);

  code = TkCreateFrame((ClientData) NULL, STk_main_interp, argc, args, 1, name);
  if (code != TCL_OK) {
    panic(STk_main_interp->result);
  }
  
#ifndef WIN32
  if (synchronize) XSynchronize(Tk_Display(Tk_MainWindow(STk_main_interp)), TRUE);
#endif
  /* Set the geometry of the main window, if requested. */
  if (geometry != NULL) {
    if (TCL_OK != Tcl_VarEval(STk_main_interp, "(wm 'geometry \".\" '", 
			      geometry, ")", NULL))
      fprintf(STk_stderr, "**** Warning: %s\n", STk_main_interp->result);
  }
  code = TkpInit(STk_main_interp);
  
  if (code == TCL_OK)
    Tk_initialized = 1;   /* Ok, it's fully initialized */

  STk_init_glue();  

  /* Initialize commands which are now in Tcl */
  Tcl_CreateCommand(STk_main_interp, "after",  Tcl_AfterCmd, NULL, NULL);
  Tcl_CreateCommand(STk_main_interp, "update", Tcl_UpdateCmd, NULL, NULL);
  	/* Vwait command can always be done with (tkwait 'variable ...).
	 * So we don't define explicitely vwait 
	 * Note that "update" is also redefined by Tk4.1
	 * Tcl_CreateCommand(STk_main_interp, "vwait",  Tcl_VwaitCmd, NULL, NULL);
	 */

  /* Execute STk's initialization script */
  {
    char buffer[MAX_PATH_LENGTH];
    
    sprintf(buffer, "%s/STk/tk-init.stk", STk_library_path);
    STk_load_file(buffer, TRUE, STk_Tk_module);
  }
}
#endif /* USE_TK */
