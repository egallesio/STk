/*
 *
 * t o p l e v e l . c				-- The REP loop
 *
 * Copyright © 1993-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *	     Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date:  6-Apr-1994 14:46
 * Last file update:  3-Sep-1999 21:01 (eg)
 *
 * Modifications made by Steve Pruitt <steve@pruitt.net> fot Win32
 * To maintain compatability with Tcl Library paths and
 * For Win32 support the following changes were made:
 *     - Defined IMPORT_DLL_GLOBALS for dll support
 *     - Copyright symbol changed to (c)
 */

/* Import DLL globals from library */ 
#if defined(WIN32) && defined(USE_DYNLOAD) && defined(MSC_VER) && !defined(CYGWIN32)
#  define IMPORT_DLL_GLOBALS
#endif

#include "stk.h"
#include "gc.h"
#include "module.h"

#ifndef WIN32
struct error_handler *STk_err_handler;    /* Current error handler pointer */ 
#else
#  if (defined(USE_DYNLOAD) && defined(MSC_VER) && !defined(CYGWIN32))
     __declspec( dllimport ) struct error_handler *STk_err_handler;
#  else
     struct error_handler *STk_err_handler;
#  endif
#endif


static struct obj VNIL	   = {{NULL}, 0, tc_nil}; /* The cell representing NIL */

static void print_banner(void)
{
  if (STk_lookup_variable(PRINT_BANNER, NIL) != Ntruth){
    Fprintf(STk_curr_eport, "Welcome to the STk interpreter version %s [%s]\n", 
	    STK_VERSION, MACHINE);
#ifdef WIN32
    /* the copyright symbol does not work on WIN32 system */
    Fprintf(STk_curr_eport, "Copyright (c) 1993-1999 Erick Gallesio - ");
#else
    Fprintf(STk_curr_eport, "Copyright © 1993-1999 Erick Gallesio - ");
#endif
    Fprintf(STk_curr_eport, "I3S - CNRS / ESSI <eg@unice.fr>\n");
  }
}

static void weird_dirs(char *argv0)
{
  panic("Could not find the directory where STk was installed.\nPerhaps some directories don't exist, or current executable (\"%s\") is in a strange place.\nYou should consider to set the \"STK_LIBRARY\" shell variable.", argv0);
}

#ifdef USE_TK
static void no_display(char *argv0)
{
  panic("DISPLAY variable is not set. Tk cannot be initialized. Please use command line option ``-no-tk'' when executing \"%s\"", argv0);
}
#endif


static void load_init_file(void)
{
  /* Try to load init.stk in "." and, if not present, in $STK_LIBRARY/STk */
  char init_file[] = "init.stk";
  char file[2*MAX_PATH_LENGTH];

  sprintf(file, "./%s", init_file);
  if (STk_load_file(file, FALSE, STk_selected_module) == Truth) return;

  sprintf(file, "%s/STk/%s", STk_library_path, init_file);
  if (STk_load_file(file, FALSE, STk_selected_module) == Ntruth)
    weird_dirs(STk_Argv0);
}


static void load_user_init_file(void)
{
  /* Try to load .stkrc in "." and, if not present, in $HOME */
  char init_file[] = ".stkrc";
  char file[2*MAX_PATH_LENGTH];
  char *s;

  sprintf(file, "./%s", init_file);
  if (STk_load_file(file, FALSE, STk_selected_module) == Truth) return;

  if ((s = getenv("HOME")) != NULL) {
    sprintf(file, "%s/%s", s, init_file);
    STk_load_file(file, FALSE, STk_selected_module);
  }
}


static void init_library_path(char *argv0)
{
  char *s;

  STk_library_path = "";

  if ((s = getenv("STK_LIBRARY"))) {
    /* Initialize STk_library_path with the content of STK_LIBRARY 
     * shell variable.
     * Make a copy of environment variable (copy is necessary for
     * images files) 
     */
    STk_library_path = (char *) must_malloc(strlen(s) + 1); 
    strcpy(STk_library_path, s);
  }
  else {
    SCM canonical_argv0 = STk_resolve_link(argv0, 0);
    
    if (canonical_argv0 != Ntruth) {
      /* STk_library must be set to the parent directory of the executable */
      char *s, *e;

      s = CHARS(canonical_argv0);
      e = s + strlen(s) - 1;
      
      while (e > s && !ISDIRSEP(*e)) e -= 1;	/* delete exec	name */
      e -= 1;
      while (e > s && !ISDIRSEP(*e)) e -= 1;	/* delete directory name */
      *e = '\0';
      
      STk_library_path = must_malloc(strlen(s) + 1);
      strcpy(STk_library_path, s);
    }
    else weird_dirs(argv0);
  }
}

static SCM get_last_defined(char *name)
{
  return STk_last_defined;
}

static void set_last_defined(char *name, SCM val)
{
  STk_last_defined = val;
}


/* 
 * Panic procedure.
 */

static void panic_proc(char *format, ...) 
{
  va_list ap;
  char buf[1024];
  char *fmt= "\n**** Fatal error in STk:\n**** %s\n**** ABORT.\n";

  va_start(ap, format);
  vsprintf(buf, format, ap);

#ifdef WIN32
  MessageBeep(MB_ICONEXCLAMATION);
  MessageBox(NULL, buf, "Fatal error in STk", 
	     MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#else
  if (STk_curr_eport == NULL)
    fprintf(stderr, fmt, buf);		/* IO are not yet fully initialized */
  else
    Fprintf(STk_curr_eport, fmt, buf);
#endif
  exit(1);
}



static void init_interpreter(void)
{
  /* Remember if we are running the stk or snow interpreter */
#ifdef USE_TK
  STk_snow_is_running = FALSE;
#else
  STk_snow_is_running = TRUE;
#endif

  /* Global variables to initialize */
  STk_tkbuffer	   = (char *) must_malloc(TKBUFFERN+1);
  STk_is_safe	   = 0;
  NIL		   = &VNIL;
  
  /* Initialize GC */
  STk_init_gc();

  /* Initialize symbol & keyword tables */
  STk_initialize_symbol_table();
  STk_initialize_keyword_table();

  /* 
   * Define some scheme objects used by the interpreter 
   * and protect them against GC 
   */
  NEWCELL(UNDEFINED, tc_undefined); STk_gc_protect(&UNDEFINED);
  NEWCELL(UNBOUND,   tc_unbound);   STk_gc_protect(&UNBOUND);
  NEWCELL(Truth,     tc_boolean);   STk_gc_protect(&Truth);
  NEWCELL(Ntruth,    tc_boolean);   STk_gc_protect(&Ntruth);

  Sym_lambda	   = Intern("lambda");		 STk_gc_protect(&Sym_lambda);
  Sym_quote	   = Intern("quote");		 STk_gc_protect(&Sym_quote);
  Sym_imply	   = Intern("=>");		 STk_gc_protect(&Sym_imply);
  Sym_dot	   = Intern(".");		 STk_gc_protect(&Sym_dot);
  Sym_debug	   = Intern(DEBUG_MODE);	 STk_gc_protect(&Sym_debug);
  Sym_else	   = Intern("else");		 STk_gc_protect(&Sym_else);
  Sym_quasiquote   = Intern("quasiquote");	 STk_gc_protect(&Sym_quasiquote);
  Sym_unquote	   = Intern("unquote");		 STk_gc_protect(&Sym_unquote);
  Sym_unq_splicing = Intern("unquote-splicing"); STk_gc_protect(&Sym_unq_splicing);
  Sym_break	   = Intern("break");		 STk_gc_protect(&Sym_break);

  STk_globenv	   = STk_makeenv(NIL, 1);	 STk_gc_protect(&STk_globenv);

  /* GC_VERBOSE and REPORT_ERROR must ABSOLUTLY initialized before any GC occurs
   * Otherwise, they will be allocated during a GC and this lead to an infinite 
   * loop
   */
  STk_define_variable(GC_VERBOSE,    Ntruth, NIL);
  STk_define_variable(REPORT_ERROR,  NIL,    NIL);

  /* Initialize standard ports */
  STk_init_standard_ports();

  /* Initialize module system */
  STk_init_modules();

  /* Initialize the path of the library */
  init_library_path(STk_Argv0);

  /* Initialize *eval-hook* */
  STk_init_eval_hook();

  /* Initialize Scheme primitives */
  STk_init_primitives();

  /* Initialize signal table */
  STk_init_signal();

  /* Define some global variables */
  STk_define_variable(LOAD_SUFFIXES, NIL,			       NIL);
  STk_define_variable(LOAD_PATH,     NIL,			       NIL);
  STk_define_variable(LOAD_VERBOSE,  Ntruth,			       NIL);
  STk_define_variable(STK_LIBRARY,   STk_makestring(STk_library_path), NIL);

  /* Initialize C variables */
  STk_last_defined = Ntruth;
  STk_define_C_variable(LAST_DEFINED, get_last_defined, set_last_defined);
  STk_gc_protect(&STk_last_defined);
}

static void finish_initialisation(void)
{
  /* 
   * Initialize user extensions 
   */
  STk_user_init();

  /* 
   * See if we have the '-file' option
   */
  if (STk_arg_file) {
    STk_set_signal_handler(STk_makeinteger(SIGINT), Truth);
    STk_interactivep = 0;
    if (STk_load_file(STk_arg_file, FALSE, STk_selected_module) == Ntruth)
      panic("Cannot open file \"%s\".", STk_arg_file);

#ifdef USE_TK
    if (Tk_initialized){
      STk_internal_eval_string("(tk-set-error-handler!)", 0, NIL); /* Do it late */
      Tk_MainLoop();
    }
#endif
    exit(0);
  }
  else 
    STk_interactivep = STk_arg_interactive 			|| 
#ifdef USE_TK
      		       STk_arg_console 				||
#endif
      		       isatty(fileno(PORT_FILE(STk_stdin)));
      
  /* 
   * See if we are interactive:
   *   1/ Create a console if needed;
   *   2/ Unbufferize stdout and  stderr so that the interpreter can  be
   *	  used with Emacs and 
   *   3/ print the STk banner
   *   4/ set the input handler if we are on Unix
   */
  if (STk_interactivep) {
    static char *out, *err;

#ifdef USE_TK
    if (STk_arg_console) STk_init_console();
#endif
    out = STk_line_bufferize_io(STk_stdout);
    err = STk_line_bufferize_io(STk_stderr);
    print_banner();
#if (defined(USE_TK) && !defined(WIN32))
    if (!STk_arg_console) 
      /* Set up a handler for characters coming from stdin */
      Tcl_CreateFileHandler(fileno(PORT_FILE(STk_stdin)),
			    TCL_READABLE,
			    (Tk_FileProc *) STk_StdinProc, 
			    (ClientData) NULL);
#endif
  }
  Flush(STk_curr_oport);

  /* 
   * Manage -load option 
   */
  if (STk_arg_load) {
    STk_load_file(STk_arg_load, TRUE, STk_selected_module);
#ifdef USE_TK
    if (Tk_initialized) Tcl_GlobalEval(STk_main_interp, "(update)");
#endif
  }
}

static void repl_loop(void)
{
  /* The print/eval/read loop */
  for( ; ; ) {
    SCM x;
    SCM env = MOD_ENV(STk_selected_module);

    if (STk_interactivep) {
#ifdef USE_TK
      if (STk_arg_console) STk_console_prompt(env); else
#endif
      if (STk_internal_eval_string("(catch (repl-display-prompt (current-module)))",
				   0, env) == Truth)
	Fprintf(STk_curr_eport, "STk> ");
      Flush(STk_curr_oport); 
      Flush(STk_curr_eport);	/* This is for Ilisp users */
    }

    if (EQ(x=STk_readf(STk_stdin, FALSE), STk_eof_object)) return;

    x = STk_eval(x, env);

    if (STk_dumped_core) {
      /* 
       * When restoring an image we arrive here x contains the result of applying
       * the saved continuation.
       */
      STk_dumped_core = 0;
      longjmp(STk_err_handler->j, JMP_RESTORE);
    }
    else {
      if (STk_interactivep) {
	 STk_define_variable("*repl-result*", x, NIL);
	 if (STk_internal_eval_string("(catch (repl-display-result *repl-result*))",
				      0, env) == Truth) {
	   STk_print(x, STk_curr_oport, WRT_MODE);
	   Putc('\n', STk_curr_oport);
	 }
      }
    }
  }
}

static void repl_driver(int argc, char **argv)
{
  static int k;
  static char **new_argv;
  struct error_handler err_handler;

  /* Inititialize the error handler. */
  STk_err_handler     	      = &err_handler;
  err_handler.prev    	      = NULL;
  err_handler.context 	      = ERR_FATAL;
  err_handler.dynamic_handler = &VNIL;	/* since NIL is not yet initialized */
 
  /* Initialize IO and set the panic procedure */
  STk_init_io();
  Tcl_SetPanicProc(panic_proc);

  new_argv = STk_process_argc_argv(argc, argv);

  if (STk_arg_image) {
    STk_save_unix_args_and_environment(argc, argv);
    STk_restore_image(STk_arg_image);
  }
  else {
    /* Normal initialisation */
    STk_reset_eval_stack();
  }

  /* We come back here on errors, image restauration, ... */
  k = setjmp(err_handler.j); 

  err_handler.context = ERR_OK;	

  switch (k) {
    case 0:		init_interpreter();
			STk_initialize_scheme_args(new_argv);
			load_init_file();
			/* And now set the default module to the global one */
			STk_initialize_stk_module();
#ifdef USE_TK
#  ifdef WIN32
			if (!STk_arg_no_tk)
			  Tk_main(STk_arg_sync,
				  STk_arg_name,
				  STk_arg_file,
				  "localhost:0",
				  STk_arg_geometry);
#  else
			if (!STk_arg_Xdisplay) 
			  STk_arg_Xdisplay =  getenv("DISPLAY");
			if (!STk_arg_no_tk) {
			  if (!STk_arg_Xdisplay) no_display(STk_Argv0);
			  Tk_main(STk_arg_sync,
				  STk_arg_name,
				  STk_arg_file,
				  STk_arg_Xdisplay,
				  STk_arg_geometry);
			}
#  endif
#endif
			load_user_init_file();
			finish_initialisation();
			break;
    case JMP_RESTORE:	STk_restore_unix_args_and_environment(&argc, &argv);
			/* Process another time args since we have lost them ! */
			new_argv = STk_process_argc_argv(argc, argv);
			STk_initialize_scheme_args(new_argv);
#ifdef USE_TK
			if (!STk_arg_no_tk && (STk_arg_Xdisplay||getenv("DISPLAY")))
			  Tk_main(STk_arg_sync, 
				  STk_arg_name, 
				  STk_arg_file, 
				  STk_arg_Xdisplay,
				  STk_arg_geometry);
#endif
			finish_initialisation();
			break;
    case JMP_THROW:
    case JMP_ERROR:	break;
    case JMP_INTERRUPT: STk_control_C = 0;
			STk_err_handler = &err_handler;
			STk_reset_eval_stack();
			break;
  }

  repl_loop();

  if (STk_interactivep) Fprintf(STk_curr_eport, "Bye.\n");
  STk_quit_interpreter(UNBOUND);
}


/******************************************************************************
 *
 * Toplevel
 * 
 ******************************************************************************/

void STk_toplevel(int argc, char **argv)
{
  SCM stack_start; /* Unused variable. Its the first stack allocated variable */

  STk_stack_start_ptr = &stack_start;
  repl_driver(argc, argv);
}
