/*
 * s p o r t . c			-- String ports management
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
 *    Creation date: 17-Feb-1993 12:27
 * Last file update: 13-Jun-1996 18:24
 *
 *
 * This is achieved in a (surely very) dependant way. A string port is implemented
 * via a pseudo FILE descriptor malloc'd when open-input-string is called. This 
 * descriptor is released when free-string-port is called.
 */

#include "stk.h"
#include "sport.h"

SCM STk_internal_open_input_string(char *str)
{
  struct str_iob *p;
  SCM z;

  p = (struct str_iob *) must_malloc(sizeof (struct str_iob));
  
  p->signature = SPORT_SIGNATURE;
  p->flag      = READING;
  p->cnt       = p->bufsiz = strlen(str);
  p->base      = p->ptr    = must_malloc(p->cnt + 1);
  strcpy(p->base, str);

  /* Sport_descr is a short version of a port_descr */
  NEWCELL(z, tc_isport);
  z->storage_as.port.p   = (struct port_descr *) 
				must_malloc(sizeof(struct sport_descr));
  PORT_FILE(z)           = (FILE *) p;
  PORT_FLAGS(z)	 	 = 0;

  return z;
}

void STk_free_string_port(SCM port)
{
  struct str_iob * p;

  p = (struct str_iob *) PORT_FILE(port);
  free(p->base);
  free(p);
  free(port->storage_as.port.p);
}

SCM STk_internal_read_from_string(SCM port, int *eof, int case_significant)
{
  jmp_buf jb, *prev_jb = Top_jmp_buf;
  long prev_context     = Error_context;
  SCM result;
  int k;

  /* save normal error jmpbuf  so that read error don't lead to toplevel */
  /* If in a "catch", keep the ERR_IGNORED bit set */
  if ((k = setjmp(jb)) == 0) {
    Top_jmp_buf   = &jb;
    Error_context = (Error_context & ERR_IGNORED) | ERR_READ_FROM_STRING;
    result 	  = STk_readf(PORT_FILE(port), case_significant);
    *eof   	  = Eof(PORT_FILE(port));
  }
  Top_jmp_buf   = prev_jb;;
  Error_context = prev_context;
  
  if (k == 0) return result;
  
  /* if we are here, an error has occured during the string reading 
   * Two cases:
   *    - we are in a catch. Do a longjump to the catch to signal it a fail
   *    - otherwise error has already signaled, just return EVAL_ERROR
   */
  if (Error_context & ERR_IGNORED) longjmp(*Top_jmp_buf, k);
  return EVAL_ERROR;
}

PRIMITIVE STk_open_input_string(SCM s)
{
  if (NSTRINGP(s)) Err("open-input-string: not a string", s);
  return STk_internal_open_input_string(CHARS(s));
}


PRIMITIVE STk_open_output_string()
{
  struct str_iob *p;
  SCM z;

  p = (struct str_iob *) must_malloc(sizeof (struct str_iob));

  p->signature = SPORT_SIGNATURE;
  p->flag      = WRITING;
  p->cnt       = 0;
  p->bufsiz    = START_ALLOC;
  p->base      = p->ptr = (char *) must_malloc(START_ALLOC);

  NEWCELL(z, tc_osport);
  z->storage_as.port.p   = (struct port_descr *) 
    				must_malloc(sizeof(struct sport_descr));
  PORT_FILE(z)           = (FILE *) p;
  PORT_FLAGS(z)	 	 = 0;

  return z;
}

PRIMITIVE STk_get_output_string(SCM port)
{
  if (NOSPORTP(port)) Err("get-output-string: Bad string-port", port);
  if (PORT_FLAGS(port) & PORT_CLOSED) 
    Err("get-output-string: string port is closed", port);

  return STk_makestrg(((struct str_iob *)PORT_FILE(port))->cnt, 
		      ((struct str_iob *)PORT_FILE(port))->base);
}

PRIMITIVE STk_input_string_portp(SCM port)
{
  return (ISPORTP(port)) ? Truth: Ntruth;
}

PRIMITIVE STk_output_string_portp(SCM port)
{
  return (OSPORTP(port)) ? Truth: Ntruth;
}

PRIMITIVE STk_with_input_from_string(SCM string, SCM thunk)
{
  jmp_buf env, *prev_env = Top_jmp_buf;
  SCM result, prev_iport = STk_curr_iport;
  int prev_context	 = Error_context;
  int k;

  if (NSTRINGP(string))     Err("with-input-from-string: bad string", string);
  if (!STk_is_thunk(thunk)) Err("with-input-from-string: bad thunk", thunk);

  if ((k = setjmp(env)) == 0) {
    Top_jmp_buf    = &env;
    STk_curr_iport = STk_internal_open_input_string(CHARS(string));
    result         = Apply(thunk, NIL);
  }
  /* restore normal error jmpbuf  and current input port*/
  STk_curr_iport = prev_iport;
  Top_jmp_buf    = prev_env;
  Error_context  = prev_context;

  if (k) /*propagate error */ longjmp(*Top_jmp_buf, k);
  return result;
}

PRIMITIVE STk_with_output_to_string(SCM thunk)
{
  jmp_buf env, *prev_env = Top_jmp_buf;
  SCM result, prev_oport = STk_curr_oport;
  int prev_context       = Error_context;
  int k;

  if (!STk_is_thunk(thunk)) Err("with-output-to-string: bad thunk", thunk);

  if ((k = setjmp(env)) == 0) {
    Top_jmp_buf    = &env;
    STk_curr_oport = STk_open_output_string();
    Apply(thunk, NIL);
    result         = STk_get_output_string(STk_curr_oport);
  }
  /* restore normal error jmpbuf  and current input port*/
  STk_curr_oport = prev_oport;
  Top_jmp_buf    = prev_env;
  Error_context  = prev_context;

  if (k) /*propagate error */ longjmp(*Top_jmp_buf, k);
  return result;
}


PRIMITIVE STk_read_from_string(SCM str)
{
  SCM result, port;
  int eof;	/* not used here */

  if (NSTRINGP(str)) Err("read-from-string: Bad string", str);

  /* Create a string port to read in the expression */
  port   = STk_internal_open_input_string(CHARS(str));
  result = STk_internal_read_from_string(port, &eof, FALSE);

  return result == EVAL_ERROR? UNDEFINED: result;
}


