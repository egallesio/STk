/*
 * p r i n t . c				-- writing stuff
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: ??-Oct-1993 ??:?? 
 * Last file update: 14-Sep-1999 09:23 (eg)
 *
 */

#include "stk.h"
#include "extend.h"
#include "module.h"
#ifdef USE_STKLOS
#  include "stklos.h"
#endif

static char valid_symbol_chars[]=
		"abcdefghijklmnopqrstuvwxyz0123456789+-.*/<=>!?:$%_&~^";

static void printlist(SCM exp, SCM port, int mode)
{
  SCM tmp;

  STk_print(CAR(exp), port, mode);
  for (tmp=CDR(exp); CONSP(tmp); tmp=CDR(tmp)) {
    Putc(' ', port);
    STk_print(CAR(tmp), port, mode);
  }
  if (NNULLP(tmp)) {
    Puts(" . ", port);
    STk_print(tmp, port, mode);
  }
}


static void printsymbol(char *s, SCM port, int mode)
{
  if (mode==WRT_MODE) {	/* See if we need to enclose pname between a "|" pair */
    register char *p;
    for (p = s; *p; p++) {
      if (!strchr(valid_symbol_chars, *p)) {
	Putc('|', port);  Puts(s, port); Putc('|', port);
	return;
      }
    }
  }
  Puts(s, port);
}


static void printstring(SCM s, SCM port, int mode)
{
  register unsigned char *p  = (unsigned char *) CHARS(s);
  register size_t len        = STRSIZE(s);
   
  if (mode == DSP_MODE) {
    if (strlen(p) == len)
      /* No null in the string. We can use a Puts, instead of the slow Putc */
      Puts((char *) p, port);
    else
      for (len = STRSIZE(s); len; len--, p++) Putc(*p, port);
  } else {
    char *buff = STk_tkbuffer;
    
    *buff++ = '"';
    for (   ; len; len--, p++) {
      if (buff >= STk_tkbuffer + TKBUFFERN-2) {
	*buff = '\0';
	Puts(STk_tkbuffer, port);
	buff = STk_tkbuffer;
      }
      switch (*p) {
        case '\0' : *buff++ = '\\'; *buff++ = '0'; break;
        case '\a' : *buff++ = '\\'; *buff++ = 'a'; break;
        case '\b' : *buff++ = '\\'; *buff++ = 'b'; break;
        case '\f' : *buff++ = '\\'; *buff++ = 'f'; break;
        case '\n' : *buff++ = '\\'; *buff++ = 'n'; break;
        case '\r' : *buff++ = '\\'; *buff++ = 'r'; break;
        case '\t' : *buff++ = '\\'; *buff++ = 't'; break;
        case '\v' : *buff++ = '\\'; *buff++ = 'v'; break;
        case '"'  : 
        case '\\' : *buff++ = '\\';  /* NO BREAK */
        default   : *buff++ = *p;
      }
    }
    *buff++ = '"';
    *buff   = '\0';
    Puts(STk_tkbuffer, port);
  }
}


#ifdef USE_STKLOS
static void internal_display_instance(SCM instance, SCM port)
{
  sprintf(STk_tkbuffer, "#[instance %lx]", (unsigned long) instance);
  Puts(STk_tkbuffer, port);
}

static void display_instance(SCM instance, SCM port, int type)
{
  char *fct_name = ""; 			/* for GCC !! */
  SCM fct;

  switch (type) {
    case DSP_MODE:   fct_name = "display-object";  break;
    case WRT_MODE:   fct_name = "write-object";    break;
    case TK_MODE:    fct_name = "tk-write-object"; break;
    case PANIC_MODE: internal_display_instance(instance, port); return;
  }

  fct = STk_STklos_value(Intern(fct_name));
  if (fct == UNBOUND)
    internal_display_instance(instance, port);
  else 
    Apply2(fct, instance, port);
}
#endif


SCM STk_print(SCM exp, SCM port, int mode)
{
  switch TYPE(exp) {
    case tc_nil:
      Puts("()", port);
      break;
    case tc_undefined:
      Puts("#[undefined]", port);
      break;
    case tc_boolean:
      Puts(EQ(exp, Truth) ? "#t" : "#f", port);
      break;
    case tc_eof:
      Puts("#[eof]", port);
      break;
    case tc_cons:
      Putc('(', port); printlist(exp, port, mode); Putc(')', port);
      break;
    case tc_integer:
    case tc_bignum:
    case tc_flonum:
      {
	char buffer[100];

	char *s = STk_number2Cstr(exp, 10L, buffer);
	Puts(s, port);
	if (TYPE(exp) == tc_bignum) free(s);
      }
      break;
    case tc_symbol:
      printsymbol(PNAME(exp), port, mode);
      break;
    case tc_keyword:
      if (mode != DSP_MODE) Putc(':', port);
      Puts(KEYVAL(exp)+1, port);
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
      Puts("#[subr ", port);
      Puts((*exp).storage_as.subr.name, port);
      Putc(']', port);
      break;
    case tc_closure:
      if (mode != TK_MODE) {
	Puts("#[closure arglist=", port);
	STk_print(CLOSPARAMS(exp), port, mode);
	sprintf(STk_tkbuffer, " %lx]", (unsigned long) exp);
      }
      else 
	sprintf(STk_tkbuffer, "#p%lx", (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
    case tc_char:
      if (mode!=DSP_MODE){
	Puts("#\\", port);
	Puts((char *) STk_char2string((unsigned char) CHAR(exp)), port);
      }
      else Putc(CHAR(exp), port);
      break;      
    case tc_string:
      printstring(exp, port, mode);
      break;
    case tc_vector:
      {
        int j, n = exp->storage_as.vector.dim;
        
        Puts("#(", port);
        for(j=0; j < n; j++) {
	  STk_print(VECT(exp)[j], port, mode);
	  if ((j + 1) < n) Putc(' ', port);
	}
        Putc(')', port);
      }
      break;         
    case tc_iport:
      if (mode != TK_MODE) 
	sprintf(STk_tkbuffer, "#[input-port '%s'%s]", PORT_NAME(exp),
		              (PORT_FLAGS(exp) & PORT_CLOSED) ? "(closed)" : "");
      else
	sprintf(STk_tkbuffer, "#file%lx", (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
    case tc_oport:
      if (mode != TK_MODE) 
	sprintf(STk_tkbuffer, "#[output-port '%s'%s]", PORT_NAME(exp),
		              (PORT_FLAGS(exp) & PORT_CLOSED) ? "(closed)" : "");
      else
	sprintf(STk_tkbuffer, "#file%lx", (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
    case tc_isport:
      sprintf(STk_tkbuffer, "#[input-string-port %lx%s]",
	      (unsigned long) exp,
	      (PORT_FLAGS(port) & PORT_CLOSED) ? "(closed)" : "");
      Puts(STk_tkbuffer, port);
      break;
    case tc_osport:
      sprintf(STk_tkbuffer, "#[output-string-port %lx%s]",
	      (unsigned long) exp,
	      (PORT_FLAGS(port) & PORT_CLOSED) ? "(closed)" : "");
      Puts(STk_tkbuffer, port);
      break;
    case tc_ivport:
      sprintf(STk_tkbuffer, "#[input-virtual-port %lx%s]",
	      (unsigned long) exp,
	      (PORT_FLAGS(port) & PORT_CLOSED) ? "(closed)" : "");
      Puts(STk_tkbuffer, port);
      break;
    case tc_ovport:
      sprintf(STk_tkbuffer, "#[output-virtual-port %lx%s]",
	      (unsigned long) exp,
	      (PORT_FLAGS(port) & PORT_CLOSED) ? "(closed)" : "");
      Puts(STk_tkbuffer, port);
      break;
    case tc_macro:
      sprintf(STk_tkbuffer, "#[macro %lx]",
	      (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
    case tc_localvar:
      sprintf(STk_tkbuffer,"#[local %s @%d,%d]",
	      PNAME(exp->storage_as.localvar.symbol),
	      exp->storage_as.localvar.level, 
	      exp->storage_as.localvar.position);
      Puts(STk_tkbuffer, port);
      break;
    case tc_globalvar:
      sprintf(STk_tkbuffer, "#[global %s]", PNAME(VCELL(exp)));
      Puts(STk_tkbuffer, port);
      break;
    case tc_modulevar:
      sprintf(STk_tkbuffer, "#[modulevar %s:%s]", 
	      PNAME(MOD_NAME(CDR(CAR(exp)))),
	      PNAME(CAR(CAR(exp))));
      Puts(STk_tkbuffer, port);
      break;

    case tc_cont:
      sprintf(STk_tkbuffer, "#[continuation %lx]", (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
    case tc_env:
      if (mode != TK_MODE) 
	sprintf(STk_tkbuffer, "#[environment %lx]", (unsigned long) exp);
      else 
	sprintf(STk_tkbuffer, "#p%lx", (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
    case tc_address:
      {
	char * fmt = (mode == DSP_MODE) ? "%lx" : "#p%lx";
	sprintf(STk_tkbuffer, fmt, (unsigned long) exp->storage_as.env.data);
	Puts(STk_tkbuffer, port);
      }
      break;
    case tc_autoload: /* should never occur!!!!! */
      sprintf(STk_tkbuffer, "#[autoload %lx]", (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
    case tc_Cpointer:
      STk_Cpointer_display(exp, port, mode);
      break;
    case tc_module:
      sprintf(STk_tkbuffer, "#[module %s]", PNAME(STk_module_name(exp)));
      Puts(STk_tkbuffer, port);
      break;
    case tc_frame:
      sprintf(STk_tkbuffer, "#[frame %lx]", (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
    case tc_values:
      Puts("#[values", port);
      if (!NULLP(CAR(exp))) {
	Putc(' ', port);
	printlist(CAR(exp), port, mode);
      }
      Putc(']', port);
      break;
#ifdef USE_STKLOS
    case tc_instance:
      display_instance(exp, port, mode);
      break;
    case tc_next_method:
      sprintf(STk_tkbuffer, "#[next-method %lx]", (unsigned long) exp);
      Puts(STk_tkbuffer, port);
      break;
#endif
#ifdef USE_TK
    case tc_tkcommand:
      if (mode != TK_MODE) Puts("#[Tk-command ", port);
      Puts(exp->storage_as.tk.data->Id, port);
      if (mode != TK_MODE) Putc(']', port);
      break;
    case tc_tclobject:
      if (mode == TK_MODE) 
	STk_print(CAR(exp), port, WRT_MODE);
      else
	sprintf(STk_tkbuffer, "#[Tcl-obj %lx]", (unsigned long) exp);
        Puts(STk_tkbuffer, port);
      break;
#endif
    case tc_quote:
      Puts("#quote", port);
      break;
    case tc_lambda:
      Puts("#lambda", port); 
      break;
    case tc_if:
      Puts("#if", port); 
      break;
    case tc_setq:
      Puts("#setq", port); 
      break;
    case tc_cond:
      Puts("#cond", port); 
      break;
    case tc_and:
      Puts("#and", port); 
      break;
    case tc_or:
      Puts("#or", port); 
      break;
    case tc_let:
      Puts("#let", port); 
      break;
    case tc_letstar:
      Puts("#let*", port); 
      break;
    case tc_letrec:
      Puts("#letrec", port); 
      break;
    case tc_begin:
      Puts("#begin", port); 
      break;
    case tc_promise:
      sprintf(STk_tkbuffer, "#[promise %lx (%sforced)]", 
	      		    (unsigned long) exp, 
	      		    exp->storage_as.promise.resultknown ? "" : "not ");
      Puts(STk_tkbuffer, port);
      break;
    case tc_apply:
      Puts("#apply", port);
      break;
    case tc_call_cc:
      Puts("#call/cc", port);
      break;
    case tc_dynwind:
      Puts("#dynamic-wind", port);
      break;
    case tc_extend_env:
      Puts("#extend-environment", port);
      break;
    case tc_unbound:
      Puts("#[unbound]", port);
      break;
    default:
      if (EXTENDEDP(exp))
	STk_extended_display(exp, port, mode);
      else {
	sprintf(STk_tkbuffer, "#[unknown %d %lx]", TYPE(exp), (unsigned long) exp);
	Puts(STk_tkbuffer, port);
      }
  }
  return UNDEFINED;
}


/*============================================================================= 
 * 
 *			Printing of circular structures 
 *
 *=============================================================================*/

static SCM cycles      = NULL;
static int index_label = 0;

static void pass1(SCM exp);		/* pass 1: mark cells */
static void pass2(SCM exp, SCM port);	/* pass 2: print      */


static void print_cycle(SCM exp, SCM port)
{  
  SCM value, tmp;

  if ((tmp = STk_assv(exp, cycles)) != Ntruth) {
    if (INTEGERP(value = CDR(tmp))) {
      Fprintf(port, "#%ld#", INTEGER(value));
      return;
    }
  }
  /* This is not a cycle. Do a normal print */
  pass2(exp, port);
}


static void printlist_star(SCM exp, SCM port)
{
  SCM value, tmp;

  tmp = NIL;			/* for GCC */
  Putc('(', port);

  for ( ; ; ) {
    print_cycle(CAR(exp), port);

    if (NULLP(exp=CDR(exp))) break;

    if (NCONSP(exp) || (tmp = STk_assv(exp, cycles)) != Ntruth) {
      if (NCONSP(exp) || (value = CDR(tmp)) == Truth || INTEGERP(value)) { 
	/* either  ". X" or ". #0=(...)" or ". #0#" */
	Puts(" . ", port);
	print_cycle(exp, port);
	break;
      }
    }
    Putc(' ', port);
  }
  Putc(')', port);
}


static void printvector_star(SCM exp, SCM port)
{
  int j, n = exp->storage_as.vector.dim;
  
  Puts("#(", port);
  for(j=0; j < n; j++) {
    print_cycle(VECT(exp)[j], port);
    if ((j + 1) < n) Putc(' ', port);
  }
  Putc(')', port);
}


static void pass1(SCM exp)
{
  SCM tmp;

Top:
  if (NCONSP(exp) && NVECTORP(exp)) return;

  if ((tmp = STk_assv(exp, cycles)) == Ntruth) {
    /* We have never seen this cell so far */
    cycles = Cons(Cons(exp, Ntruth), cycles);

    if (CONSP(exp)) {			/* it's a cons */
      pass1(CAR(exp));
      exp = CDR(exp); 
      goto Top;
    }
    else { 				/* it's a vector */
      int i, len = VECTSIZE(exp)-1;
      for (i = 0; i < len; i++) pass1(VECT(exp)[i]);
      if (len >= 0) {exp = VECT(exp)[len]; goto Top;}
    }
  } 
  else {
    /* This item was already seen. Note that this is the second time */
    CDR(tmp) = Truth;
  }
}


static void pass2(SCM exp, SCM port)
{
  if (NCONSP(exp) && NVECTORP(exp))
    STk_print(exp, port, WRT_MODE); 	/* Normal print */
  else {
    SCM value, tmp;

    /* Eventually print a definition label */
    if ((tmp = STk_assv(exp, cycles)) != Ntruth) {
      if ((value=CDR(tmp)) == Truth) {
	/* First use of this label. Assign it a value */
	Fprintf(port, "#%d=", index_label);
	CDR(tmp) = STk_makeinteger(index_label++);
      }
    }

    if (CONSP(exp)) printlist_star(exp, port);
    else            printvector_star(exp, port);
  }
}

PRIMITIVE STk_print_star(SCM exp, SCM port)
{
  if (NCONSP(exp) &&  NVECTORP(exp)) return STk_print(exp, port, WRT_MODE);

  if (cycles == NULL) STk_gc_protect(&cycles);
  cycles      = NIL;
  index_label = 0;

  pass1(exp); pass2(exp, port);

  return UNDEFINED;
}
