/*
 *
 *  c o n s o l e . c			-- Console management
 *
 * Copyright © 1998 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 * $Id: console.c 1.5 Mon, 28 Dec 1998 23:05:11 +0100 eg $
 *
 *            Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date:  4-Nov-1998 18:50
 * Last file update: 26-Dec-1998 23:01
 *
 */

#include "stk.h"

#ifdef USE_TK

static PRIMITIVE fill_stdin(SCM str)
{
  static SCM the_buffer = NULL;

  ENTER_PRIMITIVE("%fill-stdin");

  if (!STRINGP(str)) Serror("bad string", str);

  if (!the_buffer) {
    /* first call to fill_stdin. Protect the buffer against GC */
    STk_gc_protect(&the_buffer);
  }

  the_buffer = str;
  STk_fill_stdin_buffer(CHARS(str));

  return UNDEFINED;
}


void STk_console_prompt(SCM env)
{
  char *msg  = "Incorrect \"console-display-prompt\" procedure";
  char *expr = "(catch ((with-module Tk console-display-prompt)(current-module)))";

  if (STk_internal_eval_string(expr, 0, env) == Truth) {
    /* print on stderr AND the console because console is probably 
     * not reliable if we are here.
     */
    fprintf(stderr, "**** FATAL ERROR: %s\nABANDON\n", msg);
    panic(msg);
  }
}


void STk_init_console(void)
{
  char file[MAX_PATH_LENGTH+30];
  char *name= "%make-standard-console";
  SCM *tmp;

  /* Load the console file */
  sprintf(file, "%s/STk/console.stk", STk_library_path);
  if (STk_load_file(file, FALSE, STk_selected_module) == Ntruth)
    panic("Unable to load the console file \"%s\"", file);

  /* Add the new (secret) primitive fill_stdin */
  STk_add_new_primitive("%fill-stdin", tc_subr_1, fill_stdin);

  /* Call the function %make-standard-console */
  tmp = STk_varlookup(Intern(name), NIL, FALSE);

  if (STk_procedurep(*tmp) == Ntruth)
    panic("File \"%s\" does not define properly the function \"%s\"", file, name);

  STk_apply(*tmp, NIL);
}

#endif
