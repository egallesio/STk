/*
 *
 * e v a l . c				-- The evaluator
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
 * Last file update:  2-Sep-1996 19:18
 */

#include "stk.h"
#include "extend.h"
#ifdef USE_STKLOS
#  include "stklos.h"
#endif

#define RETURN(x)		{tmp = (x); goto Out; }

/*
 * STk_eval_flag indicates if eval has something to test (a ^C has
 * occured, a eval-hook to apply, ...). Using this flag allow to
 * minimize the number of tests done in eval the eval function (which
 * must be as fast as possible since we spent most of our time in it).
 * When this flag is up, the set of possible "diverting" things are 
 * tested.
 * This flag could also probably used for implementing a thread mechanism.
 *
 */

int STk_eval_flag = 0;

/*
 * Eval stack
 * 
 * The eval stack is a stack of the arguments passed to eval. This stack permits
 * to facilitate debugging  of Scheme programs. Its contents is displayed 
 * when an error occurs.
 * Note that "STk_eval_stack" does'nt need to be protected since it contains 
 * pointers which are themselves copies of the eval C routine. Eval parameters
 * will be marked as are all the objects which are in the C stack
 * */

static struct Stack_info {
  SCM expr, env;
  struct Stack_info *previous;
} *stack = NULL;


void STk_show_eval_stack(int depth)
{
  int j;
  struct Stack_info *p;

  fprintf(STk_stderr, "\nCurrent eval stack:\n__________________\n");
  for (p=stack, j=0; p && j<=depth ; p=p->previous, j++) {
    fprintf(STk_stderr, "%3d    ", j);
    STk_print(STk_uncode(p->expr), STk_curr_eport, WRT_MODE);
    Putc('\n', STk_stderr);
    if (j == depth && p->previous) fprintf(STk_stderr, "...\n");
  }
}

void STk_reset_eval_stack(void)
{
  stack = NULL;
}

PRIMITIVE STk_get_eval_stack(void)
{
  struct Stack_info *p;
  SCM 		     z = NIL;
  
  for (p = stack; p ; p = p->previous) z = Cons(p->expr, z);
  
  return STk_reverse(z);
}

PRIMITIVE STk_get_env_stack(void)
{
  struct Stack_info *p;
  SCM 		     z = NIL;
  
  for (p = stack; p ; p = p->previous)  {
    /* Avoid to create an environment for each item */
    SCM tmp = (z!=NIL && STk_equal(CAR(z)->storage_as.env.data,p->env)==Truth) ?
      		CAR(z): 
      		STk_makeenv(p->env, 0);
    z = Cons(tmp, z);
  }
  return  STk_reverse(z);
}


SCM STk_top_env_stack(void)
{
  return stack ? STk_makeenv(stack->env, 0): STk_globenv;
}

static SCM eval_args(SCM l, SCM env)
{
  SCM result,v1,v2;

  if (NULLP(l)) return NIL;

  v1 = result = Cons(EVALCAR(l), NIL);
  
  for(v2=CDR(l); CONSP(v2); v2=CDR(v2)) {
    v1 = CDR(v1) = Cons(EVALCAR(v2),NIL);
  }
  return result;
}

static SCM eval_cond(SCM *pform, SCM env)
{
  SCM l, clause, tmp, res = Truth;

  for (l=*pform; NNULLP(l); l = CDR(l)) {
    clause = CAR(l);
    /* We are sure that clause is a cons here (see syntax_cond) */
    if (EQ(CAR(clause), Sym_else) || (res=EVALCAR(clause)) != Ntruth) {
      tmp = CDR(clause);
      if (NULLP(tmp))  SYNTAX_RETURN(res, Ntruth);
      if (NCONSP(tmp)) goto Error;

      if (EQ(CAR(tmp), Sym_imply)) {
	/* Clause is ((condition) => function) */
	if (STk_llength(tmp) != 2) Err("cond: malformed `=>'", tmp);
	SYNTAX_RETURN(Apply(EVALCAR(CDR(tmp)), LIST1(res)), Ntruth);
      }
      else {
	for( ; NNULLP(CDR(tmp)); tmp=CDR(tmp))
	  EVALCAR(tmp);
	SYNTAX_RETURN(CAR(tmp), Truth);
      }
    }
  }
  SYNTAX_RETURN(UNDEFINED, Ntruth);
Error:
  Err("cond: bad clause body", clause);
  return UNDEFINED; /* never reached */
}

/*
 * *eval-hook* management.
 *
 * STk eval-hook mechanism is similar to the CL one. The *eval-hook*
 * Scheme variable is managed as a C-variable which has a getter and a
 * setter function associated to it. The functions below allow to
 * manage a stack of hooks in the C stack. Each hook info is stored in
 * a Eval_hook_info structure
 * 
 */

struct Eval_hook_info {
  SCM hook;
  int bypass_check;
  struct Eval_hook_info *previous;
};

static struct Eval_hook_info  eval_hook_bottom;
static struct Eval_hook_info *eval_hook_stack = &eval_hook_bottom;


static SCM get_eval_hook(char *s)
{
  return eval_hook_stack->hook;
}

static void set_eval_hook(char *unused, SCM value)
{
  if (value == Ntruth) {
    eval_hook_stack	   = &eval_hook_bottom;
    eval_hook_stack->hook  = Ntruth;
  }
  else {
    if (STk_procedurep(value) == Ntruth) 
      STk_err("Hook value must be #f or a procedure. It is", value);

    eval_hook_stack->hook  	  = value;
    eval_hook_stack->bypass_check = FALSE;
    SET_EVAL_FLAG(1);
  }
}

static SCM handle_eval_hook(SCM x, SCM env)
{
  struct Eval_hook_info info;
  SCM res;
  
  /* Reset eval-hook to avoid recursive application */
  info.previous     = eval_hook_stack;
  info.hook         = Ntruth;
  info.bypass_check = FALSE;
  eval_hook_stack   = &info;

  /* Call user code */
  res = STk_apply(info.previous->hook, LIST2(x, STk_makeenv(env, 0)));
  
  /* If we are here, everything was correct */
  eval_hook_stack = info.previous;
  SET_EVAL_FLAG(1);
  return res;
}


void STk_reset_eval_hook(void)
{
  eval_hook_stack		= &eval_hook_bottom;
  eval_hook_stack->hook     	= Ntruth;
  eval_hook_stack->bypass_check = FALSE;
  eval_hook_stack->previous 	= &eval_hook_bottom; /* itself */
}


void STk_init_eval_hook(void)
{
  STk_define_C_variable(EVAL_HOOK, get_eval_hook, set_eval_hook);
  STk_gc_protect(&eval_hook_bottom.hook);
  STk_reset_eval_hook();
}


PRIMITIVE STk_eval_hook(SCM x, SCM env, SCM hook)
{
  SCM res;
  struct Eval_hook_info info;

  info.hook         = hook;
  info.bypass_check = TRUE;
  info.previous     = eval_hook_stack;
  eval_hook_stack   = &info;
  SET_EVAL_FLAG(1);

  res = STk_eval(x, env->storage_as.env.data);
  eval_hook_stack = info.previous;
  SET_EVAL_FLAG(1);

  return res;
}

/*
 *
 * E V A L
 * 
 */

SCM STk_eval(SCM x, SCM env)
{
  register SCM tmp, fct;
  register int len;
  struct Stack_info infos;

  infos.previous = stack; stack = &infos;
Top:
  infos.expr = x; infos.env = env; 
  if (STk_eval_flag) {
    /* We have something to test before evaluating the form:
     *		- a ^C ?
     *		- *eval-hook*?
     */
    
    if (STk_control_C && !STk_sigint_counter) STk_handle_signal(SIGINT);
    
    if (eval_hook_stack->hook != Ntruth) {
      if (eval_hook_stack->bypass_check) {
	eval_hook_stack->bypass_check = FALSE;
	if (CONSP(x)) {
	  fct = EVAL(CAR(x)); /* Don't use EVALCAR here of course */
	  goto Apply_args;
	}
      }
      else
	RETURN(handle_eval_hook(x, env));
    }
    SET_EVAL_FLAG(STk_control_C || 
		  (eval_hook_stack->hook != Ntruth));
  }

  switch TYPE(x) {
    case tc_symbol:
         RETURN(*STk_varlookup(x, env, 1));
    case tc_globalvar:
	 RETURN(VCELL(VCELL(x)));
    case tc_localvar:
	 RETURN(STk_localvalue(x, env));
    case tc_cons: {
 	 /* Evaluate the first argument of this list (without calling eval) */
	 tmp = CAR(x);
	 switch TYPE(tmp) {
           case tc_symbol:
	        fct=*STk_varlookup(x, env, 1);
		break;
	   case tc_cons:
		fct = EVAL(tmp); break;
	   case tc_globalvar:
		fct = VCELL(VCELL(tmp)); break;
	   case tc_localvar:
		fct = STk_localvalue(tmp, env); break;
           default:
		fct = tmp;
  	 }

         Apply_args:
	 /* Find length of the parameter list */
	 for (len=0, tmp=CDR(x); NNULLP(tmp); len++, tmp=CDR(tmp))
	   if (NCONSP(tmp)) Err("eval: malformed list", x);

	 /* apply parameters to fct */
	 tmp = CDR(x);
	 switch (TYPE(fct)) {
	   case tc_subr_0:
	        if (len == 0) RETURN(SUBR0(fct)());
		goto Error;
	   case tc_subr_1:
		if (len == 1) RETURN(SUBRF(fct)(EVALCAR(tmp)));
		goto Error;
	   case tc_subr_2:
		if (len == 2) RETURN(SUBRF(fct)(EVALCAR(tmp), 
						EVALCAR(CDR(tmp))));
		goto Error;
	   case tc_subr_3:
		if (len == 3) RETURN(SUBRF(fct)(EVALCAR(tmp),
						EVALCAR(CDR(tmp)),
						EVALCAR(CDR(CDR(tmp)))));
		goto Error;
	   case tc_subr_0_or_1:
		switch (len) {
		  case 0: RETURN(SUBRF(fct)(UNBOUND));
		  case 1: RETURN(SUBRF(fct)(EVALCAR(tmp)));
		  default: goto Error;
		}
	   case tc_subr_1_or_2:
		switch (len) {
		  case 1: RETURN(SUBRF(fct)(EVALCAR(tmp), UNBOUND));
		  case 2: RETURN(SUBRF(fct)(EVALCAR(tmp), 
					    EVALCAR(CDR(tmp))));
		  default: goto Error;
		}
	   case tc_subr_2_or_3:
		switch (len) {
		  case 2: RETURN(SUBRF(fct)(EVALCAR(tmp), 
					    EVALCAR(CDR(tmp)),
					    UNBOUND));
		  case 3: RETURN(SUBRF(fct)(EVALCAR(tmp), 
					    EVALCAR(CDR(tmp)),
					    EVALCAR(CDR(CDR(tmp)))));
		  default: goto Error;
		}
	   case tc_ssubr:
		RETURN(SUBRF(fct)(tmp, env, TRUE));
	   case tc_fsubr:
		RETURN(SUBRF(fct)(tmp, env, len));
	   case tc_syntax:
		if (SUBRF(fct)(&x, env, len) == Truth) goto Top;
		RETURN(x);
	   case tc_lsubr:
		RETURN(SUBRF(fct)(eval_args(tmp, env), len));
#ifdef USE_STKLOS
	   case tc_instance:
	        tmp = eval_args(tmp, env);

	        if (PUREGENERICP(fct)) {
		  /* Do it in C */
		  if (NULLP(THE_SLOT_OF(fct, S_methods)))
		    Apply(VCELL(Intern("no-method")), LIST2(fct, tmp));

		  fct = STk_compute_applicable_methods(fct, tmp, len, FALSE);
		  /* fct is the list of applicable methods. Apply the
		   * first one with the tail of the list as first
		   * parameter (next-method). If fct is NIL, that's because
		   * the no-applicable-method triggered didn't call error.
		   */
		  if (NULLP(fct)) RETURN(UNDEFINED);
		  tmp = Cons(STk_make_next_method(CDR(fct), tmp), tmp);
		  fct = THE_SLOT_OF(CAR(fct), S_procedure);
		  env = STk_extend_env(CLOSURE_PARAMETERS(fct),
				       tmp,
				       fct->storage_as.closure.env,
				       x);
		  tmp = CDR(fct->storage_as.closure.code);
		  goto Begin;
		}
		else
		  /* Do it in Scheme */
		  RETURN(STk_apply_user_generic(fct, tmp));

	   case tc_next_method:
	        /* By nature, next methods cannot be recursive; so, we can 
		 * call the apply-next-method function */
	        RETURN(STk_apply_next_method(fct, eval_args(tmp, env)));
#endif		       
#ifdef USE_TK
	  case tc_tkcommand:
	    	RETURN(STk_execute_Tcl_lib_cmd(fct, tmp, env, 1));
#endif
	   case tc_closure:
		env = STk_extend_env(CLOSURE_PARAMETERS(fct),
				     eval_args(tmp, env),
				     fct->storage_as.closure.env,
				     x);
		tmp = CDR(fct->storage_as.closure.code);
		/* NOBREAK */
Begin:	   case tc_begin:
		for( ; NNULLP(CDR(tmp)); tmp=CDR(tmp))
		  EVALCAR(tmp);
		x = CAR(tmp);
	        goto Top;
	   case tc_cont:
		if (len == 1) STk_throw(fct, EVALCAR(tmp));
		goto Error;
	   case tc_let:
		env = STk_fast_extend_env(CAR(tmp), 
					  eval_args(CAR(CDR(tmp)),env), 
					  env);
		tmp = CDR(CDR(tmp));
		goto Begin;
	   case tc_letstar:
		{
		  SCM l1=CAR(tmp), l2=CAR(CDR(tmp));
		  /* Create a rib to avoid that internal def be seen as global  */
		  env = STk_fast_extend_env(NIL, NIL, env); 
		  for ( ; NNULLP(l1); l1=CDR(l1), l2=CDR(l2))
		    env = STk_fast_extend_env(Cons(CAR(l1), NIL), 
					      Cons(EVALCAR(l2), NIL), env);
		  tmp =  CDR(CDR(tmp));
		  goto Begin;
		}
	   case tc_letrec:
		{
		  SCM bindings = NIL, l1=CAR(tmp), l2=CAR(CDR(tmp));
		  
		  /* Make a binding list an extend current with it */
		  for (len=STk_llength(l1); len; len--) 
		    bindings=Cons(UNBOUND,bindings);
		  env = STk_fast_extend_env(l1, bindings, env);

		  /* Eval init forms in the new environment */
		  for (l1 = CAR(tmp); NNULLP(l1); l1=CDR(l1), l2=CDR(l2))
		    *(STk_varlookup(CAR(l1), env, 0)) = EVALCAR(l2);

		  /* Evaluate body */
		  tmp =  CDR(CDR(tmp));
		  goto Begin;
		}
           case tc_macro:
	        x = Apply(fct->storage_as.macro.code, x);
	        goto Top;
	   case tc_quote:
		RETURN(CAR(tmp));
	   case tc_lambda:
		NEWCELL(x, tc_closure);
		x->storage_as.closure.env  = env;
		x->storage_as.closure.code = tmp;
		RETURN(x);
	   case tc_if:
		x = NEQ(EVALCAR(tmp), Ntruth) ? CAR(CDR(tmp))
					      : CAR(CDR(CDR(tmp)));
		goto Top;
	   case tc_setq:
		*(STk_varlookup(CAR(tmp), env, 0)) = EVALCAR(CDR(tmp));
		if (TRACED_VARP(CAR(tmp))) STk_change_value(CAR(tmp), env);
		RETURN(UNDEFINED);
	   case tc_cond:
		/* Don't use tmp because 
		 *     1) it's in a register 
		 *     2) we can arrive from tc_syntax 
		 */
		x = CDR(x); /* x is a "normal" var */
		if (eval_cond(&x, env) == Truth) goto Top;
		RETURN(x);
	   case tc_and:
		if (!len) RETURN(Truth);
		for (--len ; len; len--, tmp=CDR(tmp))
		  if (EVALCAR(tmp) == Ntruth) RETURN(Ntruth);
		x=CAR(tmp);
		goto Top;
	   case tc_or:
		if (!len) RETURN(Ntruth);
		for (--len; len; len--, tmp=CDR(tmp))
		  if ((fct=EVALCAR(tmp)) != Ntruth) RETURN(fct);
		x=CAR(tmp);
		goto Top;
	   case tc_call_cc:
		if (len != 1) goto Error;
		x = EVALCAR(tmp);
		if (STk_do_call_cc(&x) == Truth) goto Top;
		RETURN(x);
	   case tc_extend_env:
	     	fct = EVALCAR(tmp);
	        if (NENVP(fct)) Err("extend-env: bad environment", fct);
		tmp = CDR(tmp);
		env = STk_append(LIST2(fct->storage_as.env.data, env), 2);
		goto Begin;
	   case tc_apply:
		tmp = eval_args(tmp, env);
		fct = CAR(tmp);
		tmp = STk_liststar(CDR(tmp),len-1);
		if (STk_llength(tmp) == -1) Err("apply: bad parameter list", tmp);

		switch (TYPE(fct)) {
		  case tc_closure: env=STk_extend_env(
		    				CAR(fct->storage_as.closure.code),
						tmp,
						fct->storage_as.closure.env,
						x);
		    		   tmp = CDR(fct->storage_as.closure.code);
		    		   goto Begin;
		    case tc_apply: /* Here we are not tail recursive. (i.e. when
				    * we have something like (apply apply f ...)
				    * We cannot use a goto, since we should go again
				    * in tc_apply which will re-evaluates its 
				    * parameters. However, this kind of call 
				    * should be rare ...
				    */
				   RETURN(Apply(fct, tmp));
		    case tc_call_cc:
		    case tc_dynwind: x=Cons(fct, tmp);
				     goto Top;
#ifdef USE_STKLOS
		    case tc_instance:
		      		     RETURN(STk_apply_generic(fct, tmp));
		    case tc_next_method:
		      		     RETURN(STk_apply_next_method(fct,tmp));
#endif
		    default: 	     RETURN(Apply(fct, tmp));
		}
           default:
		if (EXTENDEDP(fct)) {
		  if (STk_extended_eval_parameters(fct)) 
		    tmp = eval_args(tmp, env);
		  RETURN(STk_extended_apply(fct, tmp, env));
		}
	        Err("eval: bad function in ", x);
	 }
       }
     default:
       RETURN(x);
     }
Out:
  stack = infos.previous;
  return tmp;

Error:
  Err("eval: Bad number of parameters", x);
  return UNDEFINED; /* never reached */
}


SCM STk_apply(SCM fct, SCM param)
{
Top:
  switch TYPE(fct) {
    case tc_subr_0:
         if (NULLP(param)) return SUBR0(fct)();
	 break;
    case tc_subr_1:
	 if  (STk_llength(param) == 1)return SUBRF(fct)(CAR(param));
	 break;
    case tc_subr_2:
	 if (STk_llength(param) == 2)
	   return SUBRF(fct)(CAR(param), CAR(CDR(param)));
	 break;
    case tc_subr_3:
	 if (STk_llength(param) == 3)
	   return SUBRF(fct)(CAR(param), CAR(CDR(param)), CAR(CDR(CDR(param))));
	 break;
    case tc_subr_0_or_1:
	 switch (STk_llength(param)) {
	   case 0: return SUBRF(fct)(UNBOUND);
	   case 1: return SUBRF(fct)(CAR(param));
	 }	 
    case tc_subr_1_or_2:
	 switch (STk_llength(param)) {
	   case 1: return SUBRF(fct)(CAR(param), UNBOUND);
	   case 2: return SUBRF(fct)(CAR(param), CAR(CDR(param)));
	 }

    case tc_subr_2_or_3:
	 switch (STk_llength(param)) {
	   case 2: return SUBRF(fct)(CAR(param), CAR(CDR(param)));
	   case 3: return SUBRF(fct)(CAR(param), CAR(CDR(param)), 
				     CAR(CDR(CDR(param))));
	 }
    case tc_ssubr:
	 return SUBRF(fct)(param, NIL, STk_llength(param));
    case tc_lsubr:
	 return SUBRF(fct)(param, STk_llength(param));
    case tc_cont:
	 if (STk_llength(param) == 1)
	   STk_throw(fct, CAR(param));	 
    case tc_closure: { 
         register SCM env = STk_extend_env(CAR(fct->storage_as.closure.code),
				       param,
				       fct->storage_as.closure.env,
				       fct);
	 register SCM code;
	 
	 for(code=CDR(fct->storage_as.closure.code); NNULLP(code); code=CDR(code))
	   param = EVALCAR(code);
	 return param;
       }
#ifdef USE_STKLOS
    case tc_instance:
        return STk_apply_generic(fct, param);
    case tc_next_method:
        return STk_apply_next_method(fct, param);
#endif
#ifdef USE_TK
    case tc_tkcommand:
      	return STk_execute_Tcl_lib_cmd(fct, param, NIL, 0);
#endif
    case tc_apply:
	 fct   = CAR(param);
	 param = STk_liststar(CDR(param), STk_llength(CDR(param)));
	 goto Top;
    default:
	 if (EXTENDEDP(fct)) 
	   if (STk_extended_procedurep(fct)) 
	     return STk_extended_apply(fct, param, UNBOUND);
	 Err("apply: bad procedure", fct);
  }

  Err("apply: bad number of arguments to apply", Cons(fct,param));
  return UNDEFINED; /* never reached */
}


PRIMITIVE STk_user_eval(SCM expr, SCM env)
{
  if (env == UNBOUND) env = STk_globenv;
  else 
    if (NENVP(env)) Err("eval: bad environment", env);

  /* If expr is a list, make a copy of it to avoid the user to see it modified
   * (i.e. "recoded") when eval returns
   */
  if (CONSP(expr)) expr = STk_copy_tree(expr);
  return STk_eval(expr, env->storage_as.env.data);
}


PRIMITIVE STk_eval_string(SCM str, SCM env)
{
  SCM result;

  if (env == UNBOUND) env = STk_globenv;
  else 
    if (NENVP(env)) Err("eval-string: bad environment", env);

  if (NSTRINGP(str)) Err("eval-string: Bad string", str);
  result = STk_internal_eval_string(CHARS(str), 
				    ERR_READ_FROM_STRING, 
				    env->storage_as.env.data);
  return result == EVAL_ERROR? UNDEFINED: result;
}
