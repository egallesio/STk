/*
 *
 * s i g n a l . c			-- Signal handling
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 10-Oct-1995 07:55
 * Last file update: 10-Sep-1998 12:25
 *
 */

#include "stk.h"
#include <signal.h>

#ifdef HAVE_SIGACTION
   static sigset_t global_sigset;
#  ifdef SA_RESTART
#    define SA_FLAGS SA_RESTART
#  else 
#    define SA_FLAGS 0
#  endif
#endif

int STk_sigint_counter = 0;
int STk_control_C      = 0;

static SCM signals[MAX_SIGNAL];

/* Convention: signals[i] can be 
 * 			- #f if signal is ignored
 *			- #t if signal is set to default
 *			- a list of handlers
 */

static void execute_signal_handlers(int sig, SCM handlers)
{
  SCM arg = LIST1(STk_makeinteger(sig));

  for ( ; CONSP(handlers); handlers = CDR(handlers)) {
    if (STk_apply(CAR(handlers), arg) == Sym_break) break;
  } 
}


static void handle_sigint_signal(void)
{
  SCM l = signals[SIGINT];
  
  STk_control_C = 0;
  if (l == NIL) {
    /* User has not redefined ^C action */
#ifdef HAVE_SIGACTION
    /* Empty the signal mask before calling STk_err, because it will do 
     * a longjmp and don't let the system (at least Linux) a chance to 
     * restore the mask 
     */
    sigset_t set;
    
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
#endif 
    fprintf(STk_stderr, "*** Interrupt ***\n"); fflush(stderr);
    STk_err("", NIL);
  }
  else
    execute_signal_handlers(SIGINT, l);
}

#ifdef SIGSEGV
static void handle_sigsegv_signal(void)
{
  SCM l = signals[SIGSEGV];
  
  if (l == NIL) {
    /* User has not redefined SIGSEGV action */
#ifdef HAVE_SIGACTION
    /* See comment in procedure above */
    sigset_t set;
    
    sigemptyset(&set);
    sigaddset(&set, SIGSEGV);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
#endif
    /*  SEGV occurs generally when there is an error in the
     * interpreter. We cannot do a lot of things here, excepted
     * signalling this fact and go back to toplevel
     */
    Err("Segmentation violation: Returning to toplevel", NIL);
  }
  else 
    execute_signal_handlers(SIGSEGV, l);
}
#endif


void STk_handle_signal(int sig)
{
#ifndef  HAVE_SIGACTION
  if (sig < MAX_SYSTEM_SIG)
    signal(sig, STk_handle_signal);
#endif

  if (sig == SIGINT) {			/* SIGINT */
    if (STk_sigint_counter > 0) {
      /* ^C is disallowed for now. (We are probably in a sequence which
       * uses malloc). Retain that a Control-C was issued and return
       */
      STk_control_C = 1;
      SET_EVAL_FLAG(1);
      return;
    }
    else handle_sigint_signal();
  }
#ifdef SIGSEGV
  else if (sig == SIGSEGV) {		/* SIGSEGV */
     handle_sigsegv_signal();
  }
#endif
  else
    execute_signal_handlers(sig, signals[sig]);
}


static SCM set_handler(long s, SCM proc)
{
  void (*handler)();

  if (BOOLEANP(proc))
    /* Special case #f means ignore and #t means default */
    handler = (proc == Truth) ? SIG_DFL: SIG_IGN;
  else
    /* Use our handler */
    handler = STk_handle_signal;
     
  if (s < MAX_SYSTEM_SIG)
#ifdef HAVE_SIGACTION
    {
      struct sigaction sigact;
      
      sigfillset(&(sigact.sa_mask));
      /* FIXME: => handler non interruptible mais est-ce un problème en pratique? */
      /* sigdelset(&(sigact.sa_mask), SIGINT);  */
      sigact.sa_handler = handler;
      sigact.sa_flags   = SA_FLAGS;
      sigaction(s, &sigact, NULL);
    }
#else
    signal(s, handler);
#endif
  return signals[s] = proc;
}


PRIMITIVE STk_set_signal_handler(SCM sig, SCM proc)
{
  long s = STk_integer_value(sig);

  if (s == LONG_MIN || s < 0 || s >= MAX_SIGNAL)
    STk_err("set-signal-handler!: bad signal number", sig);

  if (BOOLEANP(proc))
    return set_handler(s, proc);
  else {
    if (STk_procedurep(proc) == Ntruth)
      STk_err("set-signal-handler!: bad procedure", proc);
    return set_handler(s, LIST1(proc));
  }
}

  
PRIMITIVE STk_add_signal_handler(SCM sig, SCM proc)
{
  long s = STk_integer_value(sig);

  if (s == LONG_MIN || s < 0 || s >= MAX_SIGNAL)
    STk_err("add-signal-handler!: bad signal number", sig);

  if (STk_procedurep(proc) == Ntruth) 
    STk_err("add-signal-handler!: bad procedure", proc);

  if (BOOLEANP(signals[s]))
    /* We add a handler on a defaulted or ignored signal */
    return  set_handler(s, LIST1(proc));
  else
    /* Just add the new signal in front of the list */
    return signals[s] = Cons(proc,signals[s]);
}

PRIMITIVE STk_get_signal_handlers(SCM sig)
{
  if (sig == UNBOUND) {				/* Return all handlers */
    SCM z;
  
    z = STk_makevect(MAX_SIGNAL, NULL);
    memcpy(VECT(z), signals, MAX_SIGNAL * sizeof(SCM));
    return z; 
  }
  else {					/* Return handlers of sig only */
    long s = STk_integer_value(sig);

    if (s == LONG_MIN || s < 0 || s >= MAX_SIGNAL)
      STk_err("get-signal-handlers: bad signal number", sig);
    return signals[s];
  }
}

void STk_mark_signal_table(void)
{
  int i;

  for (i = 0; i < MAX_SIGNAL; i++) 
    STk_gc_mark(signals[i]);
}


void STk_ignore_signals(void)	/* Block all signals */
{
#ifdef HAVE_SIGACTION
  sigset_t new;

  sigfillset(&new);
  sigprocmask(SIG_BLOCK, &new, &global_sigset);
#endif
}

void STk_allow_signals(void)  /* Restore signals as  before ignore_signals */
{
#ifdef HAVE_SIGACTION
  sigprocmask(SIG_SETMASK, &global_sigset, NULL);
#endif
}


void STk_signal_GC(void)
{
  SCM old = signals[SIGHADGC];
  
  STk_ignore_signals();
  /* Ignore this signal while executing the handler */
  signals[SIGHADGC] = Ntruth;
  execute_signal_handlers(SIGHADGC, old);
  signals[SIGHADGC] = old;
  STk_allow_signals();
}
 
/*******************************************************************************/


void STk_init_signal(void)
{
  int i;


  for (i = 0; i < MAX_SIGNAL; i++) {
    set_handler(i, (
#ifdef SIGTSTP
      (i==SIGTSTP) || /* interact. ^Z is set to SIGIGN to allow program supension */
#endif
#ifdef SIGABRT
      (i== SIGABRT) || /* Really abort when receiving  ABORT (loops otherwise)*/
#endif
#ifdef SIGQUIT
      (i == SIGQUIT) || /* ditto for QUIT */
#endif
      0) ? Truth : NIL);
  }
  /* 
   * POSIX.1 signals 
   */
#ifdef SIGABRT
  STk_define_scheme_variable("SIGABRT", STk_makeinteger(SIGABRT));
#endif
#ifdef SIGALRM
  STk_define_scheme_variable("SIGALRM", STk_makeinteger(SIGALRM));
#endif
#ifdef SIGFPE
  STk_define_scheme_variable("SIGFPE", STk_makeinteger(SIGFPE));
#endif
#ifdef SIGHUP
  STk_define_scheme_variable("SIGHUP", STk_makeinteger(SIGHUP));
#endif
#ifdef SIGILL
  STk_define_scheme_variable("SIGILL", STk_makeinteger(SIGILL));
#endif
#ifdef SIGINT
  STk_define_scheme_variable("SIGINT", STk_makeinteger(SIGINT));
#endif
#ifdef SIGKILL
  STk_define_scheme_variable("SIGKILL", STk_makeinteger(SIGKILL));
#endif
#ifdef SIGPIPE
  STk_define_scheme_variable("SIGPIPE", STk_makeinteger(SIGPIPE));
#endif
#ifdef SIGQUIT
  STk_define_scheme_variable("SIGQUIT", STk_makeinteger(SIGQUIT));
#endif
#ifdef SIGSEGV
  STk_define_scheme_variable("SIGSEGV", STk_makeinteger(SIGSEGV));
#endif
#ifdef SIGTERM
  STk_define_scheme_variable("SIGTERM", STk_makeinteger(SIGTERM));
#endif
#ifdef SIGUSR1
  STk_define_scheme_variable("SIGUSR1", STk_makeinteger(SIGUSR1));
#endif
#ifdef SIGUSR2
  STk_define_scheme_variable("SIGUSR2", STk_makeinteger(SIGUSR2));
#endif

  /* 
   * Following signals exist only on system which support Job Control 
   */
#ifdef SIGCHLD
  STk_define_scheme_variable("SIGCHLD", STk_makeinteger(SIGCHLD));
#endif
#ifdef SIGCONT
  STk_define_scheme_variable("SIGCONT", STk_makeinteger(SIGCONT));
#endif
#ifdef SIGSTOP
  STk_define_scheme_variable("SIGSTOP", STk_makeinteger(SIGSTOP));
#endif
#ifdef SIGTSTP
  STk_define_scheme_variable("SIGTSTP", STk_makeinteger(SIGTSTP));
#endif
#ifdef SIGTTIN
  STk_define_scheme_variable("SIGTTIN", STk_makeinteger(SIGTTIN));
#endif
#ifdef SIGTTOU
  STk_define_scheme_variable("SIGTTOU", STk_makeinteger(SIGTTOU));
#endif

  /*
   * Non POSIX signals stolen on Sun and Linux
   */

#ifdef SIGTRAP
  STk_define_scheme_variable("SIGTRAP", STk_makeinteger(SIGTRAP));
#endif
#ifdef SIGIOT
  STk_define_scheme_variable("SIGIOT", STk_makeinteger(SIGIOT));
#endif
#ifdef SIGEMT
  STk_define_scheme_variable("SIGEMT", STk_makeinteger(SIGEMT));
#endif
#ifdef SIGBUS
  STk_define_scheme_variable("SIGBUS", STk_makeinteger(SIGBUS));
#endif
#ifdef SIGSYS
  STk_define_scheme_variable("SIGSYS", STk_makeinteger(SIGSYS));
#endif
#ifdef SIGURG
  STk_define_scheme_variable("SIGURG", STk_makeinteger(SIGURG));
#endif
#ifdef SIGCLD
  STk_define_scheme_variable("SIGCLD", STk_makeinteger(SIGCLD));
#endif
#ifdef SIGIO
  STk_define_scheme_variable("SIGIO", STk_makeinteger(SIGIO));
#endif
#ifdef SIGPOLL
  STk_define_scheme_variable("SIGPOLL", STk_makeinteger(SIGPOLL));
#endif
#ifdef SIGXCPU
  STk_define_scheme_variable("SIGXCPU", STk_makeinteger(SIGXCPU));
#endif
#ifdef SIGXFSZ
  STk_define_scheme_variable("SIGXFSZ", STk_makeinteger(SIGXFSZ));
#endif
#ifdef SIGVTALRM
  STk_define_scheme_variable("SIGVTALRM", STk_makeinteger(SIGVTALRM));
#endif
#ifdef SIGPROF
  STk_define_scheme_variable("SIGPROF", STk_makeinteger(SIGPROF));
#endif
#ifdef SIGWINCH
  STk_define_scheme_variable("SIGWINCH", STk_makeinteger(SIGWINCH));
#endif
#ifdef SIGLOST
  STk_define_scheme_variable("SIGLOST", STk_makeinteger(SIGLOST));
#endif

  /* Add GC signal */
  STk_define_scheme_variable("SIGHADGC", STk_makeinteger(SIGHADGC));
}
