/*
 *
 * e r r o r . c 			-- The error procedure
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
 * $Id: error.c 1.4 Fri, 10 Apr 1998 09:13:18 +0200 eg $
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 14-Nov-1993 14:58
 * Last file update:  8-Apr-1998 16:57
 */

#include "stk.h"

/**** Static variables ****/

static jmp_buf global_jmp_buf; 		/* Jump buffer denoting toplevel context */
static int err_counter=0; 		/* To avoid loops when REPORT_ERROR 	 */
					/* proc is buggy 			 */

/**** Public variables ****/
jmp_buf *STk_top_jmp_buf   = &global_jmp_buf;
long     STk_error_context = ERR_FATAL;


		/***************************************************/

static print_error_message(char *head, char *message, SCM who, 
			   int uncode, int show_stack)
{
  if (uncode) who = STk_uncode(who);

  fprintf(STk_stderr, "%s    %s", head, message);
  if (NNULLP(who)) {
    fprintf(STk_stderr, ": ");
    STk_print(who, STk_curr_eport, PANIC_MODE);
  }
  if (show_stack) STk_show_eval_stack(5, uncode);
}

void print_message(char *message, SCM x)
{
  SCM tmp;
  char head[MAX_PATH_LENGTH+50];
  int k;
  
  switch (Error_context) {
    case ERR_READ_FROM_STRING: strcpy(head,"*** Read from string error:\n"); break;
    case ERR_FATAL: 	
    case ERR_OK:               if (STk_current_filename==UNBOUND)
      				 sprintf(head, "*** Error:\n");
    			       else {
				 sprintf(head, "*** Error at line %d of file %s:\n",
					 STk_line_counter, 
					 CHARS(STk_current_filename));
				 STk_current_filename = UNBOUND;
			       }
			       break;	
#ifdef USE_TK
    case ERR_TCL_BACKGROUND:   strcpy(head, "*** Background error:\n"); break;
#endif
  }

  if (err_counter == 0) {
    err_counter += 1;
    tmp          = STk_lookup_variable(REPORT_ERROR, NIL);
    
    if (STk_procedurep(tmp) == Truth)
      Apply(tmp, LIST3(STk_makestring(head),STk_makestring(message),STk_uncode(x)));
    else
      print_error_message(head, message, x, TRUE, TRUE);
  }
  else {
    fprintf(STk_stderr, 
	    "\n#### WARNING: error occurred while executing the body of %s!!\n", 
	    REPORT_ERROR);
    fprintf(STk_stderr, "#### Error in %s was\n", REPORT_ERROR);
    print_error_message("", message, x, FALSE, TRUE);
    fprintf(STk_stderr, "#### Going back to original error:\n");
  }

  fflush(STk_stdout); fflush(STk_stderr);
 
  /* Retain the arguments of this error in global Scheme variables */
  STk_define_variable("*last-error-message*", STk_makestring(message), NIL);
  STk_define_variable("*last-error-arg*", x, NIL);
}

void STk_err(char *message, SCM x)
{
  SCM tmp;
  char head[MAX_PATH_LENGTH+50];
  
  STk_reset_eval_hook();
  
  if (!(Error_context & ERR_IGNORED)) {
    if (*message) print_message(message, x);
    STk_reset_eval_stack();
    STk_unwind_all();
  }

  err_counter = 0;
  switch (Error_context) {
    case ERR_FATAL: 
      STk_panic("FATAL ERROR IN CRITICAL CODE SECTION.");
    default:
      longjmp(*STk_top_jmp_buf, JMP_ERROR);
  }
}


void STk_procedure_error(char *procedure, char *message, SCM x)
{
  char msg[200];

  sprintf(msg, "%s: %s", procedure, message);
  STk_err(msg, x);
}
