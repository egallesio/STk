/*
 *
 * s t r . c				-- Strings management
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: ??????
 * Last file update: 13-May-1996 22:42
 */

#include <ctype.h>
#include "stk.h"

static char bad_string_message[] = "comparing string: bad string";
 
static int stringcomp(SCM s1, SCM s2)
{
  register int l1, l2;
  register char *str1, *str2;

  if (NSTRINGP(s1)) Err(bad_string_message, s1); 
  if (NSTRINGP(s2)) Err(bad_string_message, s2);

  for (l1=STRSIZE(s1), str1=CHARS(s1), l2=STRSIZE(s2), str2=CHARS(s2);
       l1 && l2;
       l1--, str1++, l2--, str2++)
    if (*str1 != *str2) return (*str1 - *str2);
  
  /* l1 == 0 || l2 == 0 */
  return l1 ? +1 : (l2 ? -1 : 0);
}


static int stringcompi(SCM s1, SCM s2)
{
  register int l1, l2;
  register char *str1, *str2;

  if (NSTRINGP(s1)) Err(bad_string_message, s1);
  if (NSTRINGP(s2)) Err(bad_string_message, s2);

  for (l1=STRSIZE(s1), str1=CHARS(s1), l2=STRSIZE(s2), str2=CHARS(s2);
       l1 && l2;
       l1--, str1++, l2--, str2++)
    if (tolower(*str1) != tolower(*str2))
	return (tolower(*str1) - tolower(*str2));

  /* l1 == 0 || l2 == 0 */
  return l1 ? +1 : (l2 ? -1 : 0);
}

SCM STk_makestrg(int len, char *init)
{
  SCM  z;

  STk_disallow_sigint();
  NEWCELL(z, tc_string);

  z->storage_as.string.dim  = len;
  z->storage_as.string.data = (char *) must_malloc(len+1); 
  z->storage_as.string.data[len] = 0;

  if (init) memcpy(z->storage_as.string.data, init, len);
  STk_allow_sigint();

  return z;
}


/**** Section 6.7 ****/

PRIMITIVE STk_stringp(SCM obj)
{
  return STRINGP(obj) ? Truth: Ntruth;
}

PRIMITIVE STk_make_string(SCM len, SCM init_char)
{
  long k;
  SCM z;

  if ((k=STk_integer_value(len)) < 0) Err("make-string: bad string length", len);

  z = STk_makestrg(k, NULL);
      
  if (init_char != UNBOUND) {
    if (CHARP(init_char)) {
      char c = CHAR(init_char);
      int j;

      for(j=0 ;j<k; j++) z->storage_as.string.data[j] = c;
    }
    else
      Err("make-string: initializing char not valid", init_char); }
  return z;
}

PRIMITIVE STk_lstring(SCM l, int len)
{
  int j;
  SCM tmp, z;

  if (len < 0) Err("string: bad list", l);
  z = STk_makestrg(len, NULL);

  /* copy element in newly allocated string */
  for (j=0; j < len; j++, l=CDR(l)) {
    tmp = CAR(l);
    if (NCHARP(tmp)) Err("string: bad element", tmp);
    CHARS(z)[j] = CHAR(tmp);
  }
  return z;
}

PRIMITIVE STk_string_length(SCM str)
{
  if (NSTRINGP(str)) Err("string-length: not a string", str);
  return STk_makeinteger(STRSIZE(str));
}

PRIMITIVE STk_string_ref(SCM str, SCM index)
{
  long k;

  if (NSTRINGP(str))    	        Err("string-ref: not a string", str);
  if ((k=STk_integer_value(index)) < 0) Err("string-ref: bad index", index);

  if (k >= STRSIZE(str)) 
    Err("string-ref: index out of bounds", index);
  return STk_makechar(CHARS(str)[k]);
}

PRIMITIVE STk_string_set(SCM str, SCM index, SCM value)
{
  long k;

  if (NSTRINGP(str))		        Err("string-set!: not a string", str); 
  if ((k=STk_integer_value(index)) < 0) Err("string-set!: bad index", index); 
  
  if (k >= STRSIZE(str)) 
    Err("string-set!: index out of bounds", index);
    
  if (NCHARP(value)) Err("string-set!: value is not a char", value);
    
  CHARS(str)[k] = CHAR(value);
  return UNDEFINED;
}

PRIMITIVE STk_streq   (SCM s1,SCM s2){return (stringcomp(s1,s2)==0)? Truth: Ntruth;}
PRIMITIVE STk_strless (SCM s1,SCM s2){return (stringcomp(s1,s2)<0) ? Truth: Ntruth;}
PRIMITIVE STk_strgt   (SCM s1,SCM s2){return (stringcomp(s1,s2)>0) ? Truth: Ntruth;}
PRIMITIVE STk_strlesse(SCM s1,SCM s2){return (stringcomp(s1,s2)<=0)? Truth: Ntruth;}
PRIMITIVE STk_strgte  (SCM s1,SCM s2){return (stringcomp(s1,s2)>=0)? Truth: Ntruth;}

PRIMITIVE STk_streqi   (SCM s1,SCM s2){return (stringcompi(s1,s2)==0)?Truth:Ntruth;}
PRIMITIVE STk_strlessi (SCM s1,SCM s2){return (stringcompi(s1,s2)<0) ?Truth:Ntruth;}
PRIMITIVE STk_strgti   (SCM s1,SCM s2){return (stringcompi(s1,s2)>0) ?Truth:Ntruth;}
PRIMITIVE STk_strlessei(SCM s1,SCM s2){return (stringcompi(s1,s2)<=0)?Truth:Ntruth;}
PRIMITIVE STk_strgtei  (SCM s1,SCM s2){return (stringcompi(s1,s2)>=0)?Truth:Ntruth;}

PRIMITIVE STk_substring(SCM string, SCM start, SCM end)
{
  char msg[] = "substring: not an integer";
  int from, to;

  if (NSTRINGP(string)) Err("substring: not a string", string);

  if ((from=STk_integer_value(start))==LONG_MIN) Err(msg ,start);
  if ((to=STk_integer_value(end)) == LONG_MIN)   Err(msg ,end);

  if (0 <= from && from <= to && to <= STRSIZE(string))
    return STk_makestrg(to - from, CHARS(string)+from);

  Err("substring: bad bounds", Cons(start, end));
}

PRIMITIVE STk_string_append(SCM l, int len)
{
  int i, total=0;
  SCM z, tmp = l;
  char *p;
  
  /* Compute total length of resulting string */
  for (i = 0; i < len; i++) {
    if (NSTRINGP(CAR(tmp)))
      Err("string-append: bad string", CAR(tmp));
    total += STRSIZE(CAR(tmp));
    tmp = CDR(tmp);
  }

  /* Allocate result */
  z = STk_makestrg(total, NULL);
  p = CHARS(z);
  
  /* copy strings */
  for (i=0; i < len; i++) {
    memcpy(p, CHARS(CAR(l)), STRSIZE(CAR(l)));
    p += STRSIZE(CAR(l));
    l = CDR(l);
  }
  return z;
}

PRIMITIVE STk_string2list(SCM str)
{
  int j, len;
  SCM tmp, z = NIL;

  if (NSTRINGP(str)) Err("string->list: not a string", str);
  len = STRSIZE(str);

  for (j=0; j<len; j++) {
    if (j == 0)
      tmp = z = Cons(STk_makechar(CHARS(str)[j]), NIL);
    else 
      tmp = CDR(tmp) = Cons(STk_makechar(CHARS(str)[j]), NIL);
  }
  return z;
}

PRIMITIVE STk_list2string(SCM l)
{
  int j=0, len = STk_llength(l);
  SCM z;

  if (len < 0) Err("list->string: bad list", l);
  z = STk_makestrg(len, NULL);
  for ( ; NNULLP(l); l=CDR(l)) {
    if (NCHARP(CAR(l))) Err("list->string: not a character", CAR(l));
    CHARS(z)[j++] = CHAR(CAR(l));
  }
  return z;
}

PRIMITIVE STk_string_copy(SCM str)
{
  if (NSTRINGP(str)) Err("string-copy: not a string", str);
  return STk_makestring(CHARS(str));
}

PRIMITIVE STk_string_fill(SCM str, SCM c)
{
  int len, i;
  char c_char;

  if (NSTRINGP(str)) Err("string-fill: not a string", str);
  if (NCHARP(c))     Err("string-fill: not a char", c);

  len = STRSIZE(str);
  c_char = CHAR(c);

  for (i = 0; i < len; i++)
    CHARS(str)[i] = c_char;
  return UNDEFINED;
}


/*
 * 
 * STk bonus
 *
 */

static char *Memmem(char *s1, int l1, char *s2, int l2)
{
  if (l2 == 0) return s1;

  for ( ; l1 >= l2 ; s1++, l1--)
    if (memcmp(s1, s2, l2) == 0) return s1;

  return NULL;
}

PRIMITIVE STk_string_findp(SCM s1, SCM s2)
{
  char msg[] = "string-find?: bad string";

  if (NSTRINGP(s1)) Err(msg,s1);
  if (NSTRINGP(s2)) Err(msg,s2);
  
  return Memmem(CHARS(s2), STRSIZE(s2), CHARS(s1), STRSIZE(s1)) ? Truth: Ntruth;
}

PRIMITIVE STk_string_index(SCM s1, SCM s2)
{
  char *p, msg[] = "string-index: bad string";

  if (NSTRINGP(s1)) Err(msg,s1);
  if (NSTRINGP(s2)) Err(msg,s2);

  p = Memmem(CHARS(s2), STRSIZE(s2), CHARS(s1), STRSIZE(s1));
  
  return p? STk_makeinteger(p - CHARS(s2)) : Ntruth;
}



PRIMITIVE STk_string_lower(SCM s)
{
  SCM z;
  register char *p, *q;

  if (NSTRINGP(s)) Err("string-lower: not a string", s);
  z = STk_makestrg(strlen(CHARS(s)), NULL);

  for (p=CHARS(s), q=CHARS(z); *p; p++, q++) *q = tolower(*p);
  return z;
}

PRIMITIVE STk_string_upper(SCM s)
{
  SCM z;
  register char *p, *q;

  if (NSTRINGP(s)) Err("string-upper: not a string", s);
  z = STk_makestrg(strlen(CHARS(s)), NULL);

  for (p=CHARS(s), q=CHARS(z); *p; p++, q++) *q = toupper(*p);
  return z;
}
