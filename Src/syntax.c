/*
 *
 * s y n t a x . c			-- Syntaxic forms implementation
 *
 * Copyright © 1993-1998 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *    Creation date: 25-Oct-1993 23:39
 * Last file update: 14-May-1998 22:55
 */

/* Notes:
 * ------ 
 * C functions syntax_xxx implement the scheme syntax xxx. A syntax function 
 * returns its work in its first argument (SCM *pform). The function result
 * is a boolean. If false, it indicates to eval that this result is 
 * a final one (eval can return it unmodified). Otherwise, the eval function 
 * take the result stored in *pform and evaluates it again (in the same eval 
 * frame). This mechanaism permits to treat tail recursive calls as jump in
 * the eval function.
 *
 * Syntax function which returns EVALCAR(zzzz) are not tail recursive in debug 
 * mode (in normal mode only the first call is non tail recursive, since this 
 * first call will replace the original code by an equivalent code which is 
 * clean on tail recursive calls.
 *
 */

#include "stk.h"
#include "module.h"

#ifdef COMPACT_SMALL_CST
#   define makecell(type) ((SCM) MAKE_SMALL_CST(0, type))
#else
static SCM makecell(int type)
{
  register SCM z;
  NEWCELL(z, type);
  return z;
}
#endif


static SCM define2lambda(SCM l, int len)
{
  /* transform (define (f p) b) in (define f (lambda (p) b)) */
  if (CONSP(l) && CONSP(CAR(l))){
    if (len < 2) goto Error;
    return Cons(CAR(CAR(l)), Cons(Cons(Sym_lambda,
				       Cons(CDR(CAR(l)), CDR(l))),
				  NIL));
  }
  else 
    if (len == 2) return l;
Error:
  Err("define: bad definition", l);
}

PRIMITIVE STk_syntax_quote(SCM *pform, SCM env, int len)
{
  SCM args = CDR(*pform);

  if (len != 1) Err("quote: Bad syntax", *pform);
  if (ModifyCode()) CAR(*pform) = makecell(tc_quote);
  SYNTAX_RETURN(CAR(args), Ntruth);
}

PRIMITIVE STk_syntax_lambda(SCM *pform, SCM env, int len)
{
  register SCM args=CDR(*pform);

  if (len < 2) Err("lambda: bad syntax", *pform);

  if(ModifyCode()) CAR(*pform) = makecell(tc_lambda);

  SYNTAX_RETURN(STk_makeclosure(args, env), Ntruth);
}


PRIMITIVE STk_syntax_if(SCM *pform, SCM env, int len)
{
  SCM args = CDR(*pform);
  
  switch (len) {
    case 2:  args = LIST3(CAR(args), CAR(CDR(args)), UNDEFINED);
    case 3:  break;
    default: Err("if: bad syntax", *pform);
  }

  if (ModifyCode()) {
    CAR(*pform) = makecell(tc_if);
    CDR(*pform) = args; /* will always contain a else part */
  }

  SYNTAX_RETURN(NEQ(EVALCAR(args), Ntruth) ? CAR(CDR(args)):CAR(CDR(CDR(args))), 
		Truth);
}

PRIMITIVE STk_syntax_setq(SCM *pform, SCM env, int len)
{
  SCM var, *tmp, args = CDR(*pform);

  if (len != 2)  	       Err("set!: bad assignement", args);
  if (NSYMBOLP(var=CAR(args))) Err("set!: first argument is not a symbol", var);

  tmp = STk_varlookup(var, env, 0);
  if (*tmp == UNBOUND) {
    /* C variables are always seen as unbound variables. This tends to 
     * make them slower than standard variables but, in counterpart, this
     * doesn't slow down accesses to Scheme variables 
     */
    if (var->cell_info & CELL_INFO_C_VAR) {
      /* This is not an unbound variable but rather a C variable */
      STk_apply_setter_C_variable(PNAME(var), EVALCAR(CDR(args)));
      goto Out;
    }
    else
      Err("set!: variable not defined", var);
  }
  if (ModifyCode()) CAR(*pform) = makecell(tc_setq);
  *tmp = EVALCAR(CDR(args));

Out:
  if (TRACED_VARP(var)) STk_change_value(var, env);

  SYNTAX_RETURN(UNDEFINED, Ntruth);
}

PRIMITIVE STk_syntax_cond(SCM *pform, SCM env, int len) /* len unused here */
{
  SCM l, tmp;
  
  for (l=CDR(*pform); CONSP(l); l=CDR(l)) {
    if (NCONSP(CAR(l))) Err("cond: malformed clause", CAR(l));
    if (EQ(CAR(CAR(l)), Sym_else) && NNULLP(CDR(l)))
      Err("cond: else clause must be the last", *pform);
  }
  if (NNULLP(l)) Err("cond: malformed clauses list", CDR(*pform));

  tmp = Cons(makecell(tc_cond), CDR(*pform));
  if (ModifyCode()) CAR(*pform) = CAR(tmp);

  SYNTAX_RETURN(tmp, Truth);
}

PRIMITIVE STk_syntax_and(SCM *pform, SCM env, int len)
{
  SCM l   = CDR(*pform);

  if (ModifyCode()) CAR(*pform) = makecell(tc_and);

  if (len == 0) SYNTAX_RETURN(Truth, Ntruth);

  for (--len; len; len--, l=CDR(l)) {
    if (EVALCAR(l) == Ntruth) SYNTAX_RETURN(Ntruth, Ntruth);
  }
  SYNTAX_RETURN(CAR(l), Truth);
}

PRIMITIVE STk_syntax_or(SCM *pform, SCM env, int len)
{
  SCM l   = CDR(*pform);
  SCM val;

  if (ModifyCode()) CAR(*pform) = makecell(tc_or);

  if (len == 0) SYNTAX_RETURN(Ntruth, Ntruth);

  for (--len; len; len--, l=CDR(l)) {
    if ((val=EVALCAR(l)) != Ntruth) SYNTAX_RETURN(val, Ntruth);
  }
  SYNTAX_RETURN(CAR(l), Truth);
}

static SCM syntax_let_family(SCM *pform, SCM env, char *who, int type, int len)
{
  register SCM p, tmp, fl=NIL, al=NIL;
  char buffer[100];
  int named_let = 0;

  if (len < 2) goto Error;

  p = CAR(CDR(*pform));
  if (SYMBOLP(p) && type == tc_let) {
    /* It's a named let. Re-initialize to a correct value */ 
    if (len < 3) goto Error;
    named_let = 1;
    p = CAR(CDR(CDR(*pform)));
  }

  for(  ; NNULLP(p); p=CDR(p)) {
    if (STk_llength(tmp=CAR(p)) != 2 || NSYMBOLP(CAR(tmp))) {
      sprintf(buffer, "%s: badly formed binding", who);
      Err(buffer, CONSP(p)? tmp: p);
    }
    /* Verify that this binding doesn't already exist in fl 
     * except for let* which aloows it (at least the formal semantics 
     * given in R4RS). Problem shown by Brent Knight <knight@CS.Cornell.EDU>
     */
    if (type!=tc_letstar && STk_memv(CAR(tmp),fl)!=Ntruth) {
      sprintf(buffer, "%s: duplicate binding", who);
      Err(buffer, CAR(CDR(*pform)));
    }
    fl = Cons(CAR(tmp),fl); 
    al = Cons(CAR(CDR(tmp)),al);
  }

  tmp = named_let ?
    	   /* named let */
    	   LIST4(makecell(tc_letrec), 
		 LIST1(CAR(CDR(*pform))),
		 LIST1(Cons(Sym_lambda, 
			    Cons(Reverse(fl), CDR(CDR(CDR(*pform)))))),
		 Cons(CAR(CDR(*pform)), Reverse(al))) :
	   /* normal case */
	   Cons(makecell(type), 
		Cons(Reverse(fl), 
		     Cons(Reverse(al), 
			  CDR(CDR(*pform)))));

  if (ModifyCode()) {
    CAR(*pform) = CAR(tmp); 
    CDR(*pform) = CDR(tmp); 
  }
  SYNTAX_RETURN(tmp, Truth);

Error:
  sprintf(buffer, "%s: incorrect number of subforms", who);
  Err(buffer, *pform);
}


PRIMITIVE STk_syntax_let(SCM *pform, SCM env, int len)
{
  return syntax_let_family(pform, env, "let", tc_let, len);
}

PRIMITIVE STk_syntax_letstar(SCM *pform, SCM env, int len)
{
  return syntax_let_family(pform, env, "let*", tc_letstar, len);
}

PRIMITIVE STk_syntax_letrec(SCM *pform, SCM env, int len)
{
  return syntax_let_family(pform, env, "letrec", tc_letrec, len);
}

PRIMITIVE STk_syntax_begin(SCM *pform, SCM env, int len)
{
  register SCM l = CDR(*pform);

  if (len == 0) Err("begin: no subform in sequence", l);
  for ( ; NNULLP(CDR(l));  l = CDR(l))
    EVALCAR(l);
  if (ModifyCode()) CAR(*pform) = makecell(tc_begin);
  SYNTAX_RETURN(CAR(l), Truth);;
}

PRIMITIVE STk_syntax_delay(SCM *pform, SCM env, int len)
{
  SCM z, tmp;

  if (len != 1) Err("delay: Bad expression", *pform);

  /* Build (lambda () expr) in tmp */
  tmp = STk_makeclosure(Cons(NIL, CDR(*pform)), env);

  /* save this closure in the promise */
  NEWCELL(z, tc_promise);
  z->storage_as.promise.expr = tmp;
  z->storage_as.promise.resultknown = 0;
  SYNTAX_RETURN(z, Ntruth);
}

static SCM backquotify(SCM x, SCM env, int level)
{
  if (VECTORP(x)) 
    return STk_list2vector(backquotify(STk_vector2list(x), env, level));

  if (NCONSP(x)) return x;

  if (EQ(CAR(x), Sym_quasiquote))
    return LIST2(Sym_quasiquote,
		 backquotify(CAR(CDR(x)), env, level+1));

  if (EQ(CAR(x), Sym_unquote))
    return (level == 1) ? EVALCAR(CDR(x))
      			: LIST2(Sym_unquote,
				backquotify(CAR(CDR(x)), env, level-1));

  if (CONSP(CAR(x)) && EQ(CAR(CAR(x)), Sym_unq_splicing))
    return NULLP(CDR(x)) ? EVALCAR(CDR(CAR(x)))
	    		 : STk_append2(EVALCAR(CDR(CAR(x))),
				       backquotify(CDR(x), env, level));
  /* Otherwise */
  return Cons(backquotify(CAR(x), env, level), backquotify(CDR(x), env, level));
}


PRIMITIVE STk_syntax_quasiquote(SCM *pform, SCM env, int len)
{
  if (len == 0) Err("quasiquote: no form", NIL);
  SYNTAX_RETURN(backquotify(CAR(CDR(*pform)), env, 1), Ntruth);
}


PRIMITIVE STk_syntax_define(SCM *pform, SCM env, int len)
{
  SCM *tmp, var, args, expr;

  args = define2lambda(CDR(*pform), len);
  var  = CAR(args); if (NSYMBOLP(var)) Err("define: bad variable name", var);
  expr = EVALCAR(CDR(args)); 

  if (NULLP(env)) {				/* Global var */
    STk_define_public_var(NIL, var, expr);
    STk_last_defined = var;   
  }
  else {
    if (MODULEP(CAR(env))) {			/* Public variable */
      STk_define_public_var(CAR(env), var, expr);
      STk_last_defined = var;
    }
    else {				 	/* Local var */
      tmp = STk_value_in_env(var, env);
      if (tmp != &UNBOUND) {
	/* This symbol was already defined at current level. Just do an assignment*/
	*tmp = expr;
      }
      else {
	/* Extend current environment for that definition */
	SCM vars, vals, tmp = CAR(env);
	
	/* We add new definition at the end of current environment (since
	 * some code has possbly be re-written using tc_localvar). This avoid
	 * re-numbering  the code.
	 *
	 * Note: The first append cannot be destructive because vars are direct
	 * references on the procedure environmment. 
	 */
	vars = STk_append2(CAR(tmp), LIST1(var));
	vals = STk_dappend2(CDR(tmp), LIST1(expr));
	CAR(tmp) = vars;
	CDR(tmp) = vals;
      }
    }
  }
  if (TRACED_VARP(var)) STk_change_value(var, env);
  SYNTAX_RETURN(UNDEFINED, Ntruth);
}

/*
 *
 * STk special syntaxic forms
 *
 */

PRIMITIVE STk_syntax_extend_env(SCM *pform, SCM env, int len)
{
  if (len < 2) Err("extend-environement: Bad syntax", *pform);
  if (ModifyCode()) CAR(*pform) = makecell(tc_extend_env);
  SYNTAX_RETURN(*pform, Truth);
}

PRIMITIVE STk_while(SCM l, SCM env, int len)
{
  register SCM test, body;

  if (len < 2) Err("while: bad argument list", NIL);
  
  for (test = EVALCAR(l); test != Ntruth; test = EVAL(CAR(l))) {
    /* Evaluate each expression of the body */
    for (body = CDR(l); !NULLP(body); body = CDR(body))
      EVALCAR(body);
  }
  return UNDEFINED;
}

PRIMITIVE STk_until(SCM l, SCM env, int len)
{
  register SCM test, body;

  if (len < 2) Err("until: bad argument list", NIL);
  
  for (test = EVALCAR(l); test == Ntruth; test = EVAL(CAR(l))) {
    /* Evaluate each expression of the body */
    for (body = CDR(l); !NULLP(body); body = CDR(body))
      EVALCAR(body);
  }
  return UNDEFINED;
}
