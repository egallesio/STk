/*
 *
 * c h a r . c				-- Characters management
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: ??????
 * Last file update:  3-Sep-1999 20:18 (eg)
 */

#include <ctype.h>
#include "stk.h"

struct charelem {
  char *name;
  unsigned char value;
};

static struct charelem chartable [] = { 
  {"null",       '\000'},
  {"bell",       '\007'},
  {"backspace",  '\010'},
  {"newline",    '\012'},
  {"page",       '\014'},
  {"return",     '\015'},
  {"escape",     '\033'},
  {"space",      '\040'},
  {"delete",     '\177'},

  /* poeticless names */
  {"nul",        '\000'},
  {"soh",        '\001'},
  {"stx",        '\002'},
  {"etx",        '\003'},
  {"eot",        '\004'},
  {"enq",        '\005'},
  {"ack",        '\006'},
  {"bel",        '\007'},

  {"bs",         '\010'},
  {"ht",         '\011'},  /*and also */ {"tab", '\011'},
  {"nl",         '\012'},
  {"vt",         '\013'},
  {"np",         '\014'},
  {"cr",         '\015'},
  {"so",         '\016'},
  {"si",         '\017'},

  {"dle",        '\020'},
  {"dc1",        '\021'},
  {"dc2",        '\022'},
  {"dc3",        '\023'},
  {"dc4",        '\024'},
  {"nak",        '\025'},
  {"syn",        '\026'},
  {"etb",        '\027'},

  {"can",        '\030'},
  {"em",         '\031'},
  {"sub",        '\032'},
  {"esc",        '\033'},
  {"fs",         '\034'},
  {"gs",         '\035'},
  {"rs",         '\036'},
  {"us",         '\037'},

  {"sp",	'\040'},
  {"del",	'\177'},
  
  {"",           '\000'}
};


static int my_strcmpi(register char *p1, register char *p2)
{
  for( ; tolower(*p1) == tolower(*p2); p1++, p2++) 
    if (!*p1) return 0;

  return tolower(*p1) - tolower(*p2);
}



unsigned char STk_string2char(char *s)
/* converts a char name to a char */
{
  register struct charelem *p;
  
  if (s[1] == '\000') return s[0];
  for (p=chartable; *(p->name); p++) {
    if (my_strcmpi(p->name, s) == 0) return p->value;
  }
  Err("Bad char name", NIL);
  return '\0'; /* never reached */
}

unsigned char *STk_char2string(unsigned char c)  /* convert a char to it's */
{						 /* external representation */
  static unsigned char result[2] = " ";  /* sets the \0 */
  register struct charelem *p;

  for (p=chartable; *(p->name); p++)
    if (p->value == c) return (unsigned char *) p->name;
  
  /* If we are here it's a "normal" char */
  *result = c;
  return result;
}

SCM STk_makechar(unsigned char c)
{
  SCM z;

#ifndef COMPACT_SMALL_CST
  NEWCELL(z,tc_char);
#endif
  SET_CHARACTER(z, (int) c);
  return z;
}


/**** Section 6.6 ****/

PRIMITIVE STk_charp(SCM obj)
{
  return CHARP(obj) ? Truth: Ntruth;
}

static int charcomp(SCM c1, SCM c2)
{
  if (NCHARP(c1)) Err("comparing char: bad char", c1); 
  if (NCHARP(c2)) Err("comparing char: bad char", c2);
  
  return (CHAR(c1) - CHAR(c2));
}

static int charcompi(SCM c1, SCM c2)
{
  if (NCHARP(c1)) Err("comparing char: bad char", c1); 
  if (NCHARP(c2)) Err("comparing char: bad char", c2);
  
  return (tolower(CHAR(c1)) - tolower(CHAR(c2)));
}
  
PRIMITIVE STk_chareq   (SCM c1, SCM c2){return (charcomp(c1,c2)==0) ?Truth: Ntruth;}
PRIMITIVE STk_charless (SCM c1, SCM c2){return (charcomp(c1,c2)<0)  ?Truth: Ntruth;}
PRIMITIVE STk_chargt   (SCM c1, SCM c2){return (charcomp(c1,c2)>0)  ?Truth: Ntruth;}
PRIMITIVE STk_charlesse(SCM c1, SCM c2){return (charcomp(c1,c2)<=0) ?Truth: Ntruth;}
PRIMITIVE STk_chargte  (SCM c1, SCM c2){return (charcomp(c1,c2)>=0) ?Truth: Ntruth;}

PRIMITIVE STk_chareqi   (SCM c1, SCM c2){return (charcompi(c1,c2)==0)?Truth:Ntruth;}
PRIMITIVE STk_charlessi (SCM c1, SCM c2){return (charcompi(c1,c2)<0) ?Truth:Ntruth;}
PRIMITIVE STk_chargti   (SCM c1, SCM c2){return (charcompi(c1,c2)>0) ?Truth:Ntruth;}
PRIMITIVE STk_charlessei(SCM c1, SCM c2){return (charcompi(c1,c2)<=0)?Truth:Ntruth;}
PRIMITIVE STk_chargtei  (SCM c1, SCM c2){return (charcompi(c1,c2)>=0)?Truth:Ntruth;}

PRIMITIVE STk_char_alphap(SCM c)
{
  if (NCHARP(c)) Err("char-alphabetic?: bad character", c);
  return isalpha(CHAR(c))? Truth: Ntruth;
}

PRIMITIVE STk_char_numericp(SCM c)
{
  if (NCHARP(c)) Err("char-numeric?: bad character", c);
  return isdigit(CHAR(c))? Truth: Ntruth;
}

PRIMITIVE STk_char_whitep(SCM c)
{
  if (NCHARP(c)) Err("char-whitespace?: bad character", c);
  return isspace(CHAR(c))? Truth: Ntruth;
}

PRIMITIVE STk_char_upperp(SCM c)
{
  if (NCHARP(c)) Err("char-upper-case?: bad character", c);
  return isupper(CHAR(c))? Truth: Ntruth;
}

PRIMITIVE STk_char_lowerp(SCM c)
{
  if (NCHARP(c)) Err("char-lower-case?: bad character", c);
  return islower(CHAR(c))? Truth: Ntruth;
}

PRIMITIVE STk_char2integer(SCM c)
{
  if (NCHARP(c)) Err("char->integer: bad character", c);
  return STk_makeinteger((long) CHAR(c));
}

PRIMITIVE STk_integer2char(SCM i)
{
  int c = STk_integer_value(i);

  if (c < 0 || c > MAX_CHAR_CODE) Err("integer->char: bad integer", i);
  return STk_makechar((unsigned char) c);
}

PRIMITIVE STk_char_upper(SCM c)
{
  if (NCHARP(c)) Err("char-upcase: bad character", c);
  return STk_makechar((unsigned char) toupper(CHAR(c)));
}

PRIMITIVE STk_char_lower(SCM c)
{
  if (NCHARP(c)) Err("char-downcase: bad character", c);
  return STk_makechar((unsigned char) tolower(CHAR(c)));
}
