/*
 *
 * s y m b o l . c			-- Symbols management
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
 *    Creation date: 20-Nov-1993 12:12
 * Last file update:  3-Sep-1999 20:59 (eg)
 */

#include "stk.h"

static Tcl_HashTable obarray;

void STk_initialize_symbol_table(void)
{
  Tcl_InitHashTable(&obarray, TCL_STRING_KEYS);
}

void STk_mark_symbol_table(void)
{
  register SCM x;
  Tcl_HashEntry *ent;
  Tcl_HashSearch tmp;

  for (ent=Tcl_FirstHashEntry(&obarray, &tmp); ent;  ent=Tcl_NextHashEntry(&tmp)) {
    x = (SCM) Tcl_GetHashValue(ent); 
    if (VCELL(x) != UNBOUND || (x->cell_info & CELL_INFO_C_VAR)) STk_gc_mark(x);
  }
}

void STk_free_symbol(SCM symbol)
{
  if (symbol->cell_info & CELL_INFO_UNINTERNED)
    free(PNAME(symbol));
  else
    Tcl_DeleteHashEntry(Tcl_FindHashEntry(&obarray, PNAME(symbol)));
}


SCM STk_intern(char *name)
{
  Tcl_HashEntry *p;

  if ((p = Tcl_FindHashEntry(&obarray, name)))
    return (SCM) Tcl_GetHashValue(p);
  else {
    SCM sym;
    int absent;
    
    /* Be careful with GC: Create hash entry after the new cell to avoid 
     * partially initialized table entry 
     */
    NEWCELL(sym, tc_symbol);
    p 	       = Tcl_CreateHashEntry(&obarray, name, &absent);
    PNAME(sym) = Tcl_GetHashKey(&obarray, p);
    VCELL(sym) = UNBOUND;
    Tcl_SetHashValue(p, (ClientData) sym);
    return sym;
  }
}


SCM STk_global_env2list(void)
{
  register SCM symbol, res = NIL;
  Tcl_HashEntry *ent;
  Tcl_HashSearch tmp;

  for (ent=Tcl_FirstHashEntry(&obarray, &tmp); ent;  ent=Tcl_NextHashEntry(&tmp)) {
    symbol = (SCM)Tcl_GetHashValue(ent);
    res    = Cons(Cons(symbol, VCELL(symbol)), res);
  }
  return res;
}

SCM STk_global_symbols(void)
{
  register SCM symbol, res = NIL;
  Tcl_HashEntry *ent;
  Tcl_HashSearch tmp;

  for (ent=Tcl_FirstHashEntry(&obarray, &tmp); ent;  ent=Tcl_NextHashEntry(&tmp)) {
    symbol = (SCM)Tcl_GetHashValue(ent);
    if (VCELL(symbol) != UNBOUND) res = Cons(symbol, res);
  }
  return res;
}

PRIMITIVE STk_symbolp(SCM x)
{
  return SYMBOLP(x) ? Truth : Ntruth;
}

PRIMITIVE STk_symbol2string(SCM symbol)
{  
  if (NSYMBOLP(symbol)) Err("symbol->string: bad symbol", symbol);
  return STk_makestring(PNAME(symbol));
}
   
PRIMITIVE STk_string2symbol(SCM string)
{
  if (NSTRINGP(string)) Err("string->symbol: bad string", string);
  return Intern(CHARS(string));
}

PRIMITIVE STk_string2usymbol(SCM string)
{
  SCM z;
  char *s;

  if (NSTRINGP(string)) Err("string->uninterned-symbol: bad string", string);
  
  s = must_malloc(strlen(CHARS(string)) + 1);
  strcpy(s, CHARS(string));

  NEWCELL(z, tc_symbol);
  PNAME(z) = s;
  VCELL(z) = UNBOUND;
  z->cell_info = CELL_INFO_UNINTERNED;	/* retain that this symbol is uninterned */

  return z;
}
