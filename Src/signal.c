/*
 *
 * s i g n a l . c			-- Signal handling
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 10-Oct-1995 07:55
 * Last file update: 23-Sep-1996 22:47
 *
 */

#include "stk.h"
#include <signal.h>

#ifdef HAVE_SIGACTION
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
  else {
    SCM arg = LIST1(STk_makeinteger((long) SIGINT));

    for ( ; CONSP(l); l = CDR(l)) {
      if (STk_apply(CAR(l), arg) == Sym_break) break;
    }
  }
}


void STk_handle_signal(int sig)
{
  SCM l, arg;

#ifndef  HAVE_SIGACTION
  if (sig < MAX_SYSTEM_SIG)
    signal(sig, STk_handle_signal);
#endif

  if (sig == SIGINT) {			/* SIGINT */
    if (STk_sigint_counter > 0) {
      /* ^C is disallowed for now. (We are probably in sequence which
       * uses malloc). Retain that a Control-C was issued and return
       */
      STk_control_C = 1;
      SET_EVAL_FLAG(1);
      return;
    }
    else handle_sigint_signal();
  }
  
  arg = LIST1(STk_makeinteger((long) sig));
  for (l = signals[sig]; CONSP(l); l = CDR(l)) {
    if (STk_apply(CAR(l), arg) == Sym_break) break;
  }
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
      
#ifdef HAVE_SIGACTION
  {
    struct sigaction sigact;
    
    sigemptyset(&(sigact.sa_mask));
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
  VCELL(Intern("SIGABRT")) = STk_makeinteger(SIGABRT);
#endif
#ifdef SIGALRM
  VCELL(Intern("SIGALRM")) = STk_makeinteger(SIGALRM);
#endif
#ifdef SIGFPE
  VCELL(Intern("SIGFPE")) = STk_makeinteger(SIGFPE);
#endif
#ifdef SIGHUP
  VCELL(Intern("SIGHUP")) = STk_makeinteger(SIGHUP);
#endif
#ifdef SIGILL
  VCELL(Intern("SIGILL")) = STk_makeinteger(SIGILL);
#endif
#ifdef SIGINT
  VCELL(Intern("SIGINT")) = STk_makeinteger(SIGINT);
#endif
#ifdef SIGKILL
  VCELL(Intern("SIGKILL")) = STk_makeinteger(SIGKILL);
#endif
#ifdef SIGPIPE
  VCELL(Intern("SIGPIPE")) = STk_makeinteger(SIGPIPE);
#endif
#ifdef SIGQUIT
  VCELL(Intern("SIGQUIT")) = STk_makeinteger(SIGQUIT);
#endif
#ifdef SIGSEGV
  VCELL(Intern("SIGSEGV")) = STk_makeinteger(SIGSEGV);
#endif
#ifdef SIGTERM
  VCELL(Intern("SIGTERM")) = STk_makeinteger(SIGTERM);
#endif
#ifdef SIGUSR1
  VCELL(Intern("SIGUSR1")) = STk_makeinteger(SIGUSR1);
#endif
#ifdef SIGUSR2
  VCELL(Intern("SIGUSR2")) = STk_makeinteger(SIGUSR2);
#endif

  /* 
   * Following signals exist only on system which support Job Control 
   */
#ifdef SIGCHLD
  VCELL(Intern("SIGCHLD")) = STk_makeinteger(SIGCHLD);
#endif
#ifdef SIGCONT
  VCELL(Intern("SIGCONT")) = STk_makeinteger(SIGCONT);
#endif
#ifdef SIGSTOP
  VCELL(Intern("SIGSTOP")) = STk_makeinteger(SIGSTOP);
#endif
#ifdef SIGTSTP
  VCELL(Intern("SIGTSTP")) = STk_makeinteger(SIGTSTP);
#endif
#ifdef SIGTTIN
  VCELL(Intern("SIGTTIN")) = STk_makeinteger(SIGTTIN);
#endif
#ifdef SIGTTOU
  VCELL(Intern("SIGTTOU")) = STk_makeinteger(SIGTTOU);
#endif

  /*
   * Non POSIX signals stolen on Suns and Linux
   */

#ifdef SIGTRAP
  VCELL(Intern("SIGTRAP")) = STk_makeinteger(SIGTRAP);
#endif
#ifdef SIGIOT
  VCELL(Intern("SIGIOT")) = STk_makeinteger(SIGIOT);
#endif
#ifdef SIGEMT
  VCELL(Intern("SIGEMT")) = STk_makeinteger(SIGEMT);
#endif
#ifdef SIGBUS
  VCELL(Intern("SIGBUS")) = STk_makeinteger(SIGBUS);
#endif
#ifdef SIGSYS
  VCELL(Intern("SIGSYS")) = STk_makeinteger(SIGSYS);
#endif
#ifdef SIGURG
  VCELL(Intern("SIGURG")) = STk_makeinteger(SIGURG);
#endif
#ifdef SIGCLD
  VCELL(Intern("SIGCLD")) = STk_makeinteger(SIGCLD);
#endif
#ifdef SIGIO
  VCELL(Intern("SIGIO")) = STk_makeinteger(SIGIO);
#endif
#ifdef SIGPOLL
  VCELL(Intern("SIGPOLL")) = STk_makeinteger(SIGPOLL);
#endif
#ifdef SIGXCPU
  VCELL(Intern("SIGXCPU")) = STk_makeinteger(SIGXCPU);
#endif
#ifdef SIGXFSZ
  VCELL(Intern("SIGXFSZ")) = STk_makeinteger(SIGXFSZ);
#endif
#ifdef SIGVTALRM
  VCELL(Intern("SIGVTALRM")) = STk_makeinteger(SIGVTALRM);
#endif
#ifdef SIGPROF
  VCELL(Intern("SIGPROF")) = STk_makeinteger(SIGPROF);
#endif
#ifdef SIGWINCH
  VCELL(Intern("SIGWINCH")) = STk_makeinteger(SIGWINCH);
#endif
#ifdef SIGLOST
  VCELL(Intern("SIGLOST")) = STk_makeinteger(SIGLOST);
#endif

  /* Add GC signal */
  VCELL(Intern("SIGHADGC")) = STk_makeinteger(SIGHADGC);
}
