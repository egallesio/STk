/*
 *
 * k e y w o r d . c				-- Keywords management
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
 *    Creation date: 19-Nov-1993 16:12
 * Last file update: 13-May-1996 22:40
 */

#include "stk.h"

static Tcl_HashTable k_table;

void STk_initialize_keyword_table(void)
{
  Tcl_InitHashTable(&k_table, TCL_STRING_KEYS);
}

void STk_free_keyword(SCM keyword)
{
  Tcl_DeleteHashEntry(Tcl_FindHashEntry(&k_table, KEYVAL(keyword)));
}

SCM STk_makekey(char *token)
{
  Tcl_HashEntry *p;

  *token = '-';  /* because keywords corresponds to Tk options */
  if (p = Tcl_FindHashEntry(&k_table, token))
    return (SCM) Tcl_GetHashValue(p);
  else {
    SCM keyword;
    int absent;
    
   /* Be careful with GC: Create hash entry after the new cell to avoid 
    * partially initialized table entry 
    */
    NEWCELL(keyword, tc_keyword);
    p 	            = Tcl_CreateHashEntry(&k_table, token, &absent);
    KEYVAL(keyword) = Tcl_GetHashKey(&k_table, p);
    Tcl_SetHashValue(p, (ClientData) keyword);
    return keyword;
  }
}

PRIMITIVE STk_make_keyword(SCM str)
{
  SCM z;
  char *s, *copy;

  switch (TYPE(str)) {
    case tc_string: s = CHARS(str); break;
    case tc_symbol: s = PNAME(str); break;
    default:        Err("make-keyword: Bad parameter", str);
  }

  copy = must_malloc(strlen(s)+2);
  strcpy(copy+1, s); /* copy[0] will be set to '-' in STk_makekey */
  z = STk_makekey(copy);
  free(copy);

  return z;
}

PRIMITIVE STk_keywordp(SCM obj)
{
  return KEYWORDP(obj)? Truth : Ntruth;
}

PRIMITIVE STk_keyword2string(SCM obj)
{
  SCM res;

  if (NKEYWORDP(obj)) Err("keyword->string: bad keyword", obj);
  res = STk_makestring(KEYVAL(obj));
  CHARS(res)[0] = ':';
  return res;
}


PRIMITIVE STk_get_keyword(SCM key, SCM l, SCM default_value)
{
  if (NKEYWORDP(key))   Err("get-keyword: bad keyword", key);
  if (CONSP(l) || NULLP(l)) {
    int i, len = STk_llength(l);

    if (len< 0 || len&1) goto Error;

    for (i = 0; i < len; i+=2) {
      if (NKEYWORDP(CAR(l))) Err("get-keyword: bad keyword", CAR(l));
      if (strcmp(KEYVAL(CAR(l)), KEYVAL(key)) == 0) return CAR(CDR(l));
      l = CDR(CDR(l));
    }
    if (default_value == UNBOUND) Err("get-keyword: value not found for", key);
    return default_value;
  }
Error:
    Err("get-keyword: bad list", l);
}
