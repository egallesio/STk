/*
 *
 *  l o c a l e . c			-- Locale management
 *
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
 *            Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 19-Sep-1998 12:01
 * Last file update:  3-Sep-1999 20:21 (eg)
 *
 *
 */

#include <locale.h>
#include <stdlib.h>
#include "stk.h"

static char bad_string_message[] = "comparing strings: bad string";
static char bad_char_message[]   = "comparing chars: bad char";

static char *locale_name = "";


/*==== Utilities ====*/

static int compare(unsigned char c1, unsigned char c2)
{
  unsigned char s1[2] = " ";
  unsigned char s2[2] = " ";
 
  /* This is really UGLY but can we write it in a cleaner way using
   * only pure ANSI-C primitives?  					  */
  *s1 = c1; *s2 = c2; 
  return strcoll((char *)s1, (char *)s2); 
}


static int stringcomp(SCM s1, SCM s2)
{
  register int l1, l2;
  register char *str1, *str2;
  
  if (NSTRINGP(s1)) Err(bad_string_message, s1); 
  if (NSTRINGP(s2)) Err(bad_string_message, s2);

  for (l1=STRSIZE(s1), str1=CHARS(s1), l2=STRSIZE(s2), str2=CHARS(s2);
       l1 && l2;
       l1--, str1++, l2--, str2++)
    if (*str1 != *str2)       
      return compare((unsigned char) *str1, 
		     (unsigned char) *str2);

  /* l1 == 0 || l2 == 0 */
  return l1 ? +1 : (l2 ? -1 : 0);
}

static int stringcompci(SCM s1, SCM s2)
{
  register int l1, l2;
  register char *str1, *str2;
  
  if (NSTRINGP(s1)) Err(bad_string_message, s1); 
  if (NSTRINGP(s2)) Err(bad_string_message, s2);

  for (l1=STRSIZE(s1), str1=CHARS(s1), l2=STRSIZE(s2), str2=CHARS(s2);
       l1 && l2;
       l1--, str1++, l2--, str2++)
    if (tolower(*str1) != tolower(*str2))       
      return compare((unsigned char) tolower(*str1), 
		     (unsigned char) tolower(*str2));

  /* l1 == 0 || l2 == 0 */
  return l1 ? +1 : (l2 ? -1 : 0);
}

static int charcomp(SCM c1, SCM c2)
{
  if (NCHARP(c1)) Err(bad_char_message, c1); 
  if (NCHARP(c2)) Err(bad_char_message, c2);
  
  return compare(CHAR(c1), CHAR(c2));
}

static int charcompci(SCM c1, SCM c2)
{
  if (NCHARP(c1)) Err(bad_char_message, c1); 
  if (NCHARP(c2)) Err(bad_char_message, c2);
  
  return compare((unsigned char) tolower(CHAR(c1)), 
		 (unsigned char) tolower(CHAR(c2)));
}

/*==== Primitives ====*/

static PRIMITIVE set_locale(SCM locale)
{
  char *res;

  ENTER_PRIMITIVE("set-locale!");

  if (!STRINGP(locale)) Serror("bad string", locale);

  if ((res=setlocale(LC_ALL, CHARS(locale))) == NULL)
    Serror("bad locale", locale);
  locale_name = res;
  return STk_makestring(res);
}

static PRIMITIVE get_locale(void)
{
  return STk_makestring(locale_name);
}

static PRIMITIVE eq (SCM s1,SCM s2){return (stringcomp(s1,s2)==0)? Truth: Ntruth;}
static PRIMITIVE lt (SCM s1,SCM s2){return (stringcomp(s1,s2)<0) ? Truth: Ntruth;}
static PRIMITIVE gt (SCM s1,SCM s2){return (stringcomp(s1,s2)>0) ? Truth: Ntruth;}
static PRIMITIVE le (SCM s1,SCM s2){return (stringcomp(s1,s2)<=0)? Truth: Ntruth;}
static PRIMITIVE ge (SCM s1,SCM s2){return (stringcomp(s1,s2)>=0)? Truth: Ntruth;}

static PRIMITIVE eqci(SCM s1,SCM s2){return (stringcompci(s1,s2)==0)? Truth:Ntruth;}
static PRIMITIVE ltci(SCM s1,SCM s2){return (stringcompci(s1,s2)<0) ? Truth:Ntruth;}
static PRIMITIVE gtci(SCM s1,SCM s2){return (stringcompci(s1,s2)>0) ? Truth:Ntruth;}
static PRIMITIVE leci(SCM s1,SCM s2){return (stringcompci(s1,s2)<=0)? Truth:Ntruth;}
static PRIMITIVE geci(SCM s1,SCM s2){return (stringcompci(s1,s2)>=0)? Truth:Ntruth;}

static PRIMITIVE Ceq (SCM c1, SCM c2){return (charcomp(c1,c2)==0) ? Truth: Ntruth;}
static PRIMITIVE Clt (SCM c1, SCM c2){return (charcomp(c1,c2)<0)  ? Truth: Ntruth;}
static PRIMITIVE Cgt (SCM c1, SCM c2){return (charcomp(c1,c2)>0)  ? Truth: Ntruth;}
static PRIMITIVE Cle (SCM c1, SCM c2){return (charcomp(c1,c2)<=0) ? Truth: Ntruth;}
static PRIMITIVE Cge (SCM c1, SCM c2){return (charcomp(c1,c2)>=0) ? Truth: Ntruth;}

static PRIMITIVE Ceqci(SCM c1, SCM c2){return (charcompci(c1,c2)==0)? Truth:Ntruth;}
static PRIMITIVE Cltci(SCM c1, SCM c2){return (charcompci(c1,c2)<0) ? Truth:Ntruth;}
static PRIMITIVE Cgtci(SCM c1, SCM c2){return (charcompci(c1,c2)>0) ? Truth:Ntruth;}
static PRIMITIVE Cleci(SCM c1, SCM c2){return (charcompci(c1,c2)<=0)? Truth:Ntruth;}
static PRIMITIVE Cgeci(SCM c1, SCM c2){return (charcompci(c1,c2)>=0)? Truth:Ntruth;}


PRIMITIVE STk_init_locale(void)
{
  char *lang = getenv("LANG");

  STk_add_new_primitive("set-locale!",     tc_subr_1, set_locale);
  STk_add_new_primitive("get-locale",      tc_subr_0, get_locale);

  STk_add_new_primitive("string-lo=?",     tc_subr_2, eq);
  STk_add_new_primitive("string-lo<?",     tc_subr_2, lt);
  STk_add_new_primitive("string-lo>?",     tc_subr_2, gt);
  STk_add_new_primitive("string-lo<=?",    tc_subr_2, le);
  STk_add_new_primitive("string-lo>=?",    tc_subr_2, ge);

  STk_add_new_primitive("string-lo-ci=?",  tc_subr_2, eqci);
  STk_add_new_primitive("string-lo-ci<?",  tc_subr_2, ltci);
  STk_add_new_primitive("string-lo-ci>?",  tc_subr_2, gtci);
  STk_add_new_primitive("string-lo-ci<=?", tc_subr_2, leci);
  STk_add_new_primitive("string-lo-ci>=?", tc_subr_2, geci);

  STk_add_new_primitive("char-lo=?",       tc_subr_2, Ceq);
  STk_add_new_primitive("char-lo<?",       tc_subr_2, Clt);
  STk_add_new_primitive("char-lo>?",       tc_subr_2, Cgt);
  STk_add_new_primitive("char-lo<=?",      tc_subr_2, Cle);
  STk_add_new_primitive("char-lo>=?", 	   tc_subr_2, Cge);

  STk_add_new_primitive("char-lo-ci=?",    tc_subr_2, Ceqci);
  STk_add_new_primitive("char-lo-ci<?",    tc_subr_2, Cltci);
  STk_add_new_primitive("char-lo-ci>?",    tc_subr_2, Cgtci);
  STk_add_new_primitive("char-lo-ci<=?",   tc_subr_2, Cleci);
  STk_add_new_primitive("char-lo-ci>=?",   tc_subr_2, Cgeci);
   
  locale_name = setlocale(LC_ALL, lang ? lang : "");
  if(!locale_name)
    /* LANG variable is erroneous, set locale to "C" */
    locale_name = setlocale(LC_ALL, "C");
  
  return UNDEFINED;
}
