/*
 *
 * e x t e n d . h			-- All the stuff dealing with 
 *					   extended types
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 15-Mar-1995 11:36
 * Last file update: 26-Feb-1996 13:41
 */


void STk_extended_mark(SCM x);
void STk_extended_sweep(SCM x);
SCM  STk_extended_apply(SCM x, SCM args, SCM env);
void STk_extended_display(SCM x, SCM port, int mode);
int  STk_extended_procedurep(SCM x);
int  STk_extended_eval_parameters(SCM x);
SCM  STk_extended_compare(SCM x, SCM y, int equalp);

void STk_Cpointer_display(SCM obj, SCM port, int mode);

SCM STk_apply_getter_C_variable(char *var);
void STk_apply_setter_C_variable(char *var, SCM value);
