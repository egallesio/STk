#include <stk.h>

static PRIMITIVE STk_when(SCM l, SCM env)
{
  SCM res = UNDEFINED;

  if (NULLP(l)) Err("when: no argument list given", NIL);
  if (STk_eval(CAR(l), env) != Ntruth) {
    if (NULLP(l = CDR(l))) Err("when: null body", NIL);
    
    /* Argument list is well formed. Evaluates each expression of the body */
    for (  ; !NULLP(l); l = CDR(l))
      res = STk_eval(CAR(l), env);
  }
  return res;
}

static PRIMITIVE STk_unless(SCM l, SCM env)
{
  SCM res = UNDEFINED;

  if (!NULLP(l)) {
    if (STk_eval(CAR(l), env) == Ntruth) {
      for (l = CDR(l); !NULLP(l); l = CDR(l)) {
	res = STk_eval(CAR(l), env);
      }
    }
  }
  return res;
}

  
void STk_init_when(void)
{
  STk_add_new_primitive("when", 	   tc_fsubr, STk_when);
  STk_add_new_primitive("unless",	   tc_fsubr, STk_unless);
}
