/*
 *
 * a r g v . c			-- Argc/Argv management
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
 *
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 30-Aug-1994 15:38
 * Last file update:  7-Jun-1998 18:01
 */

#include "stk.h"

#ifdef WIN32
#include <process.h>
#include <io.h>
#include <stdlib.h>
#endif

/* Previous versions of Stk use the TkArgv mechanism for managing argc/argv.
 * Here is an ad hoc version for argc/argv hich is simpler than the original
 * one and which can be used when NO_TK is defined
 */

#ifdef USE_TK
char *STk_arg_Xdisplay 	  = NULL;
char *STk_arg_geometry 	  = NULL;
char *STk_arg_name 	  = NULL;
char *STk_arg_visual	  = NULL;
int   STk_arg_colormap	  = 0;
int   STk_arg_sync	  = 0;
int   STk_arg_no_tk	  = 0;
#endif
char *STk_arg_file	  = NULL;
char *STk_arg_load	  = NULL;
char *STk_arg_image	  = NULL;
char *STk_arg_cells	  = NULL;
int   STk_arg_interactive = 0;

static struct arguments {
  char *key;
  char **flag;
  char need_argument;
  char *init;
  char *help;
} Table[] = {
#ifdef USE_TK
  {"-colormap",	   (char **) &STk_arg_colormap,	   0, (char *) 0,
   		   "Use a private colormap"},
  {"-display",	   &STk_arg_Xdisplay,		   1, NULL,
		   "Display to use"},
  {"-geometry",	   &STk_arg_geometry,		   1, NULL,
		   "Initial geometry for window"},
  {"-name",	   &STk_arg_name,		   1, NULL,
		   "\tName to use for application"},
  {"-sync",	   (char **) &STk_arg_sync,	   0, (char *) 0,
		   "\tUse synchronous mode for display server"},
  {"-visual", 	   &STk_arg_visual,		   1, NULL,
   		   "Visual for main window"},
  {"-no-tk",	   (char **) &STk_arg_no_tk,	   0, (char *) 0,
		   "\tDon't initialize Tk"},
#endif
  {"-file",	   &STk_arg_file,		   1,  NULL,
		   "\tFile from which to read commands"},
  {"-load",	   &STk_arg_load,		   1,  NULL,
		   "\tFile to load after all the initializations are done"},
  {"-cells",	   &STk_arg_cells,		   1, (char *) 0L,
		   "\tDefault size for heap"},
  {"-image",	   &STk_arg_image,		   1,  NULL,
		   "\tUse  previously created image"},
  {"-interactive", (char **) &STk_arg_interactive, 0, (char *) 0,
		   "Interactive mode"},
  {"-help",	   NULL,			   0,  NULL,
		   "\tPrint summary of command-line options and abort"},
  {"", NULL, 0, NULL, ""}};

static void usage(void)
{
  struct arguments *p;
  
  for (p = Table; *(p->key); p++) 
    fprintf(STk_stderr, "%s\t%s\n", p->key, p->help);
  exit(1);
}

char **STk_process_argc_argv(int argc, char **argv)
{
  struct arguments *p;
  char **new_argv;
  int i;
  
  /* Reset arguments to their default value */
  for (p = Table; *(p->key); p++) 
    if (p->flag)
      *(p->flag) = p->init;

  /* Set the program name */
  STk_whence(*argv, STk_Argv0);

  /* Option analysis */
  while (--argc) {
    char *arg  = *++argv;
    int found  = FALSE;

    if (!*arg) break;
    
    if (strcmp(arg, "--") == 0) { /* Special delimiter */
      argv += 1;
      argc -= 1;
      break;
    }
    /* Try to find the option in table */
    for (p = Table; *(p->key); p++) {
      if (strlen(arg) >= 2) {
	if  (strstr(p->key, arg)== p->key) {
	  found = TRUE;
	
	  if (p->need_argument) {
	    argc -= 1;
	    argv += 1;
	    
	    if (*argv) 
	      *(p->flag) = *argv;
	    else {
	      fprintf(STk_stderr,
		      "\"%s\"  option requires an additional argument\n", 
		      p->key);
	      exit(1);
	    }
	  }
	  else
	    if (p->flag)
	      *(p->flag) = (char *) TRUE;
	    else
	       usage();
	}
      }
    }
    
    if (!found) {
      /* Special convention: if we have an argument before "--" alone 
       * consider it as the name of a script file 
       */
      if (!STk_arg_file) {
	STk_arg_file = arg;
	argv += 1;
	argc -= 1;
      }
      break;
    }
  }
  
  /* Option have been analysed. Parse program arguments */
  new_argv = (char**) must_malloc((argc + 1)  * sizeof(char*));

  for (i = 0; argc; argc--) {
    new_argv[i++] = *argv++;
  }
  new_argv[i] = NULL;
  return new_argv;
}

/******************************************************************************
 *
 * Unix environment saving
 *
 * 	The two following function permit to save/restore the argc/argv/envp
 * in a file. They are used upon image restauration.
 *
 ******************************************************************************/
void STk_save_unix_args_and_environment(int argc, char **argv)
{
  FILE *f;
  char **env;
  char filename[50];
  extern char **environ;

  /* Open a file in which we will save argc/argv/envp */
  sprintf(filename, "/usr/tmp/STktmp%d", getpid());
  if ((f = fopen(filename, "w")) == NULL) {
    STk_panic("Cannot save environment in %s.", filename);
    exit(1);
  }

  /* print argc */
  fprintf(f, "%d\n", argc);

  /* print argv */
  for ( ; argc; argc-=1, argv+=1) {
    fprintf(f, "%d %s\n", strlen(*argv), *argv);
  }

  /* print environment */
  for (env=environ; *env; env++) {};
  fprintf(f, "%d\n", env-environ);

  for (env=environ; *env; env++) {
    fprintf(f, "%d %s\n", strlen(*env), *env);
  }

  /* Close file */
  fclose(f);
}

void STk_restore_unix_args_and_environment(int *argc, char ***argv)
{
  FILE *f;
  int i, l, Argc, env_len;
  char **Argv, **Env;
  char filename[50];
  extern char **environ;

  /* Open a file in which we have saved argc/argv/envp */
  sprintf(filename, "/usr/tmp/STktmp%d", getpid());
  if ((f = fopen(filename, "r")) == NULL) {
    STk_panic("Cannot re-open environment in %s.", filename);
    exit(1);
  }

  /* Read argc */
  fscanf(f, "%d", &Argc); getc(f);

  /* Read argv */
  Argv = must_malloc((Argc+1) * sizeof(char *));

  for (i=0; i<Argc; i++) {
    fscanf(f, "%d", &l); getc(f);
    Argv[i] = must_malloc(l+1);
    fread(Argv[i], 1, l, f);
    Argv[i][l] = '\0';
  }
  Argv[Argc] = NULL;

  /* Read environment */
  fscanf(f, "%d", &env_len); getc(f);
  Env = must_malloc((env_len+1) * sizeof(char *));
  for (i=0; i<env_len; i++) {
    fscanf(f, "%d", &l); getc(f);
    Env[i] = must_malloc(l+1);
    fread(Env[i], 1, l, f);
    Env[i][l] = '\0';
  }
  Env[i]= NULL;

  /* Save read values in global variables */
  *argc   = Argc;
  *argv   = Argv;
  environ = Env;
  
  /* close & delete temporary file */
  fclose(f);
  unlink(filename);
}


void STk_initialize_scheme_args(char **argv)
{
  SCM l;
  char *progname;

  progname = (STk_arg_file) ? STk_arg_file : STk_Argv0;
  
  for (l = NIL; *argv; argv++)
    l = Cons(STk_makestring(*argv), l);
  
  STk_define_variable(ARGV, Reverse(l), NIL);
  STk_define_variable(PROG_NAME, STk_makestring(progname), NIL);
}

#ifdef WIN32

/* #include <dos.h>   enlevé pour CYGWIN32 */

char **STk_Win32_make_argc_argv(char *lpszCmdLine, int *argc)
{
  int size, i, code;
  char **argv, **argvlist, *p, *tmp, *prog= "STk.exe";

  /*
   * First get an upper bound on the size of the argv array by counting the
   * number of whitespace characters in the string.
   */
  for (size=1, p=lpszCmdLine; *p != '\0'; p++) {
    if (isspace(UCHAR(*p))) size++;
  }
  size+=2;		/* Leave space for program name final NULL pointer. */
  argvlist = (char **) ckalloc((unsigned) (size * sizeof(char *)));
  argv = argvlist;

  p = getenv("STK_LIBRARY");
  if (p == NULL) 
    panic("You must set the STK_LIBRARY variable");
  tmp = must_malloc(strlen(p) + strlen(prog) + 2);
  sprintf(tmp, "%s/%s", p, prog);
  argv[0] = tmp;    		/* FIXME */

  /*
   * Split the command line into words, and store pointers to the start of
   * each word into the argv array.  Skips leading whitespace on each word.
   */

  for (i=1, p=lpszCmdLine; *p != '\0'; i++) {
    while (isspace(UCHAR(*p))) p++;
    if (*p == '\0') break;
    
    argv[i] = p;
    while (*p != '\0' && !isspace(UCHAR(*p))) 
      p++;
    if (*p) *p++ = '\0';
  }
  argv[i] = NULL;
  *argc = i;
  
  return argv;
}
#endif
