/*
 *
 * c o n t . c				-- Continuations management
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
 *    Creation date:  8-Nov-1993 11:34
 * Last file update: 26-Apr-1996 17:46
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
  SCM wind_stack;
  SCM *start;
  unsigned length;
  SCM stack[1];
};

#define C_ENV(x) 	(((struct cont *)((x)->storage_as.cont.data))->env)
#define C_START(x)	(((struct cont *)((x)->storage_as.cont.data))->start)
#define C_LEN(x)	(((struct cont *)((x)->storage_as.cont.data))->length)
#define C_STACK(x)	(((struct cont *)((x)->storage_as.cont.data))->stack)
#define C_WIND_STACK(x)	(((struct cont *)((x)->storage_as.cont.data))->wind_stack)


static SCM call_cc_escaped_value;

/* Don't allocate these vars on stack */
static SCM *from, *to;
static long length;
static int  i;


static void unwind(SCM stop, int n);

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
  C_LEN(z)   	  = length;
  C_WIND_STACK(z) = STk_wind_stack;
  FLUSH_REGISTERS_WINDOW();
  for (i=length, to = C_STACK(z); i--; ) *to++ = *from++;

  return z;
}

SCM STk_mark_continuation(SCM cont)
{
  STk_mark_stack((SCM *)C_STACK(cont), (SCM *)(C_STACK(cont)+C_LEN(cont)-1));
  return C_WIND_STACK(cont);
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

void STk_throw(SCM fct, SCM val)
{
  static SCM tmp;
  union {
    SCM stack_end;
    SCM hole[1024]; /* Reserve 1K-pointers on stack */
  }u;

  /* Evaluate room on stack. If not enough call throw again to alloc. a new hole */
  if (&u.stack_end < STk_stack_start_ptr) {
    /* Stack grows downward */
    if (&u.stack_end > C_START(fct)) STk_throw(fct, val);
  }
  else {
    /* Stack grows upward */
    if (&u.stack_end < C_START(fct)+ C_LEN(fct)) STk_throw(fct, val);
  }

  /* Take care of active dynamic-winds */
  tmp = C_WIND_STACK(fct);
  unwind(tmp, STk_llength(STk_wind_stack) - STk_llength(tmp));

  /* Save val in a global and reset stack as it was before calling call/cc */
  call_cc_escaped_value = val; tmp = fct;
  FLUSH_REGISTERS_WINDOW();
  for(to=C_START(fct), from=C_STACK(fct), i=C_LEN(fct); i--; ) *to++ = *from++;

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

void STk_unwind_all(void)
{
  SCM p;

  for (p = STk_wind_stack; NNULLP(p); p = CDR(p)) {
    STk_wind_stack = CDR(p);
    Apply(CAR(CDR(CAR(p))), NIL);
  }
}

static void unwind(SCM stop, int n)
{
  if (STk_wind_stack != stop) {
    if (n < 0) {
      unwind(CDR(stop),n+1);
      Apply(CAR(CAR(stop)),NIL);
      STk_wind_stack = stop;
    }
    else {
      SCM old_wind_stack = STk_wind_stack;
      
      STk_wind_stack = CDR(STk_wind_stack);
      Apply(CAR(CDR(CAR(old_wind_stack))), NIL);
      unwind(stop, n-1);
    }
  }
}

static void test_procedure(SCM thunk)
{
  if (!STk_is_thunk(thunk)) Err("dynamic-wind: bad procedure", thunk);
}

PRIMITIVE STk_dynamic_wind(SCM thunk1, SCM thunk2, SCM thunk3)
{
  SCM result;

  test_procedure(thunk1);
  test_procedure(thunk2);
  test_procedure(thunk3);

  Apply(thunk1, NIL);
  STk_wind_stack = Cons(LIST2(thunk1, thunk3), STk_wind_stack);
  result = Apply(thunk2, NIL);
  STk_wind_stack = CDR(STk_wind_stack);
  Apply(thunk3, NIL);
  return result;
}
