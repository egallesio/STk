/*
 *
 * t c l - g l u e . h 		- Glue functions between the scheme and Tcl worlds
 *
 * Copyright © 1997-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  6-Aug-1997 12:48
 * Last file update:  3-Sep-1999 20:59 (eg)
 *
 *
 */

#ifndef _TCL_GLUE_
#define _TCL_GLUE_

SCM      STk_convert_Tcl_string2list(char *s);
char    *STk_convert_for_Tcl(SCM obj, SCM *res);

char    *STk_stringify(char *s, int free_original);

Tcl_Obj *STk_create_tcl_object(SCM data);
Tcl_Obj *STk_NewKeywordObj(char *str);
Tcl_Obj *STk_NewSymbolObj(char *str);

char *STk_tcl_getvar(char *name, char *env);
char *STk_tcl_getvar2(char *name1, char *name2, char *env);
char *STk_tcl_setvar(char *var, char *val, int flags, char *env);
char *STk_tcl_setvar2(char *name1, char *name2, char *val, int flags, char *env);

#endif /* _TCL_GLUE */
