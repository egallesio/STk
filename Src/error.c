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
 * $Id: error.c 1.13 Wed, 23 Dec 1998 23:41:27 +0100 eg $
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 14-Nov-1993 14:58
 * Last file update: 23-Dec-1998 15:13
 */

#include "stk.h"

#ifdef DEBUG_STK
void print_error_handlers(void)			/* For debug only */
{
  struct error_handler *p;

  Fprintf(STk_stderr, "\t**** Error handlers\n");

  for (p = STk_err_handler;  ; p = p->prev) {
    Fprintf(STk_stderr, "\t\thandler(@%lx) = %02x =>",
	   (unsigned long) p, p->context);

    if (p->context & ERR_FATAL) 	   Fprintf(STk_stderr, "FATAL ");
    if (p->context & ERR_OK) 		   Fprintf(STk_stderr, "OK ");
    if (p->context & ERR_READ_FROM_STRING) Fprintf(STk_stderr, "READ_FROM_STRING ");
    if (p->context & ERR_IGNORED) 	   Fprintf(STk_stderr, "IGNORED ");
    if (p->context & ERR_TCL_BACKGROUND)   Fprintf(STk_stderr, "TCL_BACKGROUND ");
    if (p->context & ERR_IN_REPORT_ERROR)  Fprintf(STk_stderr, "IN_REPORT_ERROR ");
    if (p->context & ERR_IN_LOAD)          Fprintf(STk_stderr, "IN_LOAD ");
    Fprintf(STk_stderr, "\n");
    if (!p->prev) break;
  }
  Flush(STk_stderr);
}
#endif


		/***************************************************/

static void print_error_message(char *head, char *message, SCM who, 
				int uncode, int show_stack)
{
  if (uncode) who = STk_uncode(who);

  Fprintf(STk_curr_eport, "%s    %s", head, message);
  if (NNULLP(who)) {
    Puts(": ", STk_curr_eport);
    STk_print(who, STk_curr_eport, PANIC_MODE);
  }
  if (show_stack) STk_show_eval_stack(5, uncode);
}



static void recursive_error(char *head, char *message, SCM x)
{
  Fprintf(STk_curr_eport,
	  "\n#### WARNING: error occurred while executing the body of %s!!\n", 
	  REPORT_ERROR);
  Fprintf(STk_curr_eport, "#### Error in %s was\n", REPORT_ERROR);
  print_error_message("", message, x, FALSE, TRUE);
  Fprintf(STk_curr_eport, "#### Going back to original error:\n");
  longjmp(STk_err_handler->j, JMP_ERROR);  /* Go back to previous call to error */
}


/*=============================================================================
 *
 * print-message 
 *
 *=============================================================================*/

static void print_message(char *message, SCM x)
{
  char head[MAX_PATH_LENGTH+50];
  int context = STk_err_handler->context;

  /*
   * Determine what the header of the message will contain depending of context
   */
  if (context & ERR_IN_REPORT_ERROR) {
    /* We have a recursive error (i.e. the printing function is buggy) */
    recursive_error(head, message, x);
  } else if (context & ERR_READ_FROM_STRING)
    strcpy(head,"*** Read from string error:\n");
#ifdef USE_TK
  else if (context & ERR_TCL_BACKGROUND)
    strcpy(head, "*** Background error:\n");
#endif
  else {
    /* ERR_FATAL or ERR_OK */
    if (STk_current_filename==UNBOUND)
      sprintf(head, "*** Error:\n");
    else {
      sprintf(head, "*** Error at line %d of file %s:\n",
	      STk_line_counter, CHARS(STk_current_filename));
      STk_current_filename = UNBOUND;
    }
  }

  /*
   * Header is constituted, call the report-error user procedure for
   * printing message (if available)
   */
  PUSH_ERROR_HANDLER
    {
       SCM tmp = STk_lookup_variable(REPORT_ERROR, NIL);

       STk_err_handler->context |= ERR_IN_REPORT_ERROR; /* for recursive errors */
       
       if (STk_procedurep(tmp) == Truth)
	 Apply(tmp, LIST3(STk_makestring(head),
			  STk_makestring(message),
			  STk_uncode(x)));
       else
	 print_error_message(head, message, x, TRUE, TRUE);
    }
  WHEN_ERROR
    {
      print_error_message(head, message, x, TRUE, FALSE);
      Putc('\n', STk_curr_eport);
    }
  POP_ERROR_HANDLER;

  /*
   * Retain the arguments of this error in global Scheme variables
   */
  Flush(STk_curr_oport); Flush(STk_curr_eport);
  STk_define_variable("*last-error-message*", STk_makestring(message), NIL);
  STk_define_variable("*last-error-arg*", x, NIL);
}


void STk_err(char *message, SCM x)
{
  STk_reset_eval_hook();

 /* Print a message if the error is not ignored */
  if (!(STk_err_handler->context & ERR_IGNORED)) {
    if (*message) print_message(message, x);
    STk_reset_eval_stack();
  }

  /* Propage the error if we are not in a critical section */
  if (STk_err_handler->context & ERR_FATAL)
    panic("FATAL ERROR IN CRITICAL CODE SECTION.");
  else
    longjmp(STk_err_handler->j, JMP_ERROR);
}


void STk_procedure_error(char *procedure, char *message, SCM x)
{
  char msg[200];

  sprintf(msg, "%s: %s", procedure, message);
  STk_err(msg, x);
}
