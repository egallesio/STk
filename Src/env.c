/*
 *
 * e n v . c			-- Environment management
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
 *    Creation date: 23-Oct-1993 21:37
 * Last file update: 23-Jul-1996 15:44
 */

#include "stk.h"
#include "extend.h"

static void makelocalvar(SCM x, int level, int position)
{
  if (ModifyCode() && CONSP(x)) { /* Replace (CAR x) by a localvar access */
    SCM z;
    NEWCELL(z, tc_localvar);
    z->storage_as.localvar.position = position;
    z->storage_as.localvar.level    = level;
    z->storage_as.localvar.symbol   = CAR(x);
    CAR(x) = z;
  }
}

static void makeglobalvar(SCM x)
{
  if (ModifyCode() && CONSP(x)) { /* Replace (CAR x) by a globalvar access */
    SCM z;
    NEWCELL(z, tc_globalvar);
    VCELL(z) = CAR(x);
    CAR(x) = z;
  }
}

SCM STk_makeenv(SCM l, int create_if_null)
{
  SCM z;
  
  if (NULLP(l) && !create_if_null) return STk_globenv;
  
  NEWCELL(z, tc_env);
  z->storage_as.env.data = l;
  return z;
}

/* Return the value of var in given env. Search is done only at out most level */
SCM *STk_value_in_env(SCM var, SCM env)
{
  SCM fl, *al;
  
  al = &CAR(env);
  for (fl=CAR(CAR(env)); NNULLP(fl); fl=CDR(fl)) {
    if (NCONSP(fl)) {
      if (EQ(fl, var)) return &CDR(*al);
      else break;
    }
    al = &CDR(*al);
    if EQ(CAR(fl), var) return &CAR(*al);
  }
  /* Not found */
  return &UNBOUND;
}

SCM *STk_varlookup(SCM x, SCM env, int err_if_unbound)
{
  SCM frame, fl, *al, var;
  int level, pos;

  var = CONSP(x)? CAR(x) : x;

  /* Try to find var in env */ 
  for(level=0, frame=env; CONSP(frame); frame=CDR(frame), level++) {
    al = &CAR(frame);

    for (pos=0, fl=CAR(CAR(frame)); NNULLP(fl); fl=CDR(fl), pos++) {
      if (NCONSP(fl)) {
	if (EQ(fl, var)) { makelocalvar(x, level, pos); return &CDR(*al); }
	else break;
      }
      al = &CDR(*al);
      if EQ(CAR(fl), var) { makelocalvar(x, level, pos); return &CAR(*al); }
    }
  }
  /* Not found. Return it's value in global environment */
  if (err_if_unbound) {
    SCM val = VCELL(var);

    if (val == UNBOUND) {
      /* C variables are always seen as unbound variables. This tends to 
       * make them slower than standard variables but, in counterpart, this
       * doesn't slow down acceses to Scheme variable 
       */
      if (var->cell_info & CELL_INFO_C_VAR) {
	/* This is not an unbound variable but rather a C variable */
	static SCM tmp;
	tmp = STk_apply_getter_C_variable(PNAME(var));
	return &tmp;
      }
      Err("unbound variable", var);
    }
    if (TYPEP(val, tc_autoload)) STk_do_autoload(var);
  }
  makeglobalvar(x);
  return &VCELL(var);
}

SCM STk_localvalue(SCM var, SCM env)
{
  register SCM p, q;
  register int i;

  p = env;
  /* Go down ``level'' environments */
  for (i = var->storage_as.localvar.level; i; i--)
    p = CDR(p);
  
  /* Go forward ``position'' variables */
  q = CAR(CAR(p)); p = CDR(CAR(p)); 
  for (i = var->storage_as.localvar.position; i; i--) {
    p = CDR(p);
    q = CDR(q);
  }
  return CONSP(q) ? CAR(p) : p;
}


SCM STk_extend_env(SCM formals, SCM actuals, SCM env, SCM who)
{
  register SCM f = formals, a = actuals;

  for ( ; NNULLP(f); f=CDR(f), a=CDR(a)) {
    if (NCONSP(f)) goto Out;
    if (NULLP(a)) Err("too few arguments to", who);
  }
  if (NNULLP(a)) Err("too many arguments to", who);
 Out:
  return STk_fast_extend_env(formals, actuals, env);
}



PRIMITIVE STk_symbol_boundp(SCM x, SCM env)
{
  SCM tmp;

  if (NSYMBOLP(x)) Err("symbol-bound?: not a symbol", x);
  if (env == UNBOUND) env = STk_globenv;
  else 
    if (NENVP(env)) Err("symbol-bound?: bad environment", env);

  tmp = *STk_varlookup(x, env->storage_as.env.data, FALSE);
  return (tmp == UNBOUND) ? Ntruth : Truth;
}

PRIMITIVE STk_the_environment(SCM args, SCM env, int len)
{
  if (len) Err("the-environement: Too much parameters", args);
  return STk_makeenv(env, 0);
}

PRIMITIVE STk_parent_environment(SCM env)
{
  if (NENVP(env)) Err("parent->environment: bad environment", env);

  return (env==STk_globenv) ? Ntruth: STk_makeenv(CDR(env->storage_as.env.data),0);
}

PRIMITIVE STk_global_environment(void)
{
  return STk_globenv;
}

static SCM local_env2list(SCM l)
{
  register SCM res=NIL, l1, l2;

  for (l1=CAR(l), l2=CDR(l); NNULLP(l1); l1=CDR(l1), l2=CDR(l2))
    if (CONSP(l1))
      res = Cons(Cons(CAR(l1), CAR(l2)), res);
    else
      /* We had a dotted list */
      return  Cons(Cons(l1, l2), res);
  return res;
}


PRIMITIVE STk_environment2list(SCM env)
{
  SCM l, res = NIL;

  if (NENVP(env)) Err("environment->list: bad environment", env);

  for (l=env->storage_as.env.data; NNULLP(l); l=CDR(l))
    res = Cons(local_env2list(CAR(l)), res);
  
  res = Cons(STk_global_env2list(), res);
  return Reverse(res);
}


PRIMITIVE STk_environmentp(SCM obj)
{
  return ENVP(obj)? Truth: Ntruth;
}
