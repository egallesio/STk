/*
 *
 * t c l - u t i l . c		-- Some Tcl utilities (this correpond to part 
 * 				   of code of the Tcl lib modified to take into
 *				   account some Scheme specificities)
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
 *    Creation date: 26-Feb-1993 10:10
 * Last file update:  3-Sep-1999 20:59 (eg)
 *
 *
 * This code is derivated from several Tcl files which have the following 
 * copyright notice
 *
 * Copyright (c) 1990-1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "stk.h"
#ifndef WIN32
#  include <tclInt.h>
#  include <tclPort.h>
#endif


/*
 *----------------------------------------------------------------------
 *
 * Tcl_PosixError --
 *
 *	This procedure is typically called after UNIX kernel calls
 *	return errors.  It stores machine-readable information about
 *	the error in $errorCode returns an information string for
 *	the caller's use.
 *
 * Results:
 *	The return value is a human-readable string describing the
 *	error, as returned by strerror.
 *
 * Side effects:
 *	The global variable $errorCode is reset.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_PosixError(interp)
    Tcl_Interp *interp;		/* Interpreter whose $errorCode variable
				 * is to be changed. */
{
#ifdef STk_CODE
    return (char *) strerror(errno);
#else
    char *id, *msg;

    id = Tcl_ErrnoId();
    msg = strerror(errno);
    Tcl_SetErrorCode(interp, "POSIX", id, msg, (char *) NULL);
    return msg;
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_Backslash --
 *
 *	Figure out how to handle a backslash sequence.
 *
 * Results:
 *	The return value is the character that should be substituted
 *	in place of the backslash sequence that starts at src.  If
 *	readPtr isn't NULL then it is filled in with a count of the
 *	number of characters in the backslash sequence.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char
Tcl_Backslash(src, readPtr)
    CONST char *src;		/* Points to the backslash character of
				 * a backslash sequence. */
    int *readPtr;		/* Fill in with number of characters read
				 * from src, unless NULL. */
{
    CONST char *p = src+1;
    char result;
    int count;

    count = 2;

    switch (*p) {
	case 'a':
	    result = 0x7;	/* Don't say '\a' here, since some compilers */
	    break;		/* don't support it. */
	case 'b':
	    result = '\b';
	    break;
	case 'f':
	    result = '\f';
	    break;
	case 'n':
	    result = '\n';
	    break;
	case 'r':
	    result = '\r';
	    break;
	case 't':
	    result = '\t';
	    break;
	case 'v':
	    result = '\v';
	    break;
	case 'x':
	    if (isxdigit(UCHAR(p[1]))) {
		char *end;

		result = (char) strtoul(p+1, &end, 16);
		count = end - src;
	    } else {
		count = 2;
		result = 'x';
	    }
	    break;
	case '\n':
	    do {
		p++;
	    } while (isspace(UCHAR(*p)));
	    result = ' ';
	    count = p - src;
	    break;
	case 0:
	    result = '\\';
	    count = 1;
	    break;
	default:
	    if (isdigit(UCHAR(*p))) {
		result = *p - '0';
		p++;
		if (!isdigit(UCHAR(*p))) {
		    break;
		}
		count = 3;
		result = (result << 3) + (*p - '0');
		p++;
		if (!isdigit(UCHAR(*p))) {
		    break;
		}
		count = 4;
		result = (result << 3) + (*p - '0');
		break;
	    }
	    result = *p;
	    count = 2;
	    break;
    }

    if (readPtr != NULL) {
	*readPtr = count;
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_TildeSubst --
 *
 *	Given a name starting with a tilde, produce a name where
 *	the tilde and following characters have been replaced by
 *	the home directory location for the named user.
 *
 * Results:
 *	The result is a pointer to a static string containing
 *	the new name.  If there was an error in processing the
 *	tilde, then an error message is left in interp->result
 *	and the return value is NULL.  The result may be stored
 *	in bufferPtr; the caller must call Tcl_DStringFree(bufferPtr)
 *	to free the name.
 *
 * Side effects:
 *	Information may be left in bufferPtr.
 *
 *----------------------------------------------------------------------
 */
char *
Tcl_TildeSubst(interp, name, bufferPtr)
    Tcl_Interp *interp;		/* Interpreter in which to store error
				 * message (if necessary). */
    char *name;			/* File name, which may begin with "~/"
				 * (to indicate current user's home directory)
				 * or "~<user>/" (to indicate any user's
				 * home directory). */
    Tcl_DString *bufferPtr;	/* May be used to hold result.  Must not hold
				 * anything at the time of the call, and need
				 * not even be initialized. */
{
    char *dir;
    register char *p;

    Tcl_DStringInit(bufferPtr);
    if (name[0] != '~') {
        Tcl_DStringAppend(bufferPtr, name, -1);
	return name;
    }

    if ((name[1] == '/') || (name[1] == '\0')) {
	dir = getenv("HOME");
	if (dir == NULL) {
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "couldn't find HOME environment ",
		    "variable to expand \"", name, "\"", (char *) NULL);
	    return NULL;
	}
	Tcl_DStringAppend(bufferPtr, dir, -1);
	Tcl_DStringAppend(bufferPtr, name+1, -1);
    } else {
	struct passwd *pwPtr;

	for (p = &name[1]; (*p != 0) && (*p != '/'); p++) {
	    /* Null body;  just find end of name. */
	}
	Tcl_DStringAppend(bufferPtr, name+1, p - (name+1));
#ifdef WIN32
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp, "user \"", bufferPtr->string,
		    "\" doesn't exist", (char *) NULL);
	return NULL;
    }
#else
	pwPtr = getpwnam(bufferPtr->string);
	if (pwPtr == NULL) {
	    endpwent();
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "user \"", bufferPtr->string,
		    "\" doesn't exist", (char *) NULL);
	    return NULL;
	}
	Tcl_DStringFree(bufferPtr);
	Tcl_DStringAppend(bufferPtr, pwPtr->pw_dir, -1);
	Tcl_DStringAppend(bufferPtr, p, -1);
	endpwent();
    }
    return bufferPtr->string;
#endif
}


int Tcl_ExprDouble(interp, string, ptr)
     Tcl_Interp *interp;		/* Context in which to evaluate the
					 * expression. */
     char *string;			/* Expression to evaluate. */
     double *ptr;			/* Where to store result. */
{
  SCM value = STk_eval_C_string(string, NIL);

  if (value) {
    if (NUMBERP(value)) {
      *ptr = FLONM(STk_exact2inexact(value));
      return TCL_OK;
    }
  }
  interp->result = "expression didn't have numeric value";
  return TCL_ERROR;
}


int Tcl_ExprLong(interp, string, ptr)
     Tcl_Interp *interp;		/* Context in which to evaluate the
					 * expression. */
     char *string;			/* Expression to evaluate. */
     long *ptr;				/* Where to store result. */
{
  SCM value = STk_eval_C_string(string, NIL);

  if (value) {
    if (EXACTP(value)) {
      *ptr = (long) FLONM(STk_exact2inexact(value));
      return TCL_OK;
    }
  }
  interp->result = "expression didn't have numeric value";
  return TCL_ERROR;
}

#ifdef USE_TK
void Tcl_AddErrorInfo(interp, message)
     Tcl_Interp *interp;
     char *message;	
{
  SCM old = STk_lookup_variable(ERROR_INFO, STk_Tk_module);
  SCM new = STk_makestring(message);

  if (!STRINGP(old)) old = STk_makestring("");
    
  /* Append message to current value of *error-info* */
  STk_define_variable(ERROR_INFO, 
		      STk_string_append(LIST2(old, new), 2),
		      STk_Tk_module);
}
#endif
/*
 *----------------------------------------------------------------------
 *
 * Tcl_AllowExceptions --
 *
 *	Sets a flag in an interpreter so that exceptions can occur
 *	in the next call to Tcl_Eval without them being turned into
 *	errors.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The TCL_ALLOW_EXCEPTIONS flag gets set in the interpreter's
 *	evalFlags structure.  See the reference documentation for
 *	more details.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_AllowExceptions(interp)
    Tcl_Interp *interp;		/* Interpreter in which to set flag. */
{
    Interp *iPtr = (Interp *) interp;

    iPtr->evalFlags |= TCL_ALLOW_EXCEPTIONS;
}

