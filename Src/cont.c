/*
 *
 * c o n t . c				-- Continuations management
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
 * $Id: cont.c 1.4 Wed, 23 Dec 1998 23:41:27 +0100 eg $
 *
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date:  8-Nov-1993 11:34
 * Last file update: 20-Dec-1998 21:25
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
  reenter_cont(C_HANDLER(fct));

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
  SCM thunks = NIL;
  
  before 	  = STk_err_handler;
  STk_err_handler = eh;
  
  /* Scan the stack and collect all the thunk1(s) we'll have to call. Since we
   * have the thunks in reverse order push them in a list
   */
  for (p = STk_err_handler; p && p != before; p = p->prev) {
    if (NNULLP(p->dynamic_handler))
      thunks = Cons(CAR(p->dynamic_handler), thunks);
  }
  /* Execute all the handler now */
  for ( ; NNULLP(thunks); thunks = CDR(thunks))
    STk_apply(CAR(thunks), NIL);
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
    STk_apply(CAR(thunks), NIL);
}


static void test_procedure(SCM thunk)
{
  if (!STk_is_thunk(thunk)) Err("dynamic-wind: bad procedure", thunk);
}


PRIMITIVE STk_dynamic_wind(SCM thunk1, SCM thunk2, SCM thunk3)
{
  SCM result;

  test_procedure(thunk1); test_procedure(thunk2); test_procedure(thunk3);

  Apply(thunk1, NIL);
  PUSH_ERROR_HANDLER
    {
      STk_err_handler->dynamic_handler = Cons(thunk1, thunk3);
      result = Apply(thunk2, NIL);
    }
  WHEN_ERROR
    {
      Apply(thunk3, NIL);
      PROPAGATE_ERROR();
    }
  POP_ERROR_HANDLER;
  Apply(thunk3, NIL);
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

  res = Apply(producer, NIL);
  return Apply(consumer, VALUESP(res) ? CAR(res) : LIST1(res));
}
