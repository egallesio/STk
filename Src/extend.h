/*
 *
 * e x t e n d . h			-- All the stuff dealing with 
 *					   extended types
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 15-Mar-1995 11:36
 * Last file update:  3-Sep-1999 20:20 (eg)
 */


void  STk_extended_mark(SCM x);
void  STk_extended_sweep(SCM x);
SCM   STk_extended_apply(SCM x, SCM args, SCM env);
void  STk_extended_display(SCM x, SCM port, int mode);
int   STk_extended_procedurep(SCM x);
int   STk_extended_eval_parameters(SCM x);
SCM   STk_extended_compare(SCM x, SCM y, int equalp);
char *STk_get_extended_name(int i);

#ifdef USE_STKLOS
void STk_register_extended_class(SCM c, int type_index);
SCM  STk_extended_class_of(SCM obj);   /* Warning: obj MUST be an extd type */
#endif


void  STk_Cpointer_display(SCM obj, SCM port, int mode);

SCM   STk_apply_getter_C_variable(char *var);
void  STk_apply_setter_C_variable(char *var, SCM value);
