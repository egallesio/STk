/*
 *
 * c o n t . c				-- Continuations management
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
 *    Creation date:  8-Nov-1993 11:34
 * Last file update:  3-Sep-1999 20:19 (eg)
 */

#include "stk.h"
#include "gc.h"

#ifdef sparc
#define FLUSH_REGISTERS_WINDOW()	asm("t 0x3") /* Stolen in Elk 2.0 source */
#else
#define FLUSH_REGISTERS_WINDOW()
#endif

struct cont {
  jmp_buf env;
  struct error_handler *eh;
  SCM *start;
  unsigned length;
  SCM stack[1];
};

#define C_ENV(x) 	(((struct cont *)((x)->storage_as.cont.data))->env)
#define C_HANDLER(x)    (((struct cont *)((x)->storage_as.cont.data))->eh)
#define C_START(x)	(((struct cont *)((x)->storage_as.cont.data))->start)
#define C_LEN(x)	(((struct cont *)((x)->storage_as.cont.data))->length)
#define C_STACK(x)	(((struct cont *)((x)->storage_as.cont.data))->stack)

static SCM values(SCM l, int len);
static void reenter_cont(struct error_handler *eh);

/* Don't allocate these vars on stack */
static SCM call_cc_escaped_value;
static SCM *from, *to;
static long length;
static int  i;


static int get_stack_length(void)
{
  SCM stack_limit;
  
  return (&stack_limit < STk_stack_start_ptr) ? STk_stack_start_ptr - &stack_limit
    					      : &stack_limit - STk_stack_start_ptr;
}

static SCM prepare_call_cc(SCM proc)
{
  SCM z;

  if (STk_procedurep(proc) == Ntruth)
    Err("call-with-current-continuation: Bad procedure", proc);

  /* Find the start adress and the length of the stack to save */
  length = get_stack_length();
  from   = (STk_stack_start_ptr<&z) ? STk_stack_start_ptr 
				    : STk_stack_start_ptr-length;

  /* Allocate a new object for this continuation */
  NEWCELL(z, tc_cont);

  z->storage_as.cont.data = must_malloc(sizeof(struct cont) + length * sizeof(SCM));

  C_START(z) 	  = from;
  C_HANDLER(z)	  = STk_err_handler;
  C_LEN(z)   	  = length;
  FLUSH_REGISTERS_WINDOW();
  for (i=length, to = C_STACK(z); i--; ) *to++ = *from++;

  return z;
}

void STk_mark_continuation(SCM cont)
{
  STk_mark_stack((SCM *)C_STACK(cont), (SCM *)(C_STACK(cont)+C_LEN(cont)-1));
}

SCM STk_do_call_cc(SCM *x)
{
  SCM tmp;
  
  tmp = prepare_call_cc(*x);
  /* Use a setjmp/longjmp for the continuation */
  if (setjmp(C_ENV(tmp)) == 0) {
    *x = LIST2(*x, tmp);
    return Truth;
  }
  else
    return (*x=call_cc_escaped_value);
}

void STk_throw(SCM fct, SCM vals)
{
  static SCM tmp;
  union {
    SCM stack_end;
    SCM hole[1024]; /* Reserve 1K-pointers on stack */
  }u;

  /* Evaluate room on stack. If not enough call throw again to alloc. a new hole */
  if (&u.stack_end < STk_stack_start_ptr) {
    /* Stack grows downward */
    if (&u.stack_end > C_START(fct)) STk_throw(fct, vals);
  }
  else {
    /* Stack grows upward */
    if (&u.stack_end < C_START(fct)+ C_LEN(fct)) STk_throw(fct, vals);
  }

  /* Save val in a global and reset stack as it was before calling call/cc */
  call_cc_escaped_value = values(vals, STk_llength(vals)); tmp = fct;
  FLUSH_REGISTERS_WINDOW();
  for(to=C_START(fct), from=C_STACK(fct), i=C_LEN(fct); i--; ) *to++ = *from++;

  /* Everything is restored. Execute the thunk1 of dynamic-wind we enter in back */
  reenter_cont(C_HANDLER(tmp));

  /* And Go! */
  longjmp(C_ENV(tmp), JMP_THROW);
}

PRIMITIVE STk_continuationp(SCM obj)
{
  return CONTINUATIONP(obj)? Truth: Ntruth;
}

/******************************************************************************
 *
 * Dynamic wind 
 *
 ******************************************************************************/


static void reenter_cont(struct error_handler *eh)
{
  struct error_handler *p, *before;
  SCM thunks1 = NIL;
  SCM thunks2 = NIL;
  
  before 	  = STk_err_handler;
  STk_err_handler = eh;
  
  /* Scan the stack and collect all the thunk1(s) we'll have to call. Since we
   * have the thunks in reverse order push them in a list
   */
  for (p = STk_err_handler; p && p != before; p = p->prev) {
    if (NNULLP(p->dynamic_handler)) {
      thunks1 = Cons(CAR(p->dynamic_handler), thunks1);
      thunks2 = Cons(CDR(p->dynamic_handler), thunks2);
    }
  }
  /* Execute all the handler now */
  for ( ; NNULLP(thunks2); thunks2 = CDR(thunks2))
    Apply0(CAR(thunks2));
  /* Execute all the handler now */
  for ( ; NNULLP(thunks1); thunks1 = CDR(thunks1))
    Apply0(CAR(thunks1));

}


void STk_unwind_all(void)   /* called when we exit the interpreter */
{
  struct error_handler *p;
  SCM thunks = NIL;
  
  /* Scan the stack and collect all the thunk3(s) we'll have to call. Since we
   * have the thunks in reverse order push them in a list
   */
  for (p = STk_err_handler; p ; p = p->prev) {
    if (NNULLP(p->dynamic_handler))
      thunks = Cons(CDR(p->dynamic_handler), thunks);
  }
  /* Execute all the handler now */
  for ( ; NNULLP(thunks); thunks = CDR(thunks))
    Apply0(CAR(thunks));
}


static void test_procedure(SCM thunk)
{
  if (!STk_is_thunk(thunk)) Err("dynamic-wind: bad procedure", thunk);
}


PRIMITIVE STk_dynamic_wind(SCM thunk1, SCM thunk2, SCM thunk3)
{
  SCM result;

  test_procedure(thunk1); test_procedure(thunk2); test_procedure(thunk3);

  Apply0(thunk1);
  PUSH_ERROR_HANDLER
    {
      STk_err_handler->dynamic_handler = Cons(thunk1, thunk3);
      result = Apply0(thunk2);
    }
  WHEN_ERROR
    {
      Apply0(thunk3);
      PROPAGATE_ERROR();
    }
  POP_ERROR_HANDLER;
  Apply0(thunk3);
  return result;
}

/******************************************************************************
 *
 * R5RS values
 * 
 ******************************************************************************/

static SCM values(SCM l, int len)
{
  if (len == 1)
    return CAR(l);
  else {
    SCM z;
    
    NEWCELL(z, tc_values);
    CAR(z) = l;
    return z;
  }
}

PRIMITIVE STk_values(SCM l, int len)
{
  SCM tmp;

  ENTER_PRIMITIVE("values");

  /* check the args for not being multiple-valued */
  for (tmp = l ; NNULLP(tmp) ; tmp = CDR(tmp))
    if (TYPEP(CAR(tmp), tc_values)) Serror("bad argument", CAR(tmp));

  return values(l, len);
}


PRIMITIVE STk_call_with_values(SCM producer, SCM consumer)
{
  SCM res;

  ENTER_PRIMITIVE("call-with-values");

  if (!STk_procedurep(producer)) Serror("bad producer", producer);
  if (!STk_procedurep(consumer)) Serror("bad consumer", consumer);

  res = Apply0(producer);
  return Apply(consumer, VALUESP(res) ? CAR(res) : LIST1(res));
}
