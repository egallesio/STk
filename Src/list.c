/*
 *
 * l i s t . c			-- Lists procedures
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
 *    Creation date: ??-Oct-1993 21:37
 * Last file update:  7-Nov-1995 21:36
 */

#include "stk.h"

PRIMITIVE STk_pairp(SCM x)
{
  return CONSP(x) ? Truth : Ntruth;
}

PRIMITIVE STk_cons(SCM x, SCM y)
{
  SCM z;
  NEWCELL(z,tc_cons);
  CAR(z) = x;
  CDR(z) = y;
  return z;
}

PRIMITIVE STk_car(SCM x)
{
  if (TYPEP(x, tc_cons)) return CAR(x);
  Err("car: wrong type of argument", x);
}

PRIMITIVE STk_cdr(SCM x)
{
  if (TYPEP(x, tc_cons)) return CDR(x);
  Err("cdr: wrong type of argument", x);
}

PRIMITIVE STk_setcar(SCM cell, SCM value)
{
  if NCONSP(cell) Err("set-car!: wrong type of argument", cell);
  CAR(cell) = value;
  return UNDEFINED;
}

PRIMITIVE STk_setcdr(SCM cell, SCM value)
{
  if NCONSP(cell) Err("set-cdr!: wrong type of argument", cell);
  CDR(cell) = value;
  return UNDEFINED;
}

static SCM internal_cxr(SCM l, char *fct)
{
  register SCM tmp = l;
  register char *p;

  for(p = fct + strlen(fct)-1; *p != 'X'; p--) {
    if (NCONSP(tmp)) {
      char name[50];
      sprintf(name, "c%sr: bad list", fct+1);
      Err(name, l);
    }
    tmp = (*p == 'a') ? CAR(tmp) : CDR(tmp);
  }
  return tmp;
}

PRIMITIVE STk_caar  (SCM l) { return internal_cxr(l, "Xaa");   }
PRIMITIVE STk_cdar  (SCM l) { return internal_cxr(l, "Xda");   }
PRIMITIVE STk_cadr  (SCM l) { return internal_cxr(l, "Xad");   }
PRIMITIVE STk_cddr  (SCM l) { return internal_cxr(l, "Xdd");   }
PRIMITIVE STk_caaar (SCM l) { return internal_cxr(l, "Xaaa");  }
PRIMITIVE STk_cdaar (SCM l) { return internal_cxr(l, "Xdaa");  }
PRIMITIVE STk_cadar (SCM l) { return internal_cxr(l, "Xada");  }
PRIMITIVE STk_cddar (SCM l) { return internal_cxr(l, "Xdda");  }
PRIMITIVE STk_caadr (SCM l) { return internal_cxr(l, "Xaad");  }
PRIMITIVE STk_cdadr (SCM l) { return internal_cxr(l, "Xdad");  }
PRIMITIVE STk_caddr (SCM l) { return internal_cxr(l, "Xadd");  }
PRIMITIVE STk_cdddr (SCM l) { return internal_cxr(l, "Xddd");  }
PRIMITIVE STk_caaaar(SCM l) { return internal_cxr(l, "Xaaaa"); }
PRIMITIVE STk_cdaaar(SCM l) { return internal_cxr(l, "Xdaaa"); }
PRIMITIVE STk_cadaar(SCM l) { return internal_cxr(l, "Xadaa"); }
PRIMITIVE STk_cddaar(SCM l) { return internal_cxr(l, "Xddaa"); }
PRIMITIVE STk_caadar(SCM l) { return internal_cxr(l, "Xaada"); }
PRIMITIVE STk_cdadar(SCM l) { return internal_cxr(l, "Xdada"); }
PRIMITIVE STk_caddar(SCM l) { return internal_cxr(l, "Xadda"); }
PRIMITIVE STk_cdddar(SCM l) { return internal_cxr(l, "Xddda"); }
PRIMITIVE STk_caaadr(SCM l) { return internal_cxr(l, "Xaaad"); }
PRIMITIVE STk_cdaadr(SCM l) { return internal_cxr(l, "Xdaad"); }
PRIMITIVE STk_cadadr(SCM l) { return internal_cxr(l, "Xadad"); }
PRIMITIVE STk_cddadr(SCM l) { return internal_cxr(l, "Xddad"); }
PRIMITIVE STk_caaddr(SCM l) { return internal_cxr(l, "Xaadd"); }
PRIMITIVE STk_cdaddr(SCM l) { return internal_cxr(l, "Xdadd"); }
PRIMITIVE STk_cadddr(SCM l) { return internal_cxr(l, "Xaddd"); }
PRIMITIVE STk_cddddr(SCM l) { return internal_cxr(l, "Xdddd"); }

PRIMITIVE STk_nullp(SCM x)
{
  return EQ(x, NIL) ? Truth: Ntruth;
}

int STk_llength(SCM l)
{
  register SCM start = l;
  register int len   = 0;
	
  for ( ; ; ) {
    if (NULLP(l)) return len;
    if ((l == start && len) || NCONSP(l)) return -1;
    l = CDR(l);
    len += 1;
  }
}

PRIMITIVE STk_listp(SCM x)
{
  return (STk_llength(x) < 0) ? Ntruth : Truth;
}

PRIMITIVE STk_list(SCM l, int len)
{
  /* len is unused here */
  return l;
}

PRIMITIVE STk_list_length(SCM l)
{
  int len = STk_llength(l);
  if (len >= 0) return STk_makeinteger((long) len);
  Err("length: not calculable.", NIL);
}

static SCM append2(SCM l1, SCM l2)
{
  register SCM res, p;

  if (NULLP(l1)) return l2;
  if (NCONSP(l1)) goto Error;

  for (res = NIL; ; l1 = CDR(l1)) {
    if (NCONSP(l1))      goto Error;
    if (res == NIL){
      NEWCELL(res, tc_cons);
      p = res;
    }
    else {
      NEWCELL(CDR(p), tc_cons);
      p = CDR(p);
    }
    CAR(p) = CAR(l1);
    CDR(p) = NIL;		/* Keep alwys a valid list in case of a GC */
    if (NCONSP(CDR(l1))) break;
  }
  CDR(p) = l2;
  return res;
Error: 
   Err("append: argument is not a list", l1);
}

PRIMITIVE STk_append(SCM l, int len)
{
  switch (len) {
    case 0:  return NIL;
    case 1:  return CAR(l);
    case 2:  return append2(CAR(l), CAR(CDR(l)));
    default: return append2(CAR(l), STk_append(CDR(l), len-1));
  }
}

PRIMITIVE STk_reverse(SCM l)
{
  SCM p, n = NIL;

  for(p=l; NNULLP(p); p=CDR(p)) {
    if (NCONSP(p)) Err("reverse: bad list", l);
    n = Cons(CAR(p),n);
  }
  return n;
}

PRIMITIVE STk_list_tail(SCM list, SCM k)
{
  register long x;

  if (NCONSP(list)) Err("list-tail: Bad list", list);
  x = STk_integer_value(k);
  if (x >= 0) {
    SCM l = list;

    for (l=list; x > 0; x--) {
      if (NULLP(l) || NCONSP(l)) Err("list-tail: list too short", list);
      l = CDR(l);
    }
    return l;
  }
  Err("list-tail: index must be exact positive integer", k);
}

PRIMITIVE STk_list_ref(SCM list, SCM k)
{
  register long x;

  if (NCONSP(list)) Err("list-ref: Bad list", list);	
  x = STk_integer_value(k);
  if (x >= 0) {
    SCM l = list;

    for ( ; x > 0; x--) {
      if (NULLP(l) || NCONSP(l)) goto Error;
      l = CDR(l);
    }
    
    if (CONSP(l)) return CAR(l);
  Error: 
    Err("list-ref: list too short", list);
  }
  Err("list-ref: index must be exact positive integer", k);
}

static SCM lmember(SCM obj, SCM list,  SCM (*predicate)(SCM, SCM) )
{
  register SCM ptr;
	
  if (NCONSP(list) && NNULLP(list)) goto Error;
  for (ptr=list; NNULLP(ptr); ) { 
    if (CONSP(ptr)) {
      if ((*predicate)(CAR(ptr), obj) == Truth) return ptr;
    }
    else 
      /* end of a dotted list */
      return ((*predicate)(ptr, obj) == Truth) ? ptr : Ntruth;
    if ((ptr=CDR(ptr)) == list) goto Error;
  }
  return Ntruth;
Error:
  Err("member function: Bad list", list);
}

PRIMITIVE STk_memq  (SCM obj, SCM list)	{return lmember(obj, list, STk_eq);   }
PRIMITIVE STk_memv  (SCM obj, SCM list)	{return lmember(obj, list, STk_eqv);  }
PRIMITIVE STk_member(SCM obj, SCM list)	{return lmember(obj, list, STk_equal);}

static SCM lassoc(SCM obj, SCM alist, SCM (*predicate)(SCM, SCM))
{
  register SCM l,tmp;
	
  for(l=alist; CONSP(l); ) {
    tmp = CAR(l);
    if (CONSP(tmp) && (*predicate)(CAR(tmp), obj) == Truth) return tmp;
    if ((l=CDR(l)) == alist) goto Error;
  }
  if (NULLP(l)) return(Ntruth);
Error:
  Err("assoc function: improper list", alist);
}

PRIMITIVE STk_assq (SCM obj, SCM alist){return lassoc(obj, alist, STk_eq);   }
PRIMITIVE STk_assv (SCM obj, SCM alist){return lassoc(obj, alist, STk_eqv);  }
PRIMITIVE STk_assoc(SCM obj, SCM alist){return lassoc(obj, alist, STk_equal);}


/***
 *
 * Non standard functions 
 *
 ***/

PRIMITIVE STk_liststar(SCM l, int len)
{
  if (len == 0) return NIL;
  /* l is a pair */
  return (len == 1) ? CAR(l) : STk_cons(CAR(l), STk_liststar(CDR(l), len-1));
}

PRIMITIVE STk_copy_tree(SCM l)
{
  return CONSP(l) ? STk_cons(STk_copy_tree(CAR(l)), STk_copy_tree(CDR(l))): l;
}
