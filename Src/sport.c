/*
 * s p o r t . c			-- String ports management
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
 *    Creation date: 17-Feb-1993 12:27
 * Last file update: 14-Sep-1999 09:26 (eg)
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
  p->base      = p->ptr    = must_malloc((unsigned int ) p->cnt + 1);
  strcpy(p->base, str);

  /* Sport_descr is a short version of a port_descr */
  NEWCELL(z, tc_isport);
  z->storage_as.port.p   = (struct port_descr *) 
				must_malloc(sizeof(struct sport_descr));
  PORT_UNGETC(z)	 = EOF;
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
  SCM result;

  PUSH_ERROR_HANDLER
    {
      STk_err_handler->context =
	   (STk_err_handler->context & (ERR_IGNORED|ERR_OK)) | ERR_READ_FROM_STRING;
      result = STk_readf(port, case_significant);
      *eof   = Eof(port);
    }
  WHEN_ERROR
    {
      result = EVAL_ERROR;
      /* Two cases:
       *    - if we are in a catch, propagate the error to go back in the
       *      context of the catch
       *    - otherwise error has already been signaled, do nothing
       */
      if (STk_err_handler->context & ERR_IGNORED) PROPAGATE_ERROR();
    }
  POP_ERROR_HANDLER;

  return result;
}


PRIMITIVE STk_open_input_string(SCM s)
{
  if (NSTRINGP(s)) Err("open-input-string: not a string", s);
  return STk_internal_open_input_string(CHARS(s));
}


PRIMITIVE STk_open_output_string(void)
{
  struct str_iob *p;
  SCM z;

  p = (struct str_iob *) must_malloc(sizeof (struct str_iob));

  p->signature = SPORT_SIGNATURE;
  p->flag      = WRITING;
  p->cnt       = 0;
  p->bufsiz    = START_ALLOC;
  p->base      = p->ptr = (unsigned char *) must_malloc(START_ALLOC);

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

  return STk_makestrg(        ((struct str_iob *)PORT_FILE(port))->cnt, 
		      (char*) ((struct str_iob *)PORT_FILE(port))->base);
}

PRIMITIVE STk_input_string_portp(SCM port)
{
  return (ISPORTP(port)) ? Truth: Ntruth;
}

PRIMITIVE STk_output_string_portp(SCM port)
{
  return (OSPORTP(port)) ? Truth: Ntruth;
}


/*=============================================================================*\
 * 
 * 			Redirections from/to a string
 *
\*=============================================================================*/


PRIMITIVE STk_with_input_from_string(SCM string, SCM thunk)
{
  ENTER_PRIMITIVE("with-input-from-string");

  if (NSTRINGP(string))     Serror("bad string", string);
  if (!STk_is_thunk(thunk)) Serror("bad thunk",  thunk);

  return STk_redirect_input(STk_internal_open_input_string(CHARS(string)), thunk);
}

PRIMITIVE STk_with_output_to_string(SCM thunk)
{
  SCM p;

  ENTER_PRIMITIVE("with-output-to-string");

  if (!STk_is_thunk(thunk)) Serror("bad thunk", thunk);

  p = STk_open_output_string();
  STk_redirect_output(p, thunk);
  return STk_get_output_string(p);
}

PRIMITIVE STk_with_error_to_string(SCM thunk)
{
  SCM p;

  ENTER_PRIMITIVE("with-error-to-string");

  if (!STk_is_thunk(thunk)) Serror("bad thunk", thunk);

  p = STk_open_output_string();
  STk_redirect_error(p, thunk);
  return STk_get_output_string(p);
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


