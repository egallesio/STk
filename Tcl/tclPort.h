/*
 * tclPort.h --
 *
 *	This header file handles porting issues that occur because
 *	of differences between systems.  It reads in platform specific
 *	portability files.
 *
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tclPort.h 1.15 96/02/07 17:24:21
 */

#ifndef _TCLPORT
#define _TCLPORT

#if defined(__WIN32__) || defined(_WIN32)
#  ifdef SCM_CODE
#     include "tclWinPort.h"
#  else
#     include "../win/tclWinPort.h"
#  endif
#else
#   if defined(MAC_TCL)
#	include "tclMacPort.h"
#    else
#  ifdef SCM_CODE
#     include "tclUnixPort.h"
#  else
#     include "../unix/tclUnixPort.h"
#  endif
#    endif
#endif

#endif /* _TCLPORT */
