/******************************************************************************
 *
 * w s t k . c
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
 * Last file update:  3-Sep-1999 21:03 (eg)
 *
 * Modification for Win32 DLL support by Steve Pruitt <steve@pruitt.net>
 *  - Defined IMPORT_DLL_GLOBALS instead of STK_MAIN
 *  - Conditionally remove TclWinGetTclInstance() and tclInstance.
 *    These are defined in dllmain within tclWin32Dll.c
 *
 ******************************************************************************/

/* Import DLL globals from library */ 
#if (defined(USE_DYNLOAD) && defined(MSC_VER) && !defined(CYGWIN32))
  #define IMPORT_DLL_GLOBALS
#else
  #define STK_MAIN
#endif


#ifndef CYGWIN32
#  include <dos.h>
#endif

#include <locale.h>
#include "stk.h"

#ifdef USE_TK
#  include "tkWinInt.h"
#endif


/*
 *-------------------------------------------------------------------------
 *
 * setargv --
 *
 *	Parse the Windows command line string into argc/argv.  Done here
 *	because we don't trust the builtin argument parser in crt0.  
 *	Windows applications are responsible for breaking their command
 *	line into arguments.
 *
 *	2N backslashes + quote -> N backslashes + begin quoted string
 *	2N + 1 backslashes + quote -> literal
 *	N backslashes + non-quote -> literal
 *	quote + quote in a quoted string -> single quote
 *	quote + quote not in quoted string -> empty string
 *	quote -> begin quoted string
 *
 * Results:
 *	Fills argcPtr with the number of arguments and argvPtr with the
 *	array of arguments.
 *
 * Side effects:
 *	Memory allocated.
 *
 *--------------------------------------------------------------------------
 */

static void
setargv(argcPtr, argvPtr)
    int *argcPtr;		/* Filled with number of argument strings. */
    char ***argvPtr;		/* Filled with argument strings (malloc'd). */
{
    char *cmdLine, *p, *arg, *argSpace;
    char **argv;
    int argc, size, inquote, copy, slashes;
    
    cmdLine = GetCommandLine();

    /*
     * Precompute an overly pessimistic guess at the number of arguments
     * in the command line by counting non-space spans.
     */

    size = 2;
    for (p = cmdLine; *p != '\0'; p++) {
	if (isspace(*p)) {
	    size++;
	    while (isspace(*p)) {
		p++;
	    }
	    if (*p == '\0') {
		break;
	    }
	}
    }
    argSpace = (char *) ckalloc((unsigned) (size * sizeof(char *) 
	    + strlen(cmdLine) + 1));
    argv = (char **) argSpace;
    argSpace += size * sizeof(char *);
    size--;

    p = cmdLine;
    for (argc = 0; argc < size; argc++) {
	argv[argc] = arg = argSpace;
	while (isspace(*p)) {
	    p++;
	}
	if (*p == '\0') {
	    break;
	}

	inquote = 0;
	slashes = 0;
	while (1) {
	    copy = 1;
	    while (*p == '\\') {
		slashes++;
		p++;
	    }
	    if (*p == '"') {
		if ((slashes & 1) == 0) {
		    copy = 0;
		    if ((inquote) && (p[1] == '"')) {
			p++;
			copy = 1;
		    } else {
			inquote = !inquote;
		    }
                }
                slashes >>= 1;
            }

            while (slashes) {
		*arg = '\\';
		arg++;
		slashes--;
	    }

	    if ((*p == '\0') || (!inquote && isspace(*p))) {
		break;
	    }
	    if (copy != 0) {
		*arg = *p;
		arg++;
	    }
	    p++;
        }
	*arg = '\0';
	argSpace = arg + 1;
    }
    argv[argc] = NULL;

    *argcPtr = argc;
    *argvPtr = argv;
}


#if (!defined(USE_DYNLOAD) || defined(CYGWIN32))
/*=============================================================================*/
static HINSTANCE tclInstance;

HINSTANCE TclWinGetTclInstance()
{
    return tclInstance;
}
#endif

/*=============================================================================*/


int APIENTRY WinMain(HINSTANCE hInstance, 
		     HINSTANCE hPrevInstance, 
		     LPSTR lpszCmdLine, 
		     int nCmdShow)
{
  char **argv, *p;
  int argc;
  char buffer[MAX_PATH_LENGTH];

#if (!defined(USE_DYNLOAD) || defined(CYGWIN32))
  tclInstance = hInstance;
#endif

  /*
   * Set up the default locale to be standard "C" locale so parsing
   * is performed correctly.
   */
  setlocale(LC_ALL, "C");
  
  /*
   * Increase the application queue size from default value of 8.
   * At the default value, cross application SendMessage of WM_KILLFOCUS
   * will fail because the handler will not be able to do a PostMessage!
   * This is only needed for Windows 3.x, since NT dynamically expands
   * the queue.
   */
  SetMessageQueue(64);
  
  /* 
   * Treat the arguments & the program name 
   */
  setargv(&argc, &argv);

  GetModuleFileName(NULL, buffer, sizeof(buffer));
  
  for (p = buffer; *p != '\0'; p++) 	/* substitute all '\\' with '/' so that */
    if (*p == '\\') *p = '/';		/* library search is correct 		*/
 
  argv[0] = strdup(buffer);


  TkWinXInit(hInstance);
  
  STk_toplevel(argc, argv);
  return 0;
}


