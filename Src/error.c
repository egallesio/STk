/*
 *
 * e r r o r . c 			-- The error procedure
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 14-Nov-1993 14:58
 * Last file update:  2-Jun-1996 21:49
 */

#include "stk.h"

static jmp_buf global_jmp_buf; 		/* Jump buffer denoting toplevel context */
jmp_buf *Top_jmp_buf = &global_jmp_buf;

long Error_context = ERR_FATAL;


void STk_err(char *message, SCM x)
{
  SCM tmp;
  static int err_counter=0; /* to avoid loops when REPORT_ERROR proc is buggy */
  char head[MAX_PATH_LENGTH+50];

  err_counter += 1;

  STk_reset_eval_hook();

  if (!(Error_context & ERR_IGNORED)) {
    /* Error is not ignored: A message must be printed */
    if (*message) {
      switch (Error_context) {
        case ERR_READ_FROM_STRING: 
	  	strcpy(head,"*** Read from string error:\n"); break;
        case ERR_FATAL: 	
	case ERR_OK:
	        if (STk_current_filename==UNBOUND)
		  sprintf(head, "*** Error:\n");
		else {
		  sprintf(head, "*** Error at line %d of file %s:\n",
			  STk_line_counter, CHARS(STk_current_filename));
		  STk_current_filename = UNBOUND;
		}
		break;
#ifdef USE_TK
	case ERR_TCL_BACKGROUND:    
	  	strcpy(head, "*** Background error:\n"); break;
#endif
      }

      tmp = VCELL(Intern(REPORT_ERROR));
      if (tmp!=UNBOUND && STk_procedurep(tmp)==Truth &&  err_counter==1 && *message)
	Apply(tmp, LIST3(STk_makestring(head), 
			 STk_makestring(message), 
			 STk_uncode(x)));
      else {
	if (err_counter>1) 
	  fprintf(STk_stderr, "WARNING: %s is buggy!!\n", REPORT_ERROR);
	fprintf(STk_stderr, "\n%s    %s", head, message);
	if (NNULLP(x)) {
	  fprintf(STk_stderr, ": "); 
	  STk_print(STk_uncode(x), STk_curr_eport, WRT_MODE);
	}
	STk_show_eval_stack(5);
      }
    }
    fflush(STk_stdout); fflush(STk_stderr);
    STk_reset_eval_stack();
    /* 
     * Do all the terminal thunk of dynamic winds and make a long jump to
     * location retained in Top_jmp_buf 
     */
    STk_unwind_all();
  }

  err_counter = 0;
  switch (Error_context) {
    case ERR_FATAL: 
      STk_panic("FATAL ERROR IN CRITICAL CODE SECTION. ABANDON\n");
    default:
      longjmp(*Top_jmp_buf, JMP_ERROR);
  }
}
