/*
 * r e a d  . c				-- reading stuff
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
 * $Id: read.c 1.5 Thu, 10 Sep 1998 23:44:28 +0200 eg $
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: ??-Oct-1993 ??:?? 
 * Last file update: 10-Sep-1998 15:31
 *
 */

#include <ctype.h>
#include "stk.h"
#include "module.h"

static SCM cycles = NULL;		/* used for reading circular data */
static char *proc_name = "read";	/* for Serror macro */	


static SCM read_rec(FILE *f, int case_significant);


static int flush_ws(FILE *f, char *message)
{
  int c;

  c = Getc(f);
  for ( ; ; ) {
    switch (c) {
      case EOF:  if (message) Serror(message,NIL); else return(c);
      case ';':  do 
		   c = Getc(f); 
		 while (c != '\n' && c != EOF);
		 continue;
      case '\n': STk_line_counter += 1; break;
      default:   if (!isspace(c)) return(c);
    }
    c = Getc(f);
  }
}

static SCM read_list(FILE *f, char delim, int case_significant)
/* Read a list ended by the `delim' char */
{
  int c;
  SCM tmp;
  
  c = flush_ws(f, "End of file inside list");
  if (c == delim) return(NIL);

  /* Read the car */
  Ungetc(c, f);
  tmp = read_rec(f, case_significant);
  
  /* Read the cdr */
  if (EQ(tmp, Sym_dot)) {
    tmp = read_rec(f, case_significant);
    c = flush_ws(f, "End of file inside list");
    if (c != delim) Serror("missing close parenthesis", NIL);
    return(tmp);
  }
  return(Cons(tmp, read_list(f, delim, case_significant)));
}

static void read_word(FILE *f, int c, int case_significant) 
/* read an item whose 1st char is in c */
{ 
  register int j = 0;
  int allchars   = 0;

  for( ; ; ) {
    allchars  ^= (c == '|');
    if (c != '|') 
      STk_tkbuffer[j++]  = (allchars || case_significant) ? c : tolower(c);

    c = Getc(f);
    if (c == EOF) break;
    if (!allchars) {
      if (strchr("()[]'`,;\"\n", c)) {
	Ungetc(c, f);
	break;
      }
      if (isspace(c)) break;
    }
    if (j >= TKBUFFERN-1) Serror("token too large", NIL);
  }

  STk_tkbuffer[j] = '\0';
}

static void read_char(FILE *f, int c)
/* read an char (or a char name) item whose 1st char is in c */
{ 
  register int j = 0;

  for( ; ; ) {
    STk_tkbuffer[j++] = c;
    c = Getc(f);
    if (c == EOF || isspace(c)) break;
    if (strchr("()[]'`,;\"", c)) {
      Ungetc(c, f);
      break;
    }
    if (j >= TKBUFFERN-1) Serror("token too large", NIL);
  }
  STk_tkbuffer[j] = '\0';
}
  
static SCM read_token(FILE *f, int c, int case_significant) 
{
  SCM z;

  read_word(f, c, case_significant);
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

static SCM read_cycle(FILE *f, int c, int case_significant)
/* read a #xx# or #xx= cycle item whose 1st char is in c. */
{ 
  register int j = 0;

  for( ; ; ) {
    STk_tkbuffer[j++] = c;
    c = Getc(f);
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
		   tmp      = Cons(UNBOUND, UNBOUND);       /* The fake cell */
		   cycles   = Cons(Cons(k, tmp), cycles);   /* For next read */
		   val      = read_rec(f, case_significant);/* Read item */
		   *tmp     = *val; 			    /* Overwrt fake cell*/
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
    default:  Ungetc(c, f); Serror("bad # syntax", STk_makestring(STk_tkbuffer));
  }
  
  return UNBOUND; /* for the compiler */
}
  

static SCM read_string(FILE *f)
{
  int j, k ,c,n;
  size_t len;
  char *p, *buffer;
  SCM z;

  j = 0;
  len = 100;
  p = buffer = must_malloc(len);

  while(((c = Getc(f)) != '"') && (c != EOF)) { 
    if (c == '\\') {
      c = Getc(f);
      if (c == EOF) Serror("eof encountered after \\", NIL);
      switch(c) {
        case 'b' : c = '\b'; break;	/* Bs  */
	case 'e' : c = 0x1b; break;	/* Esc */
	case 'n' : c = '\n'; break;	/* Lf  */
	case 'r' : c = '\r'; break;	/* Cr  */
	case 't' : c = '\t'; break;	/* Tab */
        case '\n': STk_line_counter += 1; continue;
	case '0' : for( k=n=0 ; ; k++ ) {
		     c = Getc(f);
		     if (c == EOF) Serror("eof encountered after \\0", NIL);
	             if (isdigit(c) && (c < '8') && k < 3) /* Max = 3 digits */
		        n = n * 8 + c - '0';
		     else {
		       Ungetc(c, f);
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

static SCM read_rec(FILE *f, int case_significant)
{
  int c;

  for ( ; ; ) {
    c = flush_ws(f, "end of file inside read encountered");
    
    switch (c) {
      case '(':
        return(read_list(f, ')', case_significant));
      case '[':
	return(read_list(f, ']', case_significant));
      case ')':
      case ']':
	fprintf(STk_stderr, "\nread: unexpected close parenthesis");
	if (STk_current_filename != UNBOUND)	
	  fprintf(STk_stderr, " at line %d in file %s", 
		  	      STk_line_counter, CHARS(STk_current_filename));
	fprintf(STk_stderr, "\n");
	break;
      case '\'':
	return LIST2(Sym_quote, read_rec(f, case_significant));
      case '`':
	return LIST2(Sym_quasiquote, read_rec(f, case_significant));
      case '#':
	switch(c=Getc(f)) {
	  case 't':
          case 'T':  return Truth;
	  case 'f':
	  case 'F':  return Ntruth;
 	  case '\\': read_char(f, Getc(f));
	             return STk_makechar(STk_string2char(STk_tkbuffer));
	  case '(' : { 
	    	       SCM l = read_list(f, ')', case_significant);
		       return STk_vector(l, STk_llength(l));
	  	     }
	  case '!' : while ((c=Getc(f)) != '\n')
	               if (c == EOF) return STk_eof_object;
		     Ungetc(c, f);
	             continue;
	  case '|':  do
	    		do
			  if (c == '\n') STk_line_counter += 1;
			while ((c != EOF) && (c = Getc(f)) != '|');
		     while ((c != EOF) && (c = Getc(f)) != '#');

 	  	     c = flush_ws(f, (char *) NULL);
		     if (c == EOF) return STk_eof_object;
		     Ungetc(c,f);
	  	     continue;		     
	  case 'p':
	  case 'P': read_word(f, Getc(f), TRUE);
	    	    return STk_address2object(STk_tkbuffer);
	  case '.': return STk_eval(read_rec(f, case_significant), 
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
	  case '9': return read_cycle(f, c, case_significant);
	  default:  Ungetc(c, f); return read_token(f, '#', FALSE);
	}
      case ',': {
	SCM symb;

	c = Getc(f);
	if (c == '@') 
	  symb = Sym_unq_splicing;
	else {
	  symb = Sym_unquote; 
	  Ungetc(c, f);
	}
	return LIST2(symb, read_rec(f, case_significant));
      }
      case '"':
	return read_string(f);
      default:
	return read_token(f, c, case_significant);
    }
  }
}

SCM STk_readf(FILE *f, int case_significant)
{
  int c;

  if (cycles == NULL) STk_gc_protect(&cycles);
  cycles = NIL;

  c = flush_ws(f, (char *) NULL);
  if (c == EOF) return(STk_eof_object);
  Ungetc(c, f);
  return read_rec(f, case_significant);
}
