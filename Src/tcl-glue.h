/*
 *
 * t c l - g l u e . h 		- Glue functions between the scheme and Tcl worlds
 *
 * Copyright © 1997-1998 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  6-Aug-1997 12:48
 * Last file update:  8-Feb-1998 18:26
 *
 *
 */

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

