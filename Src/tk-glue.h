/*
 *
 * t k - g l u e . h 		- Glue functions between the scheme and Tk worlds
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 19-Feb-1993 22:15
 * Last file update:  3-Sep-1999 21:00 (eg)
 *
 */

#ifndef _TK_GLUE
#define _TK_GLUE_
/* Include tcl-glue.h */ 
#include "tcl-glue.h"

/* Scheme objects used to represent the "." pseudo widget and its name */
extern SCM STk_root_window;
extern SCM STk_root_window_name;

/* Last result of Tcl_GlobalEval (as a SCM object rather than a string) */
extern SCM STk_last_Tk_result;

SCM   STk_convert_tcl_list_to_scheme(SCM l);
SCM   STk_execute_Tcl_lib_cmd(SCM cmd, SCM args, SCM env, int eval_args);
char *STk_convert_for_Tk(SCM obj, SCM *res);

void  STk_sharp_dot_result(Tcl_Interp *interp, char *value);
void  STk_stringify_result(Tcl_Interp *interp, char *value);
SCM   STk_last_Tk_as_SCM(void);
SCM   STk_get_NIL_value(void);
SCM   STk_get_widget_value(char *name);

int   STk_valid_environment(char *s, void **env);
int   STk_valid_callback(char *s, void **p);
void  STk_add_callback(char *key1, char *key2, char *key3, SCM closure);
void  STk_delete_callback(char *key);
void  STk_mark_callbacks(void);
char *STk_append_callback_parameters(SCM proc);

void  STk_init_glue(void);

#define STk_convert_for_Tk	STk_convert_for_Tcl /* compatibility with pre 4.0 */

#endif  /* _TK_GLUE_ */
