/*
 *
 * s t r . c				-- Strings management
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: ??????
 * Last file update:  3-Sep-1999 20:59 (eg)
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
    if (*str1 != *str2) return ((unsigned char) *str1 - (unsigned char) *str2);
  
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
  register SCM  z;

  NEWCELL(z, tc_string);
  z->storage_as.string.dim  = len;
  z->storage_as.string.data = (char *) must_malloc((unsigned int) len+1); 
  z->storage_as.string.data[len] = 0;

  if (init) memcpy(z->storage_as.string.data, init, (unsigned int) len);
  return z;
}

SCM STk_embed_C_string(char *str)
{
  /* Embed a C string in Scheme world (the dting must be dynamic */
  register SCM  z;

  NEWCELL(z, tc_string);
  z->storage_as.string.dim  = strlen(str);
  z->storage_as.string.data = str;

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

  ENTER_PRIMITIVE("make-string");
  if ((k=STk_integer_value(len)) < 0) Serror("bad string length", len);

  z = STk_makestrg(k, NULL);
      
  if (init_char != UNBOUND) {
    if (CHARP(init_char)) {
      char c = CHAR(init_char);
      int j;

      for(j=0 ;j<k; j++) z->storage_as.string.data[j] = c;
    }
    else
      Serror("initializing char not valid", init_char); }
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
  return STk_makechar((unsigned char) CHARS(str)[k]);
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
  return UNDEFINED; /* cannot occur */
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
    memcpy(p, CHARS(CAR(l)), (unsigned int) STRSIZE(CAR(l)));
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

  ENTER_PRIMITIVE("list->string");
  if (len < 0) Serror("bad list", l);
  z = STk_makestrg(len, NULL);

  for ( ; NNULLP(l); l=CDR(l)) {
    if (NCHARP(CAR(l))) Serror("not a character", CAR(l));
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

  ENTER_PRIMITIVE("string-fill");
  if (NSTRINGP(str)) Serror("not a string", str);
  if (NCHARP(c))     Serror("not a char", c);

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
    if (memcmp(s1, s2, (unsigned int) l2) == 0) return s1;

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
  z = STk_makestrg((int) strlen(CHARS(s)), NULL);

  for (p=CHARS(s), q=CHARS(z); *p; p++, q++) *q = tolower(*p);
  return z;
}

PRIMITIVE STk_string_upper(SCM s)
{
  SCM z;
  register char *p, *q;

  if (NSTRINGP(s)) Err("string-upper: not a string", s);
  z = STk_makestrg((int) strlen(CHARS(s)), NULL);

  for (p=CHARS(s), q=CHARS(z); *p; p++, q++) *q = toupper(*p);
  return z;
}

PRIMITIVE STk_split_string(SCM string, SCM delimiters)
{
  SCM result = NIL;
  char *c_string, *c_delimiters, *s;
  
  ENTER_PRIMITIVE("split-string");
  if (!STRINGP(string)) Serror("bad string", string);
  c_string = CHARS(string);

  if (delimiters == UNBOUND)
    c_delimiters = " \t\n";
  else {
    if (!STRINGP(delimiters)) 
      Serror("bad delimiter string", delimiters);
    c_delimiters = CHARS(delimiters);
  }

  for (s = c_string; *s; s++) {
    if (strchr(c_delimiters, *s)) {
      if (s > c_string) {
	int len;
	SCM tmp;
	
	len = s - c_string;
	tmp = STk_makestrg(len, NULL);
	
	strncpy(CHARS(tmp), c_string, (unsigned int) len);
	CHARS(tmp)[len] = '\0';
	result = Cons(tmp, result);
      }
      c_string = s + 1;
    }
  }
  if (s > c_string) 
    result = Cons(STk_makestring(c_string), result);

  return STk_reverse(result);
}
