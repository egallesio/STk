/*
 * r e a d  . c				-- reading stuff
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
 * Last file update: 17-May-1996 18:39
 *
 */

#include <ctype.h>
#include "stk.h"

static SCM lreadr(FILE *f, int case_significant);


static int flush_ws(FILE *f, char *message)
{
  int c, commentp=0;

  c = Getc(f);
  for ( ; ; ) {
    switch (c) {
      case EOF:  if (message) Err(message,NIL); else return(c);
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

static SCM lreadlist(FILE *f, char delim, int case_significant)
/* Read a list ended by the `delim' char */
{
  int c;
  SCM tmp;
  
  c = flush_ws(f, "end of file inside list");
  if (c == delim) return(NIL);

  /* Read the car */
  Ungetc(c, f);
  tmp = lreadr(f, case_significant);
  
  /* Read the cdr */
  if (EQ(tmp, Sym_dot)) {
    tmp = lreadr(f, case_significant);
    c = flush_ws(f, "end of file inside list");
    if (c != delim) Err("missing close paren", NIL);
    return(tmp);
  }
  return(Cons(tmp, lreadlist(f, delim, case_significant)));
}

static void lreadword(FILE *f, int c, int case_significant) 
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
    if (j >= TKBUFFERN-1) Err("read: token too large", NIL);
  }

  STk_tkbuffer[j] = '\0';
}

static void lreadchar(FILE *f, int c)
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
    if (j >= TKBUFFERN-1) Err("read: token too large", NIL);
  }
  STk_tkbuffer[j] = '\0';
}
  
static SCM lreadtoken(FILE *f, int c, int case_significant) 
{
  SCM z;

  lreadword(f, c, case_significant);
  z = STk_Cstr2number(STk_tkbuffer, 10L);

  if (z == Ntruth)
    /* It is not a number */
    return (*STk_tkbuffer == ':') ? STk_makekey(STk_tkbuffer): Intern(STk_tkbuffer);

  /* Return the number read */
  return z;
}

static SCM lreadstring(FILE *f)
{
  int j, k ,c,n,len;
  char *p, *buffer;
  SCM z;

  j = 0;
  len = 100;
  p = buffer = must_malloc(len);

  while(((c = Getc(f)) != '"') && (c != EOF)) { 
    if (c == '\\') {
      c = Getc(f);
      if (c == EOF) Err("eof after \\", NIL);
      switch(c) {
        case 'b' : c = '\b'; break;	/* Bs  */
	case 'e' : c = 0x1b; break;	/* Esc */
	case 'n' : c = '\n'; break;	/* Lf  */
	case 'r' : c = '\r'; break;	/* Cr  */
	case 't' : c = '\t'; break;	/* Tab */
        case '\n': STk_line_counter += 1; continue;
	case '0' : for( k=n=0 ; ; k++ ) {
		     c = Getc(f);
		     if (c == EOF) Err("eof after \\0", NIL);
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
  if (c == EOF) Err("end of file while reading a string", NIL);
  *p = '\0';
  
  z = STk_makestrg(j, buffer);
  free(buffer);

  return z;
}

static SCM lreadr(FILE *f, int case_significant)
{
  int c;

  for ( ; ; ) {
    c = flush_ws(f, "end of file inside read encountered");
    
    switch (c) {
      case '(':
        return(lreadlist(f, ')', case_significant));
      case '[':
	return(lreadlist(f, ']', case_significant));
      case ')':
      case ']':
	fprintf(STk_stderr, "\nunexpected close parenthesis");
	if (STk_current_filename != UNBOUND)	
	  fprintf(STk_stderr, " at line %d in file %s", 
		  	      STk_line_counter, CHARS(STk_current_filename));
	fprintf(STk_stderr, "\n");
	break;
      case '\'':
	return LIST2(Sym_quote, lreadr(f, case_significant));
      case '`':
	return LIST2(Sym_quasiquote, lreadr(f, case_significant));
      case '#':
	switch(c=Getc(f)) {
	  case 't':
          case 'T':  return Truth;
	  case 'f':
	  case 'F':  return Ntruth;
 	  case '\\': lreadchar(f, Getc(f));
	             return STk_makechar(STk_string2char(STk_tkbuffer));
	  case '(' : { 
	    	       SCM l = lreadlist(f, ')', case_significant);
		       return STk_vector(l, STk_llength(l));
	  	     }
	  case '!' : while ((c=Getc(f)) != '\n')
	               if (c == EOF) Err("eof encountered in a #! notation", NIL);
		     Ungetc(c, f);
	             continue;
	  case 'p':
	  case 'P': lreadword(f, Getc(f), TRUE);
	    	    return STk_address2object(STk_tkbuffer);
	  case '.': return STk_eval(lreadr(f, case_significant), NIL);
	  default:  Ungetc(c, f); return lreadtoken(f, '#', FALSE);
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
	return LIST2(symb, lreadr(f, case_significant));
      }
      case '"':
	return lreadstring(f);
      default:
	return lreadtoken(f, c, case_significant);
    }
  }
}

SCM STk_readf(FILE *f, int case_significant)
{
  int c;
  
  c = flush_ws(f, (char *) NULL);
  if (c == EOF) return(STk_eof_object);
  Ungetc(c, f);
  return lreadr(f, case_significant);
}
