/*
 *
 * t o p l e v e l . c				-- The REP loop
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
 *	     Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date:  6-Apr-1994 14:46
 * Last file update: 22-Sep-1996 23:48
 */

#include "stk.h"
#include "gc.h"

/* The cell representing NIL */
static struct obj VNIL	   = {0, tc_nil};


static void print_banner(void)
{
  if (VCELL(Intern(PRINT_BANNER)) != Ntruth){
    fprintf(STk_stderr, "Welcome to the STk interpreter version %s [%s]\n", 
	    STK_VERSION, MACHINE);
    fprintf(STk_stderr, "Copyright © 1993-1996 Erick Gallesio - ");
    fprintf(STk_stderr, "I3S - CNRS / ESSI <eg@unice.fr>\n");
  }
}

static void weird_dirs(char *argv0)
{
  STk_panic("Could not find the directory where STk was installed.\nPerhaps some directories don't exist, or current executable (\"%s\") is in a strange place.\nYou should consider to set the \"STK_LIBRARY\" shell variable.\nExecution is aborted.", argv0);
}

static void load_init_file(void)
{
  /* Try to load init.stk in "." and, if not present, in $STK_LIBRARY/STk */
  char init_file[] = "init.stk";
  char file[2*MAX_PATH_LENGTH];

  sprintf(file, "./%s", init_file);
  if (STk_loadfile(file, 0) == Truth) return;

  sprintf(file, "%s/STk/%s", STk_library_path, init_file);
  if (STk_loadfile(file, 0) == Ntruth)
    weird_dirs(STk_Argv0);
}

static void init_library_path(char *argv0)
{
  char *s;

  STk_library_path = "";

  if (s = getenv("STK_LIBRARY")) {
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

static void init_interpreter(void)
{
#ifdef WIN32
  /* First initialize the IO system, to have a console on Windows */
  STk_init_io();
#endif

  /* Remember if we are running the stk or snow interpreter */
#ifdef USE_TK
  STk_snow_is_running = FALSE;
#else
  STk_snow_is_running = TRUE;
#endif

  /* Global variables to initialize */
  NIL		   = &VNIL;
  STk_tkbuffer	   = (char *) must_malloc(TKBUFFERN+1);
  STk_interactivep = STk_arg_interactive ||isatty(fileno(STk_stdin));

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
  VCELL(Intern(GC_VERBOSE))	= Ntruth;
  VCELL(Intern(REPORT_ERROR))	= NIL;

  VCELL(Intern(LOAD_SUFFIXES))	= NIL;
  VCELL(Intern(LOAD_PATH))	= NIL;
  VCELL(Intern(LOAD_VERBOSE))	= Ntruth;

  /* Initialize the path of the library */
  init_library_path(STk_Argv0);

  /* Initialize *eval-hook* */
  STk_init_eval_hook();

  /* Initialize standard ports */
  STk_init_standard_ports();

  /* Initialize Scheme primitives */
  STk_init_primitives();

  /* Initialize signal table */
  STk_init_signal();

  /* initialize STk_wind_stack and protect it against garbage colection */
  STk_wind_stack = NIL;	 STk_gc_protect(&STk_wind_stack);
}

static void finish_initialisation(void)
{
  /* 
   * Initialize user extensions 
   */
  STk_user_init();


#if (defined(USE_TK) && !defined(WIN32))
  /*  
   * Set up a handler for characters coming from stdin (Verify that we are
   * interactive, because background interpreters are locked)
   */
  if (STk_interactivep) {
    Tcl_File f = Tcl_GetFile((ClientData) fileno(STk_stdin),  TCL_UNIX_FD);
    
    Tcl_CreateFileHandler(f,
			  TCL_READABLE,
			  (Tk_FileProc *) STk_StdinProc, 
			  (ClientData) NULL);
  }
#endif
  
  /* 
   * See if we have the '-file' option
   */
  if (STk_arg_file) {
    STk_set_signal_handler(STk_makeinteger(SIGINT), Truth);
    STk_interactivep = 0;
    STk_loadfile(STk_arg_file, TRUE);
#ifdef USE_TK
    if (Tk_initialized) Tk_MainLoop();
#endif
    exit(0);
  }
  
  /* 
   * See if we've used the '-interactive' option; if so, 
   *		- unbufferize stdout and  stderr so that the interpreter can
   *		  be used with Emacs and 
   *		- print the STk banner
   */
  if (STk_interactivep) {
    static char *out, *err;

    out = STk_line_bufferize_io(STk_stdout);
    err = STk_line_bufferize_io(STk_stderr);
    print_banner();
  }
  fflush(stdout);

  /* 
   * Manage -load option 
   */
  if (STk_arg_load) {
    STk_loadfile(STk_arg_load, TRUE);
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

    if (STk_interactivep) {
      fprintf(STk_stderr, "STk> ");
      fflush(STk_stderr); 
      fflush(STk_stdout);	/* This is for Ilisp users */
    }
    if (EQ(x=STk_readf(STk_stdin, FALSE), STk_eof_object)) return;
    x = STk_eval(x, NIL);
    if (STk_dumped_core) {
      /* 
       * When restoring an image we arrive here x contains the result of applying
       * the saved continuation.
       */
      STk_dumped_core = 0;
      longjmp(*Top_jmp_buf, JMP_RESTORE);
    }
    else {
      STk_print(x, STk_curr_oport, WRT_MODE);
      Putc('\n', STk_stdout);
    }
  }
}

static void repl_driver(int argc, char **argv)
{
  static int k;
  static char **new_argv;
  
  new_argv = STk_process_argc_argv(argc, argv);

  if (STk_arg_image) {
    STk_save_unix_args_and_environment(argc, argv);
    STk_restore_image(STk_arg_image);
  }
  else {
    /* Normal initialisation */
    STk_reset_eval_stack();
  }

  /* Point where we come back on errors, image restoration, ... */
  k = setjmp(*Top_jmp_buf);
  
  Error_context	     = ERR_OK;	
  STk_sigint_counter = 0;
  STk_control_C	     = 0;

  switch (k) {
    case 0:		init_interpreter();
			STk_initialize_scheme_args(new_argv);
			load_init_file();
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
			if (!STk_arg_no_tk && STk_arg_Xdisplay)
			  Tk_main(STk_arg_sync,
				  STk_arg_name,
				  STk_arg_file,
				  STk_arg_Xdisplay,
				  STk_arg_geometry);
#  endif
#endif
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
  }

  repl_loop();
  if (STk_interactivep) fprintf(STk_stderr, "Bye.\n");
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
