/*
 * m a c r o s . c			-- Simple statically scoped macros
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: ??-Oct-1993 ??:?? 
 * Last file update:  3-Sep-1999 20:21 (eg)
 *
 */


#include "stk.h"

PRIMITIVE STk_macro(SCM args, SCM env, int len)
{
  SCM z, code;
  
  if (len != 2) Err("macro: Bad definition", args);

  code = Cons(Sym_lambda, args); /* Create code before to avoid GC problems */
  NEWCELL(z, tc_macro);
  z->storage_as.macro.code = EVAL(code);
  z->storage_as.macro.env  = Ntruth;
  return z;
}

PRIMITIVE STk_macro_R5(SCM args, SCM env, int len)
{
  SCM z = STk_macro(args, env, len);

  z->storage_as.macro.env  = env;
  return z;
}


/*
 * macro_expand and macro_expand_1 are defined as fsubr rather than subr_1.
 * This provision permits to carry the environment with the form to expand
 * (this permits to corerctly expanse macros which are not are global level, 
 * since we do a varlookup instead of grabbing the macro in a global var).
 * But are there people which define macros at a level which are not global?
 *
 */

static SCM expand(SCM form, SCM env, int just_1)
{
  if (CONSP(form)) {
    register SCM tmp, car = CAR(form);
    
    if (SYMBOLP(car) && MACROP(tmp = *STk_varlookup(car, env, FALSE))) {
      tmp = Apply(tmp->storage_as.macro.code, form);
      return just_1 ? tmp : expand(tmp, env, FALSE);
    }
  }
  return form;
}

PRIMITIVE STk_macro_expand_1(SCM form, SCM env, int len)
{
  if (len != 1) Err("macro-expand-1: bad number of parameters", form);
  return expand(EVAL(CAR(form)), env, TRUE);
}

PRIMITIVE STk_macro_expand(SCM form, SCM env, int len)
{
  if (len != 1) Err("macro-expand: bad number of parameters", form);
  return expand(EVAL(CAR(form)), env, FALSE);
}

PRIMITIVE STk_macro_body(SCM form)
{
  if (NMACROP(form)) Err("macro-body: bad macro", form);
  return Cons(Intern("macro"), CDR(form->storage_as.macro.code));
}

PRIMITIVE STk_macrop(SCM obj)
{
  return MACROP(obj)? Truth: Ntruth;
}

