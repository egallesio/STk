/*
 * s l i b . c				-- Misc functions
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: ??-Oct-1993 ??:?? 
 * Last file update: 25-Sep-1996 17:55
 *
 */

#ifdef WIN32
#  include <windows.h>
#endif

#include "stk.h"
#include "gc.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#   include <time.h>
#   include <dos.h>
#   include <process.h>
#else
#   include <stdarg.h>
#   include <sys/times.h>
#endif

#ifdef USE_TK
#   include "tk-glue.h"
#endif



#ifndef _DEBUG_MALLOC_INC

#ifdef malloc
#undef malloc
#endif

#ifdef realloc
#undef realloc
#endif

#define MAX_MALLOC_BEFORE_GC 1<<20	/* 1 Mb should suffice before calling GC */

static unsigned long malloc_count = 0;


static void cannot_allocate()
{
  fprintf(stderr, "**** Storage allocation from system failed\n");
  fprintf(stderr, "**** Evaluation aborted\n");
  
  STk_gc_requested = 1;

  /* Use a null message to avoid a call to report-error (which uses conses) */
  Err("", NIL);
}

void *STk_must_malloc(unsigned long size)
{
  void *tmp;

  tmp = (void *) malloc(size);

  /* Test for size because some libc return NULL when doing malloc(0) */
  if (tmp == NULL && size) cannot_allocate();

  /* Idea of malloc limitation comes from Harvey J. Stein 
   * <hjstein@MATH.HUJI.AC.IL>. The following code provoke a GC when
   * MAX_MALLOC_BEFORE_GC have been allocated by must_malloc. This 
   * allows to GC before all cells have been exhausted 
   */
  malloc_count +=size;
  if (malloc_count > MAX_MALLOC_BEFORE_GC) {
    malloc_count     = 0;
    STk_gc_requested = 1;
  }

  return tmp;
}

void *STk_must_realloc(void *ptr, unsigned long size)
{
  void *tmp;

  tmp = (void *) realloc(ptr, size);
  /* Since we cannot know (in a portable way) the size of area pointed by ptr,
   * we will make the assumption that it is half the new requested size. 
   * Of course, we are probably false here, but it seems more reasonable than 
   * brutally increment it with size. 
   */
  malloc_count +=size/2;
  if (malloc_count > MAX_MALLOC_BEFORE_GC) {
    malloc_count     = 0;
    STk_gc_requested = 1;
  }

  if (tmp == NULL) cannot_allocate();
  return tmp;
}
#endif


SCM STk_internal_eval_string(char *s, long context, SCM env)
{
  jmp_buf jb, *prev_jb = Top_jmp_buf;
  long prev_context     = Error_context;
  SCM result, port;
  int k;
  
  /* Create a string port to read the command and evaluate it in a new context */
  port = STk_internal_open_input_string(s);

  /* save normal error jmpbuf  so that eval error don't lead to toplevel */
  /* If in a "catch", keep the ERR_IGNORED bit set */
  if ((k = setjmp(jb)) == 0) {
    Top_jmp_buf   = &jb;
    Error_context = (Error_context & ERR_IGNORED) | context;
    result = STk_eval(STk_readf(PORT_FILE(port), FALSE), env);
  }
  Top_jmp_buf   = prev_jb;;
  Error_context = prev_context;
 
  if (k == 0) return result;
  /* if we are here, an error has occured during the string reading 
   * Two cases:
   *    - we are in a catch. Do a longjump to the catch to signal it a fail
   *    - otherwise error has already signaled, just return EVAL_ERROR
   */
  if (Error_context & ERR_IGNORED) longjmp(*Top_jmp_buf, k);
  return EVAL_ERROR;
}


PRIMITIVE STk_catch(SCM expr, SCM env, int unused_len)
{
  jmp_buf jb, *prev_jb = Top_jmp_buf;
  long prev_context     = Error_context;
  SCM l;
  int k;
  
  /* save normal error jmpbuf  so that eval error don't lead to toplevel */
  if ((k = setjmp(jb)) == 0) {
    Top_jmp_buf   = &jb;
    Error_context |= ERR_IGNORED;

    /* Evaluate the list of expressions */
    for (l = expr; NNULLP(l); l = CDR(l)) 
      STk_eval(CAR(l), env);
  }
  Top_jmp_buf   = prev_jb;
  Error_context = prev_context; /* Don't use a mask to allow nested call to catch */

  return (k == 0)? Ntruth: Truth;
}

PRIMITIVE STk_quit_interpreter(SCM retcode)
{
  long ret = 0;

  if (retcode != UNBOUND) {
    if ((ret=STk_integer_value(retcode)) == LONG_MIN)
      Err("quit: bad return code", retcode);
  }
  STk_unwind_all();

  /* call user finalization code */
  STk_user_cleanup();

#ifdef USE_TK
  /* Unregister the interpreter from X server */
  if (Tk_initialized) Tcl_DeleteCommand(STk_main_interp, ".");
#endif

#if defined(WIN32) && defined(USE_SOCKET)
  /* Unregister the interpreter from Winsock */
  WSACleanup();  
#endif
  exit(ret);
}

PRIMITIVE STk_version(void)
{
  return STk_makestring(STK_VERSION);
}

PRIMITIVE STk_machine_type(void)
{
  return STk_makestring(MACHINE);
}

PRIMITIVE STk_library_location(void)
{
   return STk_makestring(STk_library_path);
}

PRIMITIVE STk_random(SCM n)
{
  if (NEXACTP(n) || STk_negativep(n) == Truth || STk_zerop(n) == Truth)
    Err("random: bad number", n);
  return STk_modulo(STk_makeinteger(rand()), n);
}

PRIMITIVE STk_set_random_seed(SCM n)
{
  if (NEXACTP(n)) Err("set-random-seed!: bad number", n);
  srand(STk_integer_value_no_overflow(n));
  return UNDEFINED;
}

#ifndef HZ
#define HZ 60.0
#endif

#ifdef CLOCKS_PER_SEC
#  define TIC CLOCKS_PER_SEC
#else 
#  define TIC HZ
#endif

double STk_my_time(void)
{
#ifdef WIN32
  return (long) 1000*(clock()/CLK_TCK);
#else
  struct tms time_buffer;
  times(&time_buffer);
  return 1000 * (time_buffer.tms_utime + time_buffer.tms_stime) / TIC;
#endif
}


PRIMITIVE STk_get_internal_info(void)
{
  SCM z = STk_makevect(7, NULL);
  long allocated, used, calls;

  /* The result is a vector which contains
   *	0 The total cpu used in ms
   *	1 The number of cells currently in use.
   *    2 Total number of allocated cells
   *	3 The number of cells used since the last call to get-internal-info
   *	4 Number of gc calls
   *    5 Total time used in the gc
   *	6 A boolean indicating if Tk is initialized
   */

  STk_gc_count_cells(&allocated, &used, &calls);

  VECT(z)[0] = STk_makenumber(STk_my_time());
  VECT(z)[1] = STk_makeinteger(used);
  VECT(z)[2] = STk_makeinteger(allocated);
  VECT(z)[3] = STk_makenumber((double) STk_alloc_cells);
  VECT(z)[4] = STk_makeinteger(calls);
  VECT(z)[5] = STk_makenumber((double) STk_total_gc_time);
#ifdef USE_TK
  VECT(z)[6] = Tk_initialized ? Truth: Ntruth;
#else
  VECT(z)[6] = Ntruth;
#endif
  
  STk_alloc_cells = 0;
  return z;
}


PRIMITIVE STk_time(SCM expr, SCM env, int len)
{
  double rt, gc_time;
  SCM res;

  if (len != 1) Err("time: bad expression" , expr);

  STk_alloc_cells = 0;
  gc_time         = STk_total_gc_time;
  rt 	          = STk_my_time();
  res 	          = EVALCAR(expr);
  fprintf(STk_stderr, ";;    Time: %.2fms\n;; GC time: %.2fms\n;;   Cells: %ld\n",
	  STk_my_time()-rt, STk_total_gc_time-gc_time, STk_alloc_cells);
  return res;
}


/*
 * STk_delete_Tcl_child_Interp
 *
 * 	This function must be called by a child process just after a fork
 * It deletes the Tk commands associated to the interpreter (except send, 
 * to avoid interpreter unregistering).
 * 
 * This must probably rewritten
 */

void STk_delete_Tcl_child_Interp(void)
{
#ifdef USE_TK
  if (Tk_initialized) {
    struct Tk_command *W;
    Interp *iPtr = (Interp *) STk_main_interp;
    Tcl_HashEntry *hPtr;

    /* Try to find "send". Modify it's delproc to point NULL */
    hPtr = Tcl_FindHashEntry(&iPtr->commandTable, "send");
    if (hPtr != NULL) {
      W = (struct Tk_command *) Tcl_GetHashValue(hPtr);
      W->delproc = NULL;
    }
    /* Now we can destroy the interpreter  (send will not be destroyed) */
    Tcl_DeleteInterp(STk_main_interp);
    
    /* Report-error points to a graphical procedure. Undefine it 
     * to display error messages on stderr in the child process
     */
    STk_set_symbol_value("report-error", UNBOUND);

    /* Redefine exit to the standard STk exit function */
    STk_add_new_primitive("exit", tc_subr_0_or_1, STk_quit_interpreter);
  }
#else
  /* Nothing to do for Snow */
#endif
}


/* When STk evaluates an expression, it recodes it in a manner which permits it
   to be more efficient for further evaluations. The uncode functions permits to 
   do the reverse job: it takes an exppression and returns a form similar to the 
   original one. 
   Warning: when a macro has been expanded, there is no mean to "revert" it to 
   its original form 
*/


static SCM associate(SCM l1, SCM l2)
{
  SCM z;

  if (NULLP(l1)) return NIL;
  
  for(z= NIL; NNULLP(l1); l1=CDR(l1), l2=CDR(l2))
    z = Cons(LIST2(CAR(l1), STk_uncode(CAR(l2))), z);
  
  return Reverse(z);
}

static SCM uncode_let(char *type, SCM expr)
{
  return Cons(Intern(type),
	      Cons(associate(CAR(expr), CAR(CDR(expr))),
		   STk_uncode(CDR(CDR(expr)))));
}
  
PRIMITIVE STk_uncode(SCM expr)
{
  switch (TYPE(expr)) {
    case tc_cons: switch (TYPE(CAR(expr))) {
    		    case tc_let:     return uncode_let("let",    CDR(expr)); 
      		    case tc_letstar: return uncode_let("let*",   CDR(expr)); 
		    case tc_letrec:  return uncode_let("letrec", CDR(expr)); 
		    case tc_if: 
		         expr = CDR(expr);
		         if (EQ(CAR(CDR(CDR(expr))), UNDEFINED)) 
			   return Cons(Intern("if"),
				       LIST2(STk_uncode(CAR(expr)),
					     STk_uncode(CAR(CDR(expr)))));
			 else
			   return Cons(Intern("if"),
				       LIST3(STk_uncode(CAR(expr)),
					     STk_uncode(CAR(CDR(expr))),
					     STk_uncode(CAR(CDR(CDR(expr))))));
		    default: return Cons(STk_uncode(CAR(expr)),
					 STk_uncode(CDR(expr)));
		  }
    case tc_quote:	return Intern("quote");
    case tc_lambda:	return Intern("lambda");
    case tc_if:		return Intern("if");
    case tc_setq:	return Intern("set!");
    case tc_cond:	return Intern("cond");
    case tc_and:	return Intern("and");
    case tc_or:		return Intern("or");
    case tc_let:	return Intern("let");
    case tc_letstar:	return Intern("letstar");
    case tc_letrec: 	return Intern("letrec");
    case tc_begin:	return Intern("begin");
    case tc_globalvar:  return VCELL(expr);
    case tc_localvar:   return expr->storage_as.localvar.symbol;
    case tc_apply:	return Intern("apply");
    case tc_call_cc:	return Intern("call-with-current-continuation");
    case tc_dynwind:    return Intern("dynamic-wind");
    case tc_extend_env: return Intern("extend-environment");
    default:		return expr;
  }
}

/* 
 * A Panic procedure.
 */
void STk_panic TCL_VARARGS_DEF(char *,arg1)
{
  va_list argList;
  char buf[1024];
  char *format;
	 
  format = TCL_VARARGS_START(char *,arg1,argList);
  vsprintf(buf, format, argList);

#ifdef WIN32
  MessageBeep(MB_ICONEXCLAMATION);
  MessageBox(NULL, buf, "Fatal error in STk", 
	     MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#else
  fprintf(STk_stderr, "\n**** %s\n", buf);
  fflush(STk_stderr);
#endif
  exit(1);
}


/******************************************************************************
 *
 * The following declarations serve only for referencing symbols which are used
 * by Tcl or Tk and which are defined in this directory. Otherwise, the ld will
 * not find them and report an error
 *
 ******************************************************************************/
#ifndef WIN32
typedef void (*dumb)();

dumb STk_dumb[] = { 
  (dumb) Tcl_TildeSubst,
  (dumb) Tcl_SetVar2
};
#endif
