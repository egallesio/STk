/*
 * r e a d  . c				-- reading stuff
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
 * Last file update:  3-Sep-1999 20:22 (eg)
 *
 */

#include <ctype.h>
#include "stk.h"
#include "module.h"

static SCM cycles = NULL;		/* used for reading circular data */
static char *proc_name = "read";	/* for Serror macro */	


static SCM read_rec(SCM port, int case_significant);


static int flush_ws(SCM port, char *message)
{
  int c;

  c = Getc(port);
  for ( ; ; ) {
    switch (c) {
      case EOF:  if (message) Serror(message,NIL); else return(c);
      case ';':  do 
		   c = Getc(port); 
		 while (c != '\n' && c != EOF);
		 continue;
      case '\n': STk_line_counter += 1; break;
      default:   if (!isspace(c)) return(c);
    }
    c = Getc(port);
  }
}

static SCM read_list(SCM port, char delim, int case_significant)
/* Read a list ended by the `delim' char */
{
  int c;
  SCM tmp;
  
  c = flush_ws(port, "End of file inside list");
  if (c == delim) return(NIL);

  /* Read the car */
  Ungetc(c, port);
  tmp = read_rec(port, case_significant);
  
  /* Read the cdr */
  if (EQ(tmp, Sym_dot)) {
    tmp = read_rec(port, case_significant);
    c = flush_ws(port, "End of file inside list");
    if (c != delim) Serror("missing close parenthesis", NIL);
    return(tmp);
  }
  return(Cons(tmp, read_list(port, delim, case_significant)));
}

static void read_word(SCM port, int c, int case_significant) 
/* read an item whose 1st char is in c */
{ 
  register int j = 0;
  int allchars   = 0;

  for( ; ; ) {
    allchars  ^= (c == '|');
    if (c != '|') 
      STk_tkbuffer[j++]  = (allchars || case_significant) ? c : tolower(c);

    c = Getc(port);
    if (c == EOF) break;
    if (!allchars) {
      if (strchr("()[]'`,;\"\n", c)) {
	Ungetc(c, port);
	break;
      }
      if (isspace(c)) break;
    }
    if (j >= TKBUFFERN-1) Serror("token too large", NIL);
  }

  STk_tkbuffer[j] = '\0';
}

static void read_char(SCM port, int c)
/* read an char (or a char name) item whose 1st char is in c */
{ 
  register int j = 0;

  for( ; ; ) {
    STk_tkbuffer[j++] = c;
    c = Getc(port);
    if (c == EOF || isspace(c)) break;
    if (strchr("()[]'`,;\"", c)) {
      Ungetc(c, port);
      break;
    }
    if (j >= TKBUFFERN-1) Serror("token too large", NIL);
  }
  STk_tkbuffer[j] = '\0';
}
  
static SCM read_token(SCM port, int c, int case_significant) 
{
  SCM z;

  read_word(port, c, case_significant);
  z = STk_Cstr2number(STk_tkbuffer, 10L);

  if (z == Ntruth)
    /* It is not a number */
    switch (*STk_tkbuffer) {
      case ':': return STk_makekey(STk_tkbuffer);
      case '#': Serror("bad # syntax", STk_makestring(STk_tkbuffer));
      default : return Intern(STk_tkbuffer);
    }
  
  /* Return the number read */
  return z;
}

static SCM read_cycle(SCM port, int c, int case_significant)
/* read a #xx# or #xx= cycle item whose 1st char is in c. */
{ 
  register int j = 0;

  for( ; ; ) {
    STk_tkbuffer[j++] = c;
    c = Getc(port);
    if (c == EOF || !isdigit(c)) break;
    if (j >= TKBUFFERN-1) Serror("token too large", NIL);
  }
  STk_tkbuffer[j] = '\0';

  switch (c) {
    case '#': {
		 SCM tmp, k = STk_makeinteger(atoi(STk_tkbuffer));
		 
		 if ((tmp = STk_assv(k, cycles)) != Ntruth) {
		   return CDR(tmp);
		 }
		 else {
		   char buffer[70];
		   sprintf(buffer, "key ``#%d='' not defined", atoi(STk_tkbuffer));
		   Serror(buffer, NIL);
		 }
    	      }
    case '=': {
      		 SCM val, tmp, k = STk_makeinteger(atoi(STk_tkbuffer));
		 
		 if ((tmp = STk_assv(k, cycles)) == Ntruth) {
		   /* This is a little bit tricky here: We create a fake cell
		    * that could be referenced by the further read. Once the read 
		    * is finished, we overwrite the fake cell with the value 
		    * returned by the read. So, the fake cell becomes the real 
		    * result (not too clear :-).
		    * ATTENTION: the value returned the next read can be of 
		    * any type (e.g. '(1 2 #0="ab" #0#) ). But all our cells 
		    * have the same size => no problem.
		    */
		   tmp      = Cons(UNBOUND, UNBOUND);          /* The fake cell */
		   cycles   = Cons(Cons(k, tmp), cycles);      /* For next read */
		   val      = read_rec(port, case_significant);/* Read item */
		   *tmp     = *val; 			       /*Overwrt fake cell*/
		   return tmp;
		 }
		 else {
		   char buffer[70];
		   sprintf(buffer, "key ``#%d='' already defined",
			   atoi(STk_tkbuffer))
;
		   Serror(buffer, NIL);
		 }
    	      }
    default:  Ungetc(c, port); Serror("bad # syntax", STk_makestring(STk_tkbuffer));
  }
  
  return UNBOUND; /* for the compiler */
}
  

static SCM read_string(SCM port)
{
  int k ,c,n;
  size_t j, len;
  char *p, *buffer;
  SCM z;

  j = 0;
  len = 100;
  p = buffer = must_malloc(len);

  while(((c = Getc(port)) != '"') && (c != EOF)) { 
    if (c == '\\') {
      c = Getc(port);
      if (c == EOF) Serror("eof encountered after \\", NIL);
      switch(c) {
        case 'b' : c = '\b'; break;	/* Bs  */
	case 'e' : c = 0x1b; break;	/* Esc */
	case 'n' : c = '\n'; break;	/* Lf  */
	case 'r' : c = '\r'; break;	/* Cr  */
	case 't' : c = '\t'; break;	/* Tab */
        case '\n': STk_line_counter += 1; continue;
	case '0' : for( k=n=0 ; ; k++ ) {
		     c = Getc(port);
		     if (c == EOF) Serror("eof encountered after \\0", NIL);
	             if (isdigit(c) && (c < '8') && k < 3) /* Max = 3 digits */
		        n = n * 8 + c - '0';
		     else {
		       Ungetc(c, port);
		       break;
		     }
		   }
	           c = n & 0xff;
      }
    }
    else
      if (c == '\n') STk_line_counter += 1;

    if ((j + 1) >= len) {
      len = len + len / 2;
      buffer = must_realloc(buffer, len);
      p = buffer + j;
    }
    j++;
    *p++ = c;
  }
  if (c == EOF) Serror("end of file while reading a string", NIL);
  *p = '\0';
  
  z = STk_makestrg(j, buffer);
  free(buffer);

  return z;
}

static SCM read_rec(SCM port, int case_significant)
{
  int c;

  for ( ; ; ) {
    c = flush_ws(port, "end of file inside read encountered");
    
    switch (c) {
      case '(':
        return(read_list(port, ')', case_significant));
      case '[':
	return(read_list(port, ']', case_significant));
      case ')':
      case ']':
	Puts("\nread: unexpected close parenthesis", STk_curr_eport);
	if (STk_current_filename != UNBOUND)	
	  Fprintf(STk_curr_eport, " at line %d in file %s", 
		  	      STk_line_counter, CHARS(STk_current_filename));
	Putc('\n', STk_curr_eport);
	Flush(STk_curr_eport);
	break;
      case '\'':
	return LIST2(Sym_quote, read_rec(port, case_significant));
      case '`':
	return LIST2(Sym_quasiquote, read_rec(port, case_significant));
      case '#':
	switch(c=Getc(port)) {
	  case 't':
          case 'T':  return Truth;
	  case 'f':
	  case 'F':  return Ntruth;
 	  case '\\': read_char(port, Getc(port));
	             return STk_makechar(STk_string2char(STk_tkbuffer));
	  case '(' : { 
	    	       SCM l = read_list(port, ')', case_significant);
		       return STk_vector(l, STk_llength(l));
	  	     }
	  case '!' : while ((c=Getc(port)) != '\n')
	               if (c == EOF) return STk_eof_object;
		     Ungetc(c, port);
	             continue;
	  case '|':  do
	    		do
			  if (c == '\n') STk_line_counter += 1;
			while ((c != EOF) && (c = Getc(port)) != '|');
		     while ((c != EOF) && (c = Getc(port)) != '#');

 	  	     c = flush_ws(port, (char *) NULL);
		     if (c == EOF) return STk_eof_object;
		     Ungetc(c,port);
	  	     continue;		     
	  case 'p':
	  case 'P': read_word(port, Getc(port), TRUE);
	    	    return STk_address2object(STk_tkbuffer);
	  case '.': return STk_eval(read_rec(port, case_significant), 
				    MOD_ENV(STk_selected_module));
	  case '0':
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9': return read_cycle(port, c, case_significant);
	  default:  Ungetc(c, port); return read_token(port, '#', FALSE);
	}
      case ',': {
	SCM symb;

	c = Getc(port);
	if (c == '@') 
	  symb = Sym_unq_splicing;
	else {
	  symb = Sym_unquote; 
	  Ungetc(c, port);
	}
	return LIST2(symb, read_rec(port, case_significant));
      }
      case '"':
	return read_string(port);
      default:
	return read_token(port, c, case_significant);
    }
  }
}

SCM STk_readf(SCM port, int case_significant)
{
  int c;

  if (cycles == NULL) STk_gc_protect(&cycles);
  cycles = NIL;

  c = flush_ws(port, (char *) NULL);
  if (c == EOF) return(STk_eof_object);
  Ungetc(c, port);
  return read_rec(port, case_significant);
}
