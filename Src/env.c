/*
 *
 * e n v . c			-- Environment management
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
 *    Creation date: 23-Oct-1993 21:37
 * Last file update:  3-Sep-1999 20:19 (eg)
 */

#include "stk.h"
#include "extend.h"
#include "module.h"

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

static void makemodulevar(SCM x, SCM var, SCM module, SCM pvalue)
{
  return;
  if (ModifyCode() && CONSP(x)) { /* Replace (CAR x) by a modulevar access */
    SCM z;
    NEWCELL(z, tc_modulevar);
    CAR(z) = Cons(var, module);
    CDR(z) = pvalue;
    
    CAR(x) = z;
  }
}


SCM STk_makeframe(SCM formals, SCM actuals)
{
  register SCM z;

  NEWCELL(z, tc_frame);
  CAR(z) = formals;
  CDR(z) = actuals;

  return z;
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




static SCM *module_lookup(SCM module, SCM var, SCM context, int err_if_unbound)
{
  register SCM m;
  Tcl_HashEntry *entry;
  SCM *avalue;

  /* If module is a real module (i.e. not a fake one), we have to try 
   * to find var in the current module and then in the exported variables
   * of imported modules.
   * Fake modules are created by parent-environment: when asking the
   * parent environment of a module M importing M1 and M2, we must create
   * a fake module similar to M1 importing solely M2. In this case,
   * the search is done in the exported variables of M only, and then in 
   * the exported variables of imported modules.
   * All this complicated stuff to allow things like:
   *     (eval 'a (parent-environment (module-environment M)))
   */
  if (NULLP(module) || EQ(module, STk_global_module)) {
    if (VCELL(var) == UNBOUND)
      m = MOD_IMPORTED(STk_global_module);
    else 
      goto Global;
  }
  else {
    if (FAKE_MODULEP(module))
      m = Cons(module, MOD_IMPORTED(module));
    else {
      /* Search variable in the current module */
      if ((entry = Tcl_FindHashEntry(MOD_TABLE(module), (char*) var))) {
	avalue = (SCM*) &Tcl_GetHashValue(entry);
	if (*avalue != UNBOUND) {
	  makemodulevar(context, var, module, (SCM) entry);
	  if (err_if_unbound && TYPEP(*avalue, tc_autoload)) 
	    STk_do_autoload(var, *avalue);
	  return avalue;
	}
      }
      m = MOD_IMPORTED(module);
    }
  }

  /* Variable not found in the given module (which could be the global one)
   * Try to find it in exported variables of the "m" list of modules 
   */
  for (  ; NNULLP(m); m = CDR(m)) {
    module = CAR(m);
    if ((entry = Tcl_FindHashEntry(FIND_TABLE(module), (char*) var))) {
      if (MOD_EXPORTALL(module) || STk_memq(var, MOD_EXPORTS(module)) != Ntruth) {
	avalue = (SCM*) &Tcl_GetHashValue(entry);
	if (*avalue != UNBOUND) {
	  makemodulevar(context, var, module, (SCM) entry);
	  if (TYPEP(*avalue, tc_autoload)) STk_do_autoload(var, *avalue);
	  return avalue;
	}
      }
    }
  }

Global:
  /* "True" global variable (i.e. one which is accessed from its symbol) */
  {
    SCM val = VCELL(var);
    
    if (err_if_unbound) {
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
      if (TYPEP(val, tc_autoload)) STk_do_autoload(var, val);
    }
    makeglobalvar(context);
    return &VCELL(var);
  }
}


SCM *STk_varlookup(SCM x, SCM env, int err_if_unbound)
{
  SCM frame, fl, *al, var;
  int level, pos;

  var   = CONSP(x)? CAR(x) : x;
  frame = env;
  
  /* Try to find var in env */ 
  for(level=0; CONSP(frame); frame=CDR(frame), level++) {
    al = &CAR(frame);
    if (MODULEP(*al))
      return module_lookup(*al, var, x, err_if_unbound);
    
    for (pos=0, fl=CAR(CAR(frame)); NNULLP(fl); fl=CDR(fl), pos++) {
      if (NCONSP(fl)) {
	if (EQ(fl, var)) { makelocalvar(x, level, pos); return &CDR(*al); }
	else break;
      }
      al = &CDR(*al);
      if EQ(CAR(fl), var) { makelocalvar(x, level, pos); return &CAR(*al); }
    }
  }

  return module_lookup(frame, var, x, err_if_unbound);
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
  if (len) Err("the-environement: too many parameters", args);
  return STk_makeenv(env, 0);
}

PRIMITIVE STk_parent_environment(SCM env)
{
  SCM tmp, parent;

  if (NENVP(env)) Err("parent->environment: bad environment", env);
  if (env==STk_globenv) return Ntruth;

  tmp = env->storage_as.env.data;
  
  if (MODULEP(CAR(tmp))) {
    parent = STk_make_parent_module(CAR(tmp));
    if (MODULEP(parent)) return STk_makeenv(LIST1(parent), 0);
  }
  return STk_makeenv(CDR(tmp),0);
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

  for (l=env->storage_as.env.data; NNULLP(l); l=CDR(l)) {
    if (MODULEP(CAR(l))) {
      res = STk_append2(STk_module_env2list(CAR(l)), res);
      break;
    }
    res = Cons(local_env2list(CAR(l)), res);
  }

  res = Cons(STk_global_env2list(), res);
  return Reverse(res);
}


PRIMITIVE STk_environmentp(SCM obj)
{
  return ENVP(obj)? Truth: Ntruth;
}

#ifdef DEBUG_STK
PRIMITIVE STk_get_environment(SCM env)
{
  if (NENVP(env)) Err("%get-environment: bad environment", env);
  return env->storage_as.env.data;
}
#endif
