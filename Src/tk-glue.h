/*
 *
 * t k - g l u e . h 		- Glue function between the scheme and Tk worlds
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 19-Feb-1993 22:15
 * Last file update:  2-Jul-1996 10:28
 *
 */

/* Scheme objects used to represent the "." pseudo widget and its name */
extern SCM STk_root_window;
extern SCM STk_root_window_name;

/* Last result of Tcl_GlobalEval (as a SCM object rather than a string) */
extern SCM STk_last_Tk_result;

char *STk_convert_for_Tk(SCM obj, SCM *res);

void STk_sharp_dot_result(Tcl_Interp *interp, char *value);
void STk_stringify_result(Tcl_Interp *interp, char *value);
SCM STk_last_Tk_as_SCM(void);
SCM STk_get_NIL_value(void);

int   STk_valid_callback(char *s, void **p);
void  STk_add_callback(char *key1, char *key2, char *key3, SCM closure);
void  STk_delete_callback(char *key);
void  STk_mark_callbacks(void);
char *STk_append_callback_parameters(SCM proc);

/*
 * STk_stringify permits to transform the string "s" in a valid STk string.
 * Original string is deallocated if free_original is 1 
 */

char *STk_stringify(char *s, int free_original);

