/*
 *
 * s t a c k  . c		-- Implementation of the extended type Stack
 *
 */

#include <stk.h>

static void mark_stack(SCM p);
static void free_stack(SCM p);
static void display_stack(SCM s, SCM port, int mode);
static int tc_stack;

static STk_extended_scheme_type stack_type = {
  "stack",		/* name */
  0,			/* is_procp */
  mark_stack,		/* gc_mark_fct */
  free_stack,		/* gc_sweep_fct */
  NULL,			/* apply_fct */
  display_stack		/* display_fct */
};

#define STACKP(x)	(TYPEP(x, tc_stack))
#define NSTACKP(x)	(NTYPEP(x, tc_stack))
#define STACK(x)	((Stack *) EXTDATA(x))

typedef struct {
  int len;
  SCM values;
} Stack;

static void mark_stack(SCM p)
{
  STk_gc_mark(STACK(p)->values);
}

static void free_stack(SCM p)
{
  free(EXTDATA(p));
}

static void display_stack(SCM s, SCM port, int mode)
{
  char buffer[100];
  if (mode == DSP_MODE) {
    /* A verbose display */
    if (STACK(s)->len) {
      sprintf(buffer, "Stack length = %d\nValues = ", STACK(s)->len);
      Puts(buffer, PORT_FILE(port));
      STk_display(STACK(s)->values, port);
    }
    else 
      Puts("Stack is empty", PORT_FILE(port));
  }
  else { /* WRT_MODE or TK_MODE */
    sprintf(buffer, "#<stack (length=%d) %ld>", STACK(s)->len, s);
    Puts(buffer, PORT_FILE(port));
  }
}

static PRIMITIVE make_stack(void)
{
  SCM z;

  NEWCELL(z, tc_stack);
  EXTDATA(z)       = STk_must_malloc(sizeof(Stack));
  STACK(z)->len    = 0;
  STACK(z)->values = NIL;
  return z;
}

static PRIMITIVE stackp(SCM s)
{
  return STACKP(s)? Truth: Ntruth;
}

static PRIMITIVE stack_push(SCM s, SCM val)
{
  Stack *sp;

  if (NSTACKP(s)) STk_err("stack-push: bad stack", s);

  sp         =  STACK(s);
  sp->len   += 1;
  sp->values = Cons(val, sp->values);
  
  return UNDEFINED;
}

static PRIMITIVE stack_pop(SCM s)
{
  Stack *sp;
  SCM res;

  if (NSTACKP(s)) STk_err("stack-pop: bad stack", s);
  
  sp =  STACK(s);
  
  if (sp->len == 0) STk_err("stack-pop: empty stack", s);
  res         = CAR(sp->values);
  sp->len    -= 1;
  sp->values  = CDR(sp->values);

  return res;
}

static PRIMITIVE stack_emptyp(SCM s)
{
  if (NSTACKP(s)) STk_err("stack-empty?: bad stack", s);
  return (STACK(s)->len) ? Truth: Ntruth;
}


PRIMITIVE STk_init_stack(void)
{
  /* Register the new type */
  tc_stack = STk_add_new_type(&stack_type);
  
  /* Declare new primitives */
  STk_add_new_primitive("make-stack",     tc_subr_0,      make_stack);
  STk_add_new_primitive("stack?",	  tc_subr_1,      stackp);
  STk_add_new_primitive("stack-push!",    tc_subr_2,      stack_push);
  STk_add_new_primitive("stack-pop",      tc_subr_1,      stack_pop);
  STk_add_new_primitive("stack-empty?",   tc_subr_1,      stack_emptyp);
  
  return UNDEFINED;
}
