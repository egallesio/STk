/*
 *
 *  p o r t . c			-- ports implementation
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
 * $Id: port.c 1.6 Wed, 22 Apr 1998 21:52:02 +0000 eg $
 *
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 17-Feb-1993 12:27
 * Last file update: 22-Apr-1998 11:44
 *
 */
#ifndef WIN32
#  include <sys/ioctl.h>
#  include <sys/time.h>
#  include <ctype.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>	/* This seems to be useful only for AIX */
#endif

#ifndef NO_FD_SET
#   define SELECT_MASK fd_set
#else
#   ifndef _AIX
	typedef long fd_mask;
#   endif
#   if defined(_IBMR2)
#	define SELECT_MASK void
#   else
#	define SELECT_MASK int
#   endif
#endif

#include "stk.h"
#include "module.h"

#ifdef WIN32
  /* Provide substitute functions dor WIN32 */
  FILE *popen(char *cmd, char *mode) 
  {	
    /* Returning NULL will yield an error */
    return NULL;
  }
  void pclose(FILE *f)
  {}
#endif

/* external vars */
SCM STk_curr_iport, STk_curr_oport, STk_curr_eport, STk_eof_object;


SCM STk_Cfile2port(char *name, FILE *f, int type, int flags)
{
  SCM z;

  NEWCELL(z, type);
  z->storage_as.port.p   = (struct port_descr *) 
    				must_malloc(sizeof(struct port_descr));
  PORT_FILE(z)           = f;
  PORT_FLAGS(z)	 	 = flags;
  PORT_REVENT(z)	 = Ntruth;
  PORT_WEVENT(z)	 = Ntruth;
  PORT_NAME(z)           = (char *) must_malloc(strlen(name)+1);
  strcpy(PORT_NAME(z), name);

  return z;
}

static SCM makeport(char *name, int type, char *mode, int error)
{
  SCM z     = Ntruth;
  int flags = 0;
  FILE *f;
  char *full_name;

  STk_disallow_sigint();
  if (strncmp(name, "| ", 2)) {
    full_name = CHARS(STk_internal_expand_file_name(name));

    if ((f = fopen(full_name, mode)) == NULL) {
      if (error) Err("could not open file", STk_makestring(name));
      else goto Out;
    }
  }
  else {
    full_name = name;
    flags     = PIPE_PORT;
    if ((f = popen(name+1, mode)) == NULL) {
      if (error) Err("could not create pipe", STk_makestring(name));
      else goto Out;
    }    
  }

  z = STk_Cfile2port(full_name, f, type, flags);

Out:
  STk_allow_sigint();
  return(z);
}


static SCM verify_port(char *who, SCM port, int mode)
{
  char buff[100];

  if (port == UNBOUND)     /* test write 'cause of flush */
    port = (mode&F_WRITE) ? STk_curr_oport: STk_curr_iport; 

  if (!(INP(port) || OUTP(port))) {
    sprintf(buff, "%s: bad port", who);
    Err(buff, port);
  }
  if (PORT_FLAGS(port) & PORT_CLOSED) {
    sprintf(buff, "%s: port is closed", who);
    Err(buff, port);
  }
  if ((mode & F_READ)  && INP(port))  return port; /* not else. It can be both */
  if ((mode & F_WRITE) && OUTP(port)) return port;
Error:
  sprintf(buff, "%s: bad port", who);
  Err(buff, port);
}

static void closeport(SCM port)
{
  if (PORT_FLAGS(port) & PORT_CLOSED) return;

  STk_disallow_sigint();
  
  if (IPORTP(port) || OPORTP(port)) {			    /* Not a string port */
    FILE *f = PORT_FILE(port);
#ifdef USE_TK
    /* For pipe and file ports, delete the fileevent associated to it (if any) */
    Tcl_DeleteFileHandler(fileno(f));
#endif
    fflush(f);
    if (PORT_FLAGS(port) & PIPE_PORT)   		    /* Pipe port */
      pclose(f);
    else 						    /* File port */
      fclose(f);
  }
  PORT_FLAGS(port) |= PORT_CLOSED;
  STk_allow_sigint();
}

void STk_freeport(SCM port)
{
  STk_disallow_sigint();
  closeport(port);
  free(PORT_NAME(port));
  free(port->storage_as.port.p);
  STk_allow_sigint();
}

void STk_init_standard_ports(void)
{
  STk_curr_iport = STk_Cfile2port("*stdin*",  STk_stdin, tc_iport, 0);  
  STk_gc_protect(&STk_curr_iport);
  
  STk_curr_oport = STk_Cfile2port("*stdout*", STk_stdout, tc_oport, 0);
  STk_gc_protect(&STk_curr_oport);

  STk_curr_eport = STk_Cfile2port("*stderr*", STk_stderr, tc_oport, 0);
  STk_gc_protect(&STk_curr_eport);

  
  NEWCELL(STk_eof_object, tc_eof);
  STk_gc_protect(&STk_eof_object);

  STk_line_counter = 1;
  STk_current_filename = UNBOUND;	/* Ubound <=> stdin */
  STk_gc_protect(&STk_current_filename);
}

/******************************************************************************
 *
 * L O A D  stuff
 *
 ******************************************************************************/
static int do_load(char *full_name, SCM module)
{
  FILE *f;
  int c;

  if (!STk_dirp(full_name)) {
     f = fopen(full_name, "r");
     
     if (f == NULL) return 0;

     if (STk_lookup_variable(LOAD_VERBOSE, NIL) != Ntruth)
       fprintf(STk_stderr, ";; Loading file \"%s\"\n", full_name);
     
     /* Just read one character. Assume that file is an object if this 
      * character is a control one. Here, I don't try to see if the file magic 
      * number has a particular value, since I'm not nure that all platforms
      * use identical conventions 
      */
     c = Getc(f); Ungetc(c, f);
     if (c != EOF &&  ((iscntrl(c)&& c!= '\n') || !isascii(c))) {
       fclose(f);
       STk_load_object_file(full_name);
     }
     else {
       /* file seems not to be an object file. Try to load it as a Scheme file */
       jmp_buf jb, *prev_jb = Top_jmp_buf;
       long prev_context    = Error_context;
       SCM prev_module;
       SCM previous_file, form;
       int k, previous_line;

       /* Save info about current line and file */
       previous_file	    = STk_current_filename;
       previous_line	    = STk_line_counter;
       STk_line_counter     = 1;
       STk_current_filename = STk_makestring(full_name);

       /* Save info about selected module */
       prev_module	    = STk_selected_module;
       STk_selected_module  = module;

       /* save normal error jmpbuf so that eval error don't lead to toplevel */
       /* This permits to close the opened file in case of error */
       /* If in a "catch", keep the ERR_IGNORED bit set */
       if ((k = setjmp(jb)) == 0) {
	 Top_jmp_buf   = &jb;

	 for( ; ; ) {
	   form = STk_readf(f, FALSE);
	   if EQ(form, STk_eof_object) break;
	   STk_eval(form, MOD_ENV(STk_selected_module));
	 }
       }
       fclose(f);

       Top_jmp_buf   	   = prev_jb;
       Error_context 	   = prev_context;
       STk_selected_module = prev_module;

       if (k) /*propagate error */ longjmp(*Top_jmp_buf, k);

       /* No error: restore info about current line and file */
       STk_current_filename = previous_file;
       STk_line_counter	    = previous_line;
     }
     if (STk_lookup_variable(LOAD_VERBOSE, NIL) != Ntruth)
       fprintf(STk_stderr, ";; File \"%s\" loaded\n", full_name);
     return 1;
  }
  /* No file found */
  return 0;
}

static int try_loadfile(char *prefix, char *fname, SCM suffixes, SCM module)
{
  char full_name[MAX_PATH_LENGTH], *s;

  /* First try to load without suffix */
  if (strlen(prefix) + strlen(fname) + 2 >= MAX_PATH_LENGTH) goto TooLong;
  sprintf(full_name, "%s%s%s", prefix, (*prefix ? "/": ""), fname);
  
  if (do_load(full_name, module)) return 1;

  /* Now try to load file with suffix */
  for ( ; NNULLP(suffixes); suffixes = CDR(suffixes)) {
    /* We are sure that suffixes is a well formed list (ensured by loadfile) */
    if (NSTRINGP(CAR(suffixes))) Err("load: bad suffix component", CAR(suffixes));
    s = CHARS(CAR(suffixes));

    if (strlen(prefix)+strlen(fname)+strlen(s)+3 >= MAX_PATH_LENGTH) goto  TooLong;
    sprintf(full_name, "%s%s%s.%s", prefix, (*prefix ? "/": ""), fname, s);

    if (do_load(full_name, module)) return 1;
  }
  
  /* No file loaded */
  return 0;

TooLong:
    Err("load: Filename too long", NIL);
}

SCM STk_load_file(char *fname, int err_if_absent, SCM module)
{
  int len;
  SCM load_path, load_suffixes;

  len           = strlen(fname);
  load_path     = STk_lookup_variable(LOAD_PATH,     NIL);
  load_suffixes = STk_lookup_variable(LOAD_SUFFIXES, NIL);
  
  if (STk_llength(load_path)<0)     Err("load: bad loading path", load_path);
  if (STk_llength(load_suffixes)<0) Err("load: bad set of suffixes", load_suffixes);
 
#ifdef WIN32
  if ((len > 0 && (fname[0] == '/' || fname[0] == '\\' || fname[0] == '~')) ||
      (len > 1 && fname[0] == '.' && (fname[1] == '/' || fname[1] == '\\')) ||
      (len > 2 && fname[0] == '.' && fname[1] == '.' && (fname[2] == '/' ||
							 fname[2]=='\\'))   ||
      (len > 1 && isalpha(fname[0]) && fname[1]==':')) {
#else
  if ((len > 0 && (fname[0] == '/' || fname[0] == '~')) ||
      (len > 1 && fname[0] == '.' && fname[1] == '/') ||
      (len > 2 && fname[0] == '.' && fname[1] == '.' && fname[2] == '/')) {
#endif
    
    if (fname[0] == '~') 
      fname = CHARS(STk_internal_expand_file_name(fname));

    if (try_loadfile("", fname, load_suffixes, module))
      return(err_if_absent? UNDEFINED: Truth);
  }
  else {
    /* Use *load-path* for loading file */
    for ( ; NNULLP(load_path); load_path = CDR(load_path)) {
      if (NSTRINGP(CAR(load_path))) 
	Err("load: bad loading path component", CAR(load_path));

      if (try_loadfile(CHARS(CAR(load_path)), fname, load_suffixes, module))
	return(err_if_absent? UNDEFINED: Truth);
    }
  }

    /* If we are here, we have been unable to load a file. Report err if needed */
  if (err_if_absent)
    Err("load: cannot open file", STk_makestring(fname));
  return Ntruth; 
}

PRIMITIVE STk_input_portp(SCM port)
{
  return IPORTP(port)? Truth: Ntruth;
}

PRIMITIVE STk_output_portp(SCM port)
{
  return OPORTP(port)? Truth: Ntruth;
}

PRIMITIVE STk_current_input_port(void)
{
  return STk_curr_iport;
}

PRIMITIVE STk_current_output_port(void)
{
  return STk_curr_oport;
}

PRIMITIVE STk_current_error_port(void)
{
  return STk_curr_eport;
}

PRIMITIVE STk_with_input_from_file(SCM string, SCM thunk)
{
  jmp_buf env, *prev_env = Top_jmp_buf;
  SCM result, prev_iport = STk_curr_iport;
  int prev_context 	 = Error_context;
  int k;

  if (NSTRINGP(string))     Err("with-input-from-file: bad string", string);
  if (!STk_is_thunk(thunk)) Err("with-input-from-file: bad thunk", thunk);

  STk_curr_iport = UNBOUND; 	/* will not be changed if opening fails */

  if ((k = setjmp(env)) == 0) {
    Top_jmp_buf     = &env;
    STk_curr_iport  = makeport(CHARS(string), tc_iport, "r", TRUE);
    result          = Apply(thunk, NIL);
  }
  /* restore normal error jmpbuf  and current input port*/
  if (STk_curr_iport != UNBOUND) closeport(STk_curr_iport);
  STk_curr_iport = prev_iport;
  Top_jmp_buf    = prev_env;
  Error_context  = prev_context;

  if (k) /*propagate error */ longjmp(*Top_jmp_buf, k);
  return result;
}

PRIMITIVE STk_with_output_to_file(SCM string, SCM thunk)
{
  jmp_buf env, *prev_env = Top_jmp_buf;
  SCM result, prev_oport = STk_curr_oport;
  int prev_context       = Error_context;
  int k;

  if (NSTRINGP(string))     Err("with-output-to-file: bad string", string);
  if (!STk_is_thunk(thunk)) Err("with-output-to-file: bad thunk", thunk);

  STk_curr_oport = UNBOUND;		/* will not be changed if opening fails */

  if ((k = setjmp(env)) == 0) {
    Top_jmp_buf     = &env;
    STk_curr_oport  = makeport(CHARS(string), tc_oport, "w", TRUE);
    result          = Apply(thunk, NIL);
  }
  /* restore normal error jmpbuf  and current output port*/
  if (STk_curr_oport != UNBOUND) closeport(STk_curr_oport);
  STk_curr_oport = prev_oport;
  Top_jmp_buf    = prev_env;
  Error_context  = prev_context;

  if (k) /*propagate error */ longjmp(*Top_jmp_buf, k);
  return result;
}


PRIMITIVE STk_open_input_file(SCM filename)
{
  if (NSTRINGP(filename)) Err("open-input-file: bad file name", filename);
  return makeport(CHARS(filename), tc_iport, "r", TRUE);
}

PRIMITIVE STk_open_output_file(SCM filename)
{
  if (NSTRINGP(filename)) Err("open-output-file: bad file name", filename);
  return makeport(CHARS(filename), tc_oport, "w", TRUE); 
}

PRIMITIVE STk_close_input_port(SCM port)
{
  if (!INP(port)) Err("close-input-port: not an input port", port);
  closeport(port);

  return UNDEFINED;
}

PRIMITIVE STk_close_output_port(SCM port)
{
  if (!OUTP(port)) Err("close-output-port: not an output port", port);
  closeport(port);

  return UNDEFINED;
}

PRIMITIVE STk_read(SCM port)
{
  port = verify_port("read", port, F_READ);
  return(STk_readf(PORT_FILE(port), FALSE));
}

PRIMITIVE STk_read_char(SCM port)
{
  int c;

  port = verify_port("read-char", port, F_READ);
  c = Getc(PORT_FILE(port));
  return (c == EOF) ? STk_eof_object : STk_makechar((unsigned char) c);
}

PRIMITIVE STk_peek_char(SCM port)
{
  int c;

  port = verify_port("peek-char", port, F_READ);
  c = Getc(PORT_FILE(port));
  Ungetc(c, PORT_FILE(port));
  return (c == EOF) ? STk_eof_object : STk_makechar(c);
}

PRIMITIVE STk_eof_objectp(SCM obj)
{
  return (obj == STk_eof_object)? Truth : Ntruth;
}

#ifdef WIN32
PRIMITIVE STk_char_readyp(SCM port) 
{
  STk_panic("Not yet implemented!");
}
#else
PRIMITIVE STk_char_readyp(SCM port) 
{ 
  port = verify_port("char-ready?", port, F_READ);
  if (Eof(PORT_FILE(port))) return Truth;
  if (ISPORTP(port)) /* !eof -> */  return Truth;
  else {
    /* First, see if characters are available in the buffer */
    if (STk_file_data_available(PORT_FILE(port)))
      return Truth;

#ifdef HAVE_SELECT
    {
      SELECT_MASK readfds;
      struct timeval timeout;
      int f = fileno(PORT_FILE(port));

      FD_ZERO(&readfds); 
      FD_SET(f, &readfds);
      timeout.tv_sec = timeout.tv_usec = 0;
      return (select(f+1, &readfds, NULL, NULL, &timeout)) ? Truth : Ntruth;
    }
#else
#  ifdef FIONREAD
   {
     int result;

     ioctl(fileno(PORT_FILE(port)), FIONREAD, &result);
     return result ? Truth : Ntruth;
   }
#  else
   return Truth;
#  endif
#endif
  }
}
#endif

PRIMITIVE STk_write(SCM expr, SCM port)
{
  port = verify_port("write", port, F_WRITE);
  STk_print(expr, port, WRT_MODE);
  return UNDEFINED;
}

PRIMITIVE STk_display(SCM expr, SCM port)
{
  port = verify_port("display", port, F_WRITE);
  STk_print(expr, port, DSP_MODE);
  return UNDEFINED;
}

PRIMITIVE STk_newline(SCM port)
{
  port = verify_port("newline", port, F_WRITE);
  Putc('\n', PORT_FILE(port));
  return UNDEFINED;
}

PRIMITIVE STk_write_char(SCM c, SCM port)
{
  if (NCHARP(c)) Err("write-char: not a character", c);
  port = verify_port("write-char", port, F_WRITE);
  Putc(CHAR(c), PORT_FILE(port));
  return UNDEFINED;
}

PRIMITIVE STk_load(SCM filename, SCM module)
{
  ENTER_PRIMITIVE("load");
  
  if (NSTRINGP(filename)) Serror("bad file name", filename);
  if (module != UNBOUND) {
    if (NMODULEP(module)) Serror("bad module", module);
  }
  else
    module = STk_selected_module;
  
  return STk_load_file(CHARS(filename), TRUE,  module);
}


/*
 *
 * STk bonus
 *
 */

static SCM internal_format(SCM l,int len,int error)/* a very simple and poor one */ 
{
  SCM port, fmt;
  int format_in_string = 0;
  char *p;
  FILE *f;

  if (error) {
    if (len < 1) Err("error: Bad list of parameters", l);
    format_in_string = 1;
    port = STk_open_output_string();
    len -= 1;
  }
  else {
    if (len < 2) Err("format: Bad list of parameters", l);
    port = CAR(l); l = CDR(l);
    len -= 2;
  }
  fmt  = CAR(l); l = CDR(l);

  if (BOOLEANP(port)){
    if (port == Truth) port = STk_curr_oport;
    else {
      format_in_string = 1;
      port= STk_open_output_string();
    }
  }
  
  verify_port(error? "error": "format", port, F_WRITE);
  if (NSTRINGP(fmt)) Err("format: bad format string", fmt);

  f = PORT_FILE(port);

  for(p=CHARS(fmt); *p; p++) {
    if (*p == '~') {
      switch(*(++p)) {
        case 'S':
        case 's':
        case 'A':
        case 'a': if (len-- > 0) {
                    STk_print(CAR(l), 
			      port, 
			      (tolower(*p) == 's')? WRT_MODE: DSP_MODE);
                    l = CDR(l);
                  }
                  else Err("format: too much ~ in format string", l); 
	          continue;
        case '%': Putc('\n', f);
                  continue;
        case '~': Putc('~', f);
                  continue;
        default:  Putc('~',  f);
                  /* NO BREAK */
      }
    }
    Putc(*p, f);
  }

  if (NNULLP(l)) Err("format: too few ~ in format string", l);

  return format_in_string ? STk_get_output_string(port) : UNDEFINED;
}

PRIMITIVE STk_format(SCM l, int len)
{
  return internal_format(l, len, FALSE);
}

PRIMITIVE STk_error(SCM l, int len)
{
  /* Set context to ERR_OK but keep the bit indicating if error must be caught */
  Error_context = ERR_OK | (Error_context & ERR_IGNORED);

  Err(CHARS(internal_format(l, len, TRUE)), NIL);
  return UNDEFINED; 	/* for compiler */
}

PRIMITIVE STk_try_load(SCM filename, SCM module)
{
  ENTER_PRIMITIVE("try-load");
  
  if (NSTRINGP(filename)) Serror("bad file name", filename);
  if (module != UNBOUND) {
    if (NMODULEP(module)) Serror("bad module", module);
  }
  else
    module = STk_selected_module;
  
  return STk_load_file(CHARS(filename), FALSE,  module);
}

PRIMITIVE STk_open_file(SCM filename, SCM mode)
{
  int type;
  
  if (NSTRINGP(filename)) Err("open-file: bad file name", filename); 
  if (NSTRINGP(mode) || CHARS(mode)[1] != '\0') goto Error;
  
  switch (CHARS(mode)[0]) {
    case 'a': 
    case 'w': type = tc_oport; break;
    case 'r': type = tc_iport; break;
    default:  ;
Error:	      Err("open-file: bad mode", mode);
  }
  return(makeport(CHARS(filename), type, CHARS(mode), FALSE));
}

PRIMITIVE STk_close_port(SCM port)
{
  if (INP(port) || OUTP(port)) closeport(port);
  else Err("close-port: bad port", port);
  return UNDEFINED;
}

PRIMITIVE STk_read_line(SCM port)
{
  FILE *f;
  int c, i, size = 128;
  char *buff = (char *) must_malloc(size);
  SCM res;

  port = verify_port("read-line", port, F_READ);
  f = PORT_FILE(port);
  for (i = 0; ; i++) {
    switch (c = Getc(f)) {
      case EOF:  if (i == 0) { free(buff); return STk_eof_object; }
      case '\r': i--; continue;
      case '\n': res = STk_makestrg(i, buff); free(buff); return res;
      default:   if (i == size) {
	           size += size / 2;
		   buff = must_realloc(buff, size);
		 }
	         buff[i] = c;
    }
  }
}

PRIMITIVE STk_flush(SCM port)
{
  int code;

  port = verify_port("flush", port, F_WRITE|F_READ);

  if (! SPORTP(port)) {
    if (fflush(PORT_FILE(port)) == EOF)
      Err("flush: cannot flush buffer", port);
  }

  return UNDEFINED;
}

PRIMITIVE STk_write_star(SCM expr, SCM port)
{
  port = verify_port("write*", port, F_WRITE);
  STk_print_star(expr, port);
  return UNDEFINED;
}

/******************************************************************************
 *
 * Autoload stuff
 *
 ******************************************************************************/

static SCM list_of_files = NULL;
static int dont_do_autoload = 0;	/* 1 if we are testing autoload? */ 

static SCM make_autoload(SCM file, SCM env)
{
  SCM z;
  
  NEWCELL(z, tc_autoload);
  CAR(z) = file;
  CDR(z) = env;
  return z;
}

void STk_do_autoload(SCM var, SCM autoload)
{
  static int recursive_call = 0;

  if (dont_do_autoload) return;

  if (recursive_call) {
    /* We have a recursive call if var has not be defined in the specified
     * file. In effect when a file has been loaded, we just try to find
     * the value of var. If this value has not been defined we will do 
     * another STk_do_autoload
     */
    recursive_call = 0;
    Err("autoload: symbol was not defined", var);
  }

  {
    SCM file    	= CAR(autoload);
    SCM module  	= CDR(autoload);
    SCM loaded;

    /* Retain in a list, files which are currently autoloaded to avoid mult. load */
    if (!list_of_files) {
      list_of_files = NIL;
      STk_gc_protect(&list_of_files);
    }
    
    if (STk_member(file, list_of_files) != Ntruth) return;
    list_of_files = Cons(file, list_of_files);
    
    /* Load file in the module used when the symbol was defined as autoload */
    loaded        = STk_load_file(CHARS(file), FALSE, module);
    list_of_files = CDR(list_of_files);
    
    /* Verify that file was really loaded  (loaded is true in this case) */
    if (loaded == Ntruth)
      Err("autoload: file not found for autoload symbol", Cons(var, file));
    
    /* File is now loaded. Try to lookup the value of var and see if it
     * provokes another STk_do_autoload call
     */
    recursive_call = 1;
    STk_varlookup(var, MOD_ENV(module), TRUE);
    recursive_call = 0;
  }
}

PRIMITIVE STk_autoload(SCM l, SCM env, int len)
{
  SCM file, current_module;

  ENTER_PRIMITIVE("autoload");
  if (len < 2) Serror("bad parameter list", l);
  file = CAR(l);
  if (NSTRINGP(file)) Serror("bad file name", file);

  current_module = STk_current_module(NIL, env, 0);

  for (l = CDR(l); NNULLP(l); l = CDR(l)) {
    if (NSYMBOLP(CAR(l))) Serror("bad symbol", CAR(l));
    STk_define_public_var(current_module, 
			  CAR(l), 
			  make_autoload(file, current_module));
  }
  return UNDEFINED;
}

PRIMITIVE STk_autoloadp(SCM symbol, SCM module)
{
  SCM *value, env;
  
  ENTER_PRIMITIVE("autoload?");
  
  if (NSYMBOLP(symbol)) Serror("bad symbol", symbol); 
  if (module == UNBOUND) 
    env = STk_global_module;
  else {
    if (NMODULEP(module)) Serror("bad module", module);
    env = MOD_ENV(module);
  }

  /* Looking at var value will load the file. Signal that we don't want to load */
  /* This is a little bit hacky, but it does the job */
  dont_do_autoload = 1;
  value 	   = STk_varlookup(symbol, env, FALSE);
  dont_do_autoload = 0;

  return TYPEP(*value, tc_autoload) ? Truth: Ntruth;
}

#ifdef USE_TK
/******************************************************************************
 *
 * Port event management
 *
 ******************************************************************************/

static void apply_file_closure(SCM closure)
{
  Apply(closure, NIL);
}
 

static SCM when_port_ready(SCM port, SCM closure, char *name, int mode)
{
  char str[50];
  int fd;

  if (NIPORTP(port) && NOPORTP(port)) {
    sprintf(str, "%s: bad port", name);
    STk_err(str, port);
  }

  if (closure == UNBOUND) {
    /* Return the current handler closure */
    return ((mode == TCL_READABLE)? PORT_REVENT(port): PORT_WEVENT(port));
  }

  fd = fileno(PORT_FILE(port));
  
  if (closure == Ntruth) {
    Tcl_DeleteFileHandler(fd);    
    if (mode == TCL_READABLE)
      PORT_REVENT(port) = Ntruth;
    else
      PORT_WEVENT(port) = Ntruth;
  }
  else {
    if (STk_procedurep(closure) == Ntruth) {
      sprintf(str, "%s: bad closure", name);
      STk_err(str, closure);
    }

    /* It is not necessary to mark the closure in the Tcl tables since it is
     * also pointed by the Scheme port. This prevent GC problems
     */
    Tcl_CreateFileHandler(fd, mode, (Tcl_FileProc *) apply_file_closure, 
			  (ClientData) closure);
    if (mode == TCL_READABLE)
      PORT_REVENT(port) = closure;
    else
      PORT_WEVENT(port) = closure;
  }
  return UNDEFINED;
}
  

PRIMITIVE STk_when_port_readable(SCM port, SCM closure)
{
  return when_port_ready(port, closure, "when-port-readable", TCL_READABLE);
}


PRIMITIVE STk_when_port_writable(SCM port, SCM closure)
{
  return when_port_ready(port, closure, "when-port-writable", TCL_WRITABLE);
}
#endif
