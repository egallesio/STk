/*
 *  p o r t . c			-- ports implementation
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 17-Feb-1993 12:27
 * Last file update:  3-Sep-1999 20:21 (eg)
 *
 * Win32 support by Steve Pruitt <steve@pruitt.net>
 * 	- Modified do_load for dynamic loading of WIN32 Dll files
 * 	- Added check for *shared-prefix* in try_loadfile 
 * 	- Added "#define strcasecmp stricmp" for Win32 *shared-prefix* support
 * 	- Modified STk_open_file to allow Win32 modes "wb" and "rb"
 *
 */

#include "stk.h"
#include "module.h"


#define INITIAL_LINE_SIZE 256		/* Initial size for readline */


#if defined(WIN32) && !defined(CYGWIN32)
#  define strcasecmp stricmp

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
  PORT_UNGETC(z)	 = EOF;
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
  int flags = 0;
  FILE *f;
  char *full_name;

#ifdef WIN32
  /* Transform name in an absolute name */
  name = CHARS(STk_internal_expand_file_name(name));
#endif

  if (strncmp(name, "| ", 2)) {
    full_name = CHARS(STk_internal_expand_file_name(name));

    if ((f = fopen(full_name, mode)) == NULL) {
      if (error) Err("could not open file", STk_makestring(name));
      else return Ntruth;
    }
  }
  else {
    full_name = name;
    flags     = PIPE_PORT;
    if ((f = popen(name+1, mode)) == NULL) {
      if (error) Err("could not create pipe", STk_makestring(name));
      else return Ntruth;
    }    
  }

  return STk_Cfile2port(full_name, f, type, flags);
}


static SCM verify_port(char *proc_name, SCM port, int mode)
{
  if (port == UNBOUND)     /* test write 'cause of flush */
    port = (mode&F_WRITE) ? STk_curr_oport: STk_curr_iport; 

  if (!(INP(port) || OUTP(port)))     Serror("bad port", port);
  if (PORT_FLAGS(port) & PORT_CLOSED) Serror("port is closed", port);

  if ((mode & F_READ)  && INP(port))  return port; /* not else. It can be both */
  if ((mode & F_WRITE) && OUTP(port)) return port;
  Serror("bad port", port);
  return UNDEFINED; /* cannot occur */
}

void STk_close_file_port(SCM port)
{
  FILE *f = PORT_FILE(port);
  
#if defined(USE_TK) && !defined(WIN32)
  /* For pipe and file ports, delete the fileevent associated to it (if any) */
  Tcl_DeleteFileHandler(fileno(f));
#endif
  if (PORT_FLAGS(port) & PIPE_PORT)   		    /* Pipe port */
    pclose(f);
  else { 					    /* File port */
    /* FIXME: Normally close should suffice but glibc2.1.1 on Linux dumps 
     * core if a file is closed two times. (this arrives when a socket is 
     * lost and the interpreter call this function). 
     */
    fflush(f); 
    close(fileno(f));
  }
}


void STk_freeport(SCM port)
{
  STk_close(port);
  free(PORT_NAME(port));
  free(port->storage_as.port.p);
}

void STk_init_standard_ports(void)
{
  STk_stdin = STk_curr_iport = STk_Cfile2port("*stdin*",  stdin, tc_iport, 0);  
  STk_gc_protect(&STk_stdin);
  STk_gc_protect(&STk_curr_iport);
  
  STk_stdout = STk_curr_oport = STk_Cfile2port("*stdout*", stdout, tc_oport, 0);
  STk_gc_protect(&STk_stdout);
  STk_gc_protect(&STk_curr_oport);

  STk_stderr = STk_curr_eport = STk_Cfile2port("*stderr*", stderr, tc_oport, 0);
  STk_gc_protect(&STk_stderr);
  STk_gc_protect(&STk_curr_eport);

  NEWCELL(STk_eof_object, tc_eof);
  STk_gc_protect(&STk_eof_object);

  STk_line_counter = 1;
  STk_current_filename = UNBOUND;	/* Unbound <=> stdin */
  STk_gc_protect(&STk_current_filename);
}

/*=============================================================================*\
 * 
 *  				L O A D   stuff
 *
\*=============================================================================*/

static int do_load(char *full_name, SCM module)
{
  int c;

#ifdef WIN32
  /* Transform name in an absolute name */
  full_name = CHARS(STk_internal_expand_file_name(full_name));
#endif

  if (!STk_dirp(full_name)) {
     FILE *f = fopen(full_name, "r");
     
     if (f == NULL) return 0;

     if (STk_lookup_variable(LOAD_VERBOSE, NIL) != Ntruth)
       Fprintf(STk_curr_eport, ";; Loading file \"%s\"\n", full_name);
     
     /* Just read one character. Assume that file is an object if this 
      * character is a control one. Here, I don't try to see if the file magic 
      * number has a particular value, since I'm not sure that all platforms
      * use identical conventions 
      */
     c = getc(f); fclose(f);

#if (defined(WIN32) && defined(USE_DYNLOAD) && !defined(CYGWIN32))
     /* If suffix is a Win32 shared-suffix then assume object file */
     if (c != EOF) {
	char *s, *shared_suffix;
	s=full_name;
	shared_suffix=CHARS(STk_lookup_variable("*shared-suffix*", NIL));
	s = s + strlen(s) - strlen(shared_suffix);
        if (stricmp(s,shared_suffix) == 0) c = *(s-1); else c = EOF;
     }
     if (c == '.') {
#else
     if (c != EOF && ((iscntrl(c)&& c!= '\n' && c!= '\t') || !isascii(c))) {
#endif
       STk_load_object_file(full_name);
     }
     else {
       /* file seems not to be an object file. Try to load it as a Scheme file */
       SCM prev_module;
       SCM previous_file, port;
       int previous_line;
       
       /* Save info about current line and file */
       previous_file	    = STk_current_filename;
       previous_line	    = STk_line_counter;
       STk_line_counter     = 1;
       STk_current_filename = STk_makestring(full_name);

       /* Save info about selected module */
       prev_module	    = STk_selected_module;
       STk_selected_module  = module;

       /* Create port for reading */
       port = makeport(full_name, tc_iport, "r", TRUE);

       PUSH_ERROR_HANDLER
	 {
	   SCM form;

	   STk_err_handler->context |= ERR_IN_LOAD;
	   
	   for( ; ; ) {
	     form = STk_readf(port, FALSE);
	     if (EQ(form, STk_eof_object)) break;
	     STk_eval(form, MOD_ENV(STk_selected_module));
	   }
	 }
       WHEN_ERROR
	 {
	   STk_close(port);
	   STk_selected_module = prev_module;
	   STk_last_defined    = Ntruth;
	   if (!STk_control_C) PROPAGATE_ERROR();
	 }
       POP_ERROR_HANDLER;
       
       STk_close(port);

       STk_current_filename = previous_file;
       STk_line_counter	    = previous_line;
       STk_selected_module  = prev_module;
       STk_last_defined     = Ntruth;
     }
     if (STk_lookup_variable(LOAD_VERBOSE, NIL) != Ntruth)
       Fprintf(STk_curr_eport, ";; File \"%s\" loaded\n", full_name);
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
    Err("load: filename too long", NIL);
    return 0; /* cannot occur */
}

SCM STk_load_file(char *fname, int err_if_absent, SCM module)
{
  int len;
  SCM load_path, load_suffixes;

  ENTER_PRIMITIVE("load");

  len           = strlen(fname);
  load_path     = STk_lookup_variable(LOAD_PATH,     NIL);
  load_suffixes = STk_lookup_variable(LOAD_SUFFIXES, NIL);
  
  if (STk_llength(load_path)<0)     Serror("bad loading path",    load_path);
  if (STk_llength(load_suffixes)<0) Serror("bad set of suffixes", load_suffixes);
 
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
	Serror("bad loading path component", CAR(load_path));

      if (try_loadfile(CHARS(CAR(load_path)), fname, load_suffixes, module))
	return(err_if_absent? UNDEFINED: Truth);
    }
  }

  /* If we are here, we have been unable to load a file. Report err if needed */
  if (err_if_absent)
    Serror("cannot open file", STk_makestring(fname));
  return Ntruth; 
}

PRIMITIVE STk_input_portp(SCM port)
{
  return INP(port) ? Truth: Ntruth;
}

PRIMITIVE STk_output_portp(SCM port)
{
  return OUTP(port)? Truth: Ntruth;
}

PRIMITIVE STk_input_file_portp(SCM port)
{
  return IPORTP(port) ? Truth: Ntruth;
}

PRIMITIVE STk_output_file_portp(SCM port)
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


/*============================================================================*\
 * 			w i t h - i n p u t - f r o m - . . .
\*============================================================================*/

SCM STk_redirect_input(SCM port, SCM thunk)
{
  SCM result     = UNDEFINED; /* to make gcc happy*/
  SCM prev_iport = STk_curr_iport;
  
  PUSH_ERROR_HANDLER
    {
      STk_curr_iport = port;
      result         = Apply0(thunk);
      STk_curr_iport = prev_iport;
    }
  WHEN_ERROR
    {
      STk_curr_iport = prev_iport;
      PROPAGATE_ERROR();
    }
  POP_ERROR_HANDLER;

  return result;
}


PRIMITIVE STk_with_input_from_file(SCM string, SCM thunk)
{
  SCM res, p;

  ENTER_PRIMITIVE("with-input-from-file");
  
  if (NSTRINGP(string))     Serror("bad string", string);
  if (!STk_is_thunk(thunk)) Serror("bad thunk", thunk);

  p   = makeport(CHARS(string), tc_iport, "r", TRUE);
  res = STk_redirect_input(p , thunk);
  STk_close(p);
  
  return res;
}

PRIMITIVE STk_with_input_from_port(SCM port, SCM thunk)
{
  ENTER_PRIMITIVE("with-input-from-port");

  if (!INP(port))           Serror("bad port",  port);
  if (!STk_is_thunk(thunk)) Serror("bad thunk", thunk);

  return STk_redirect_input(port, thunk);
}



/*============================================================================*\
 * 			w i t h - o u t p u t - t o - . . .
\*============================================================================*/

SCM STk_redirect_output(SCM port, SCM thunk)
{
  SCM result     = UNDEFINED; /* to make gcc happy*/
  SCM prev_oport = STk_curr_oport;
  
  PUSH_ERROR_HANDLER
    {
      STk_curr_oport = port;
      result         = Apply0(thunk);
      STk_curr_oport = prev_oport;
    }
  WHEN_ERROR
    {
      STk_curr_oport = prev_oport;
      PROPAGATE_ERROR();
    }
  POP_ERROR_HANDLER;

  Flush(port);
  return result;
}


PRIMITIVE STk_with_output_to_file(SCM string, SCM thunk)
{
  SCM res, p;

  ENTER_PRIMITIVE("with-output-to-file");

  if (NSTRINGP(string))     Serror("bad string", string);
  if (!STk_is_thunk(thunk)) Serror("bad thunk", thunk);

  p   = makeport(CHARS(string), tc_oport, "w", TRUE);
  res = STk_redirect_output(p, thunk);
  STk_close(p);
  
  return res;
}


PRIMITIVE STk_with_output_to_port(SCM port, SCM thunk)
{
  ENTER_PRIMITIVE("with-output-to-port");

  if (!OUTP(port))          Serror("bad port",  port);
  if (!STk_is_thunk(thunk)) Serror("bad thunk", thunk);

  return STk_redirect_output(port, thunk);
}

/*============================================================================*\
 * 			w i t h - e r r o r - t o - . . .
\*============================================================================*/


SCM STk_redirect_error(SCM port, SCM thunk)
{
  SCM result     = UNDEFINED; /* to make gcc happy*/
  SCM prev_eport = STk_curr_eport;
  
  PUSH_ERROR_HANDLER
    {
      STk_curr_eport = port;
      result         = Apply0(thunk);
      STk_curr_eport = prev_eport;
    }
  WHEN_ERROR
    {
      STk_curr_eport = prev_eport;
      PROPAGATE_ERROR();
    }
  POP_ERROR_HANDLER;

  Flush(port);
  return result;
}

PRIMITIVE STk_with_error_to_file(SCM string, SCM thunk)
{
  SCM res, p;

  ENTER_PRIMITIVE("with-error-to-file");

  if (NSTRINGP(string))     Serror("bad string", string);
  if (!STk_is_thunk(thunk)) Serror("bad thunk", thunk);

  p   = makeport(CHARS(string), tc_oport, "w", TRUE);
  res = STk_redirect_error(p, thunk);
  STk_close(p);
  
  return res;
}


PRIMITIVE STk_with_error_to_port(SCM port, SCM thunk)
{
  ENTER_PRIMITIVE("with-error-to-port");

  if (!OUTP(port))          Serror("bad port",  port);
  if (!STk_is_thunk(thunk)) Serror("bad thunk", thunk);

  return STk_redirect_error(port, thunk);
}


/*=============================================================================*\
 *                               Open/Close
\*=============================================================================*/


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
  STk_close(port);

  return UNDEFINED;
}

PRIMITIVE STk_close_output_port(SCM port)
{
  if (!OUTP(port)) Err("close-output-port: not an output port", port);
  STk_close(port);

  return UNDEFINED;
}

/*=============================================================================*\
 * 				Read
\*=============================================================================*/

PRIMITIVE STk_read(SCM port)
{
  port = verify_port("read", port, F_READ);
  return(STk_readf(port, FALSE));
}

PRIMITIVE STk_read_char(SCM port)
{
  int c;

  port = verify_port("read-char", port, F_READ);
  c = Getc(port);
  return (c == EOF) ? STk_eof_object : STk_makechar((unsigned char) c);
}

PRIMITIVE STk_peek_char(SCM port)
{
  int c;

  port = verify_port("peek-char", port, F_READ);
  c = Getc(port);
  Ungetc(c, port);
  return (c == EOF) ? STk_eof_object : STk_makechar((unsigned char) c);
}

PRIMITIVE STk_eof_objectp(SCM obj)
{
  return (obj == STk_eof_object)? Truth : Ntruth;
}

PRIMITIVE STk_char_readyp(SCM port) 
{
  port = verify_port("char-ready?", port, F_READ);
  return STk_internal_char_readyp(port) ? Truth : Ntruth;
}


/*=============================================================================*\
 * 				Write
\*=============================================================================*/

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
  Putc('\n', port);
  return UNDEFINED;
}

PRIMITIVE STk_write_char(SCM c, SCM port)
{
  ENTER_PRIMITIVE("write-char");

  if (NCHARP(c)) Serror("not a character", c);
  port = verify_port(proc_name, port, F_WRITE);
  Putc(CHAR(c), port);
  return UNDEFINED;
}

/*=============================================================================*\
 * 				Load
\*=============================================================================*/

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


/*===========================================================================*\
 * 
 * 			S T k   b o n u s
 *
\*===========================================================================*/

static SCM internal_format(SCM l,int len,int error)/* a very simple and poor one */ 
{
  SCM port, fmt;
  int format_in_string = 0;
  char *p, *proc_name = error? "error": "format";
  
  if (error) {
    if (len < 1) Serror("bad list of parameters", l);
    format_in_string = 1;
    port = STk_open_output_string();
    len -= 1;
  }
  else {
    if (len < 2) Serror("bad list of parameters", l);
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
  
  verify_port(proc_name, port, F_WRITE);
  if (NSTRINGP(fmt)) Serror("bad format string", fmt);

  for(p=CHARS(fmt); *p; p++) {
    if (*p == '~') {
      switch(*(++p)) {
        case 'A':
        case 'a': if (len-- <= 0) goto TooMuch;
	  	  STk_print(CAR(l), port, DSP_MODE);
                  l = CDR(l);
		  continue;
        case 'S':
        case 's': if (len-- <= 0) goto TooMuch;
                  STk_print(CAR(l), port, WRT_MODE);
                  l = CDR(l);
	          continue;        
        case 'W':
        case 'w': if (len-- <= 0) goto TooMuch;
	  	  STk_print_star(CAR(l), port);
		  l = CDR(l);
	          continue;
        case '%': Putc('\n', port);
                  continue;
        case '~': Putc('~', port);
                  continue;
        default:  Putc('~',  port);
                  /* NO BREAK */
      }
    }
    Putc(*p, port);
  }

  if (NNULLP(l)) Serror("too few ~ in format string", l);
  return format_in_string ? STk_get_output_string(port) : UNDEFINED;

TooMuch:
  Serror("too many ``~'' in format string", l);
  return UNDEFINED;
}

PRIMITIVE STk_format(SCM l, int len)
{
  return internal_format(l, len, FALSE);
}

PRIMITIVE STk_error(SCM l, int len)
{
  /* Set context to ERR_OK but keep the bit indicating if error must be caught */
  STk_err_handler->context = ERR_OK | ( STk_err_handler->context & ERR_IGNORED);

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
  
  ENTER_PRIMITIVE("open-file");

  if (NSTRINGP(filename)) Serror("bad file name", filename); 

/* allow modes "wb" and "rb" on win32 systems */
#if (defined(MSC_VER) && defined(WIN32) && !defined(CYGWIN32))
  if (NSTRINGP(mode)) goto Error;
  if (CHARS(mode)[1] != '\0' && CHARS(mode)[1] != 'b') goto Error;
#else
  if (NSTRINGP(mode) || CHARS(mode)[1] != '\0') goto Error;
#endif
  
  switch (CHARS(mode)[0]) {
    case 'a': 
    case 'w': type = tc_oport; break;
    case 'r': type = tc_iport; break;
    default:  goto Error;
  }
  return(makeport(CHARS(filename), type, CHARS(mode), FALSE));
Error:	     
  Serror("bad mode", mode);
  return UNDEFINED; /* for the compiler */
}

PRIMITIVE STk_close_port(SCM port)
{
  if (INP(port) || OUTP(port)) STk_close(port);
  else Err("close-port: bad port", port);
  return UNDEFINED;
}

PRIMITIVE STk_port_closedp(SCM port)
{
  ENTER_PRIMITIVE("port-closed?");

  if (!(INP(port) || OUTP(port))) Serror("bad port", port);
  return (PORT_FLAGS(port) & PORT_CLOSED) ? Truth : Ntruth;
}

PRIMITIVE STk_read_line(SCM port)
{
  int c;
  char buffer[INITIAL_LINE_SIZE], *buff;
  size_t i, size = INITIAL_LINE_SIZE;
  SCM res;

  port = verify_port("read-line", port, F_READ);
  buff = buffer;

  for (i = 0; ; i++) {
    if (i == size) {
      /* We must enlarge the buffer */
      size += size / 2;
      if (i == INITIAL_LINE_SIZE) {
	/* This is the first resize. Pass from static to dynamic allocation */
	buff = must_malloc(size);
	strncpy(buff, buffer, INITIAL_LINE_SIZE);
      }
      else 
	buff = must_realloc(buff, size);
    }
    switch (c = Getc(port)) {
      case EOF:  if (i == 0) return STk_eof_object;
		 /* NO BREAK */
      case '\n': res = STk_makestrg(i, buff); 
		 if (buff != buffer) free(buff); 
		 return res;
      case '\r': i--; continue;
      default:   buff[i] = c;
    }
  }
}


PRIMITIVE STk_copy_port(SCM in, SCM out)
{
  int c;
  
  ENTER_PRIMITIVE("copy-port");

  if (! INP(in))   Serror("bad input port",  in);
  if (! OUTP(out)) Serror("bad output port", out);

  while ((c = Getc(in)) != EOF)
    Putc(c, out);

  return UNDEFINED;
}


PRIMITIVE STk_flush(SCM port)
{
  ENTER_PRIMITIVE("flush");

  port = verify_port(proc_name, port, F_WRITE|F_READ);
  if (STk_internal_flush(port))
    Serror("cannot flush buffer", port);
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

  ENTER_PRIMITIVE("autoload");
  
  if (dont_do_autoload) return;

  if (recursive_call) {
    /* We have a recursive call if var has not be defined in the specified
     * file. In effect when a file has been loaded, we just try to find
     * the value of var. If this value has not been defined we will do 
     * another STk_do_autoload
     */
    recursive_call = 0;
    Serror("symbol was not defined", var);
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
      Serror("file not found for autoload symbol", Cons(var, file));
    
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

#if defined(USE_TK) && !defined(WIN32)
/******************************************************************************
 *
 * Port event management
 *
 ******************************************************************************/

static void apply_file_closure(SCM closure)
{
  Apply0(closure);
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

#ifdef USE_TK
/******************************************************************************
 *
 * Changing standard ports
 *
 ******************************************************************************/

PRIMITIVE STk_change_standard_ports(SCM in, SCM out, SCM err)
{
  static int cpt = 0;
  ENTER_PRIMITIVE("%change-standard-ports");

  if (cpt++)
    Serror("Cannot redirected standard port anymore", NIL);

  if (!INP(in))   Serror("bad input port",  in);
  if (!OUTP(out)) Serror("bad output port", out);
  if (!OUTP(err)) Serror("bad error port",  err);

  STk_curr_iport = in;
  STk_curr_oport = out;
  STk_curr_eport = err;

  return UNDEFINED;
}
#endif
