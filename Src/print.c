/*
 * p r i n t . c				-- writing stuff
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
 *    Creation date: ??-Oct-1993 ??:?? 
 * Last file update:  9-Jun-1996 23:53
 *
 */

#include "stk.h"
#include "extend.h"

static char valid_symbol_chars[]=
		"abcdefghijklmnopqrstuvwxyz0123456789+-.*/<=>!?:$%_&~^";

static void printlist(SCM exp, SCM port, int mode)
{
  SCM tmp;
  FILE *f = PORT_FILE(port);

  STk_print(CAR(exp), port, mode);
  for (tmp=CDR(exp); CONSP(tmp); tmp=CDR(tmp)) {
    Putc(' ', f);
    STk_print(CAR(tmp), port, mode);
  }
  if (NNULLP(tmp)) {
    Puts(" . ", f);
    STk_print(tmp, port, mode);
  }
}

static void printsymbol(char *s, FILE *f, int mode)
{
  if (mode==WRT_MODE) {	/* See if we need to enclose pname between a "|" pair */
    register char *p;
    for (p = s; *p; p++) {
      if (!strchr(valid_symbol_chars, *p)) {
	Putc('|', f);  Puts(s, f); Putc('|', f);
	return;
      }
    }
  }
  Puts(s, f);
}

#ifdef USE_STKLOS
static void display_instance(SCM instance, SCM port, int type)
{
  char *fct_name;

  if (type == DSP_MODE)   fct_name = "display-object"; else
  if (type == WRT_MODE)   fct_name = "write-object";   else
  /* (type == TK_MODE) */ fct_name = "tk-write-object";

  Apply(VCELL(STk_intern(fct_name)), LIST2(instance, port));
}
#endif

           
SCM STk_print(SCM exp, SCM port, int mode)
{
  FILE *f = PORT_FILE(port);

  switch TYPE(exp) {
    case tc_nil:
      Puts("()", f);
      break;
    case tc_undefined:
      Puts("#[undefined]", f);
      break;
    case tc_boolean:
      Puts(EQ(exp, Truth) ? "#t" : "#f", f);
      break;
    case tc_eof:
      Puts("#[eof]", f);
      break;
    case tc_cons:
      Putc('(', f); printlist(exp, port, mode); Putc(')', f);
      break;
    case tc_integer:
    case tc_bignum:
    case tc_flonum:
      {
	char buffer[100];

	char *s = STk_number2Cstr(exp, 10, buffer);
	Puts(s, f);
	if (TYPE(exp) == tc_bignum) free(s);
      }
      break;
    case tc_symbol:
      printsymbol(PNAME(exp), f, mode);
      break;
    case tc_keyword:
      if (mode != DSP_MODE) Putc(':', f);
      Puts(KEYVAL(exp)+1, f);
      break;
    case tc_subr_0:
    case tc_subr_1:
    case tc_subr_2:
    case tc_subr_3:
    case tc_subr_0_or_1:
    case tc_subr_1_or_2:
    case tc_subr_2_or_3:
    case tc_lsubr:
    case tc_ssubr:
    case tc_fsubr:
    case tc_syntax:
      Puts("#[subr ", f);
      Puts((*exp).storage_as.subr.name, f);
      Putc(']', f);
      break;
    case tc_closure:
      if (mode != TK_MODE) 
	sprintf(STk_tkbuffer, "#[closure %lx]", (unsigned long) exp);
      else 
	sprintf(STk_tkbuffer, "#p%lx", (unsigned long) exp);
      Puts(STk_tkbuffer, f);
      break;
    case tc_char:
      if (mode!=DSP_MODE){
	Puts("#\\", f);
	Puts(STk_char2string(CHAR(exp)), f);
      }
      else Putc(CHAR(exp), f);
      break;      
    case tc_string:
      {
	register char *p  = CHARS(exp);
	register int  len = STRSIZE(exp);

	if (mode!=DSP_MODE) Putc('"', f);
	
	for (  ; len; len--, p++) {
	  if (mode != DSP_MODE)
	    switch (*p) {
	      case '\0' : Puts("\\0000", f); break;
	      case '\a' : Puts("\\a", f); break;
	      case '\b' : Puts("\\n", f); break;
	      case '\f' : Puts("\\f", f); break;
	      case '\n' : Puts("\\n", f); break;
	      case '\r' : Puts("\\r", f); break;
	      case '\t' : Puts("\\t", f); break;
	      case '\v' : Puts("\\v", f); break;
	      case '"'  :
	      case '\\' : Putc('\\', f); /* NO BREAK */
	      default:    Putc(*p, f);
	    }
	  else 
	     Putc(*p, f);
	}
	
	if (mode!=DSP_MODE) Putc('"', f);	
      }
      break;
    case tc_vector:
      {
        int j, n = exp->storage_as.vector.dim;
        
        Puts("#(", f);
        for(j=0; j < n; j++) {
	  STk_print(VECT(exp)[j], port, mode);
	  if ((j + 1) < n) Putc(' ', f);
	}
        Putc(')', f);
      }
      break;         
    case tc_iport:
      if (mode != TK_MODE) 
	sprintf(STk_tkbuffer, "#[input-port '%s'%s]", PORT_NAME(exp),
		              (PORT_FLAGS(exp) & PORT_CLOSED) ? "(closed)" : "");
      else
	sprintf(STk_tkbuffer, "#file%lx", (unsigned long) exp);
      Puts(STk_tkbuffer, f);
      break;
    case tc_oport:
      if (mode != TK_MODE) 
	sprintf(STk_tkbuffer, "#[output-port '%s'%s]", PORT_NAME(exp),
		              (PORT_FLAGS(exp) & PORT_CLOSED) ? "(closed)" : "");
      else
	sprintf(STk_tkbuffer, "#file%lx", (unsigned long) exp);
      Puts(STk_tkbuffer, f);
      break;
    case tc_isport:
      sprintf(STk_tkbuffer, "#[input-string-port %lx%s]",
	      (unsigned long) exp,
	      (PORT_FLAGS(port) & PORT_CLOSED) ? "(closed)" : "");
      Puts(STk_tkbuffer, f);
      break;
    case tc_osport:
      sprintf(STk_tkbuffer, "#[output-string-port %lx%s]",
	      (unsigned long) exp,
	      (PORT_FLAGS(port) & PORT_CLOSED) ? "(closed)" : "");
      Puts(STk_tkbuffer, f);
      break;
    case tc_macro:
      sprintf(STk_tkbuffer, "#[macro %lx]",
	      (unsigned long) exp);
      Puts(STk_tkbuffer, f);
      break;
    case tc_localvar:
      sprintf(STk_tkbuffer,"#[local %s @%d,%d)]",
	      PNAME(exp->storage_as.localvar.symbol),
	      exp->storage_as.localvar.level, 
	      exp->storage_as.localvar.position);
      Puts(STk_tkbuffer, f);
      break;
    case tc_globalvar:
      sprintf(STk_tkbuffer, "#[global %s]", PNAME(VCELL(exp)));
      Puts(STk_tkbuffer, f);
      break;
    case tc_cont:
      sprintf(STk_tkbuffer, "#[continuation %lx]", (unsigned long) exp);
      Puts(STk_tkbuffer, f);
      break;
    case tc_env:
      sprintf(STk_tkbuffer, "#[environment %lx]", (unsigned long) exp);
      Puts(STk_tkbuffer, f);
      break;
    case tc_address:
      {
	char * fmt = (mode == DSP_MODE) ? "%lx" : "#p%lx";
	sprintf(STk_tkbuffer, fmt, (unsigned long) exp->storage_as.env.data);
	Puts(STk_tkbuffer, f);
      }
      break;
    case tc_autoload: /* should never occur!!!!! */
      sprintf(STk_tkbuffer, "#[autoload %lx]", (unsigned long) exp);
      Puts(STk_tkbuffer, f);
      break;
    case tc_Cpointer:
      STk_Cpointer_display(exp, port, mode);
      break;
#ifdef USE_STKLOS
    case tc_instance:
      display_instance(exp, port, mode);
      break;
    case tc_next_method:
      sprintf(STk_tkbuffer, "#[next_method %lx]", (unsigned long) exp);
      Puts(STk_tkbuffer, f);
      break;
#endif
#ifdef USE_TK
    case tc_tkcommand:
      if (mode != TK_MODE) Puts("#[Tk-command ", f);
      Puts(exp->storage_as.tk.data->Id, f);
      if (mode != TK_MODE) Putc(']', f);
      break;
#endif
    case tc_quote:
      Puts("#quote", f);
      break;
    case tc_lambda:
      Puts("#lambda", f); 
      break;
    case tc_if:
      Puts("#if", f); 
      break;
    case tc_setq:
      Puts("#setq", f); 
      break;
    case tc_cond:
      Puts("#cond", f); 
      break;
    case tc_and:
      Puts("#and", f); 
      break;
    case tc_or:
      Puts("#or", f); 
      break;
    case tc_let:
      Puts("#let", f); 
      break;
    case tc_letstar:
      Puts("#let*", f); 
      break;
    case tc_letrec:
      Puts("#letrec", f); 
      break;
    case tc_begin:
      Puts("#begin", f); 
      break;
    case tc_promise:
      sprintf(STk_tkbuffer, "#[promise %lx (%sforced)]", 
	      		    (unsigned long) exp, 
	      		    exp->storage_as.promise.resultknown ? "" : "not ");
      Puts(STk_tkbuffer, f);
      break;
    case tc_apply:
      Puts("#apply", f);
      break;
    case tc_call_cc:
      Puts("#call/cc", f);
      break;
    case tc_dynwind:
      Puts("#dynamic-wind", f);
      break;
    case tc_extend_env:
      Puts("#extend-environment", f);
      break;
    case tc_unbound:
      Puts("#[unbound]", f);
      break;
    default:
      if (EXTENDEDP(exp))
	STk_extended_display(exp, port, mode);
      else {
	sprintf(STk_tkbuffer, "#[unknown %d %lx]", TYPE(exp), (unsigned long) exp);
	Puts(STk_tkbuffer, f);
      }
  }
  return UNDEFINED;
}
