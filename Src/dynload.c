/*
 *
 * d y n l o a d . c			-- All the stuff dealing with 
 *					   dynamic loading
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
 *    Creation date: 23-Jan-1994 19:09
 * Last file update: 21-Jul-1996 21:24
 */

/* Support for HPUX is due to Dipankar Gupta <dg@hplb.hpl.hp.com> */
/* Support for NETBSD is from Franke Ruediger (Ruediger.Franke@rz.tu-ilmenau.de) */
/* Support for FreeBsD is due to Amancio Hasty Jr (hasty@netcom.com) */
/* Support for Linux is inspired from Patrick Nguyen (pnguyen@elde.epfl.ch) */


#if defined(SUNOS4) || defined(SUNOS5) || defined(IRIX5) || defined(OSF1)
#include <dlfcn.h>
#endif

#if defined(LINUX_DLD)
#include <dld.h>
#endif

#if defined(NETBSD1)	     
#include <sys/types.h>
#include <nlist.h>
#include <link.h>
#define dlerror() "dlerror"  /* dlerror() isn't implemented in NetBSD 1.0 */
#endif

#ifdef HPUX
#include <dl.h>
#endif

#include "stk.h"

#ifdef USE_DYNLOAD
static SCM list_of_files = NULL;

#if defined(SUNOS4) || defined(SUNOS5) || defined(NETBSD1) || defined(FREEBSD) || defined(IRIX5) || defined(OSF1) ||defined(LINUX_ELF)

static void load_and_call(char *path, char *fct_name)
{
  static void *self_handle= NULL;
  void *handle;
  void (*init_fct)();
  SCM str;

  /* Test if fct_name is already defined in the core interpreter
  /* Don't do a dlopen with NULL more than one time since it seems to break
  /* Solaris 2.3. (Moises Lejter <mlm@cs.brown.edu>) */
  if (self_handle == NULL) 
    if ((self_handle = (void *) dlopen(NULL,1)) == NULL)
      Err("Cannot open myself !!!", NIL);

  if ((init_fct = (void (*)()) dlsym(self_handle, fct_name)) != NULL)
    Err("Module is already (statically) loaded", STk_makestring(path));

  /* Try to avoid multiple loading */
  if (!list_of_files) {
    STk_gc_protect(&list_of_files);
    list_of_files = NIL;
  }
  str = STk_makestring(path);
  if (STk_member(str, list_of_files) != Ntruth) {
    Err("Module is already (dynamically) loaded", str);
  }
  
  /* Load file */
  if ((handle = (void *) dlopen(path, 1)) == NULL) {
    char msg[MAX_PATH_LENGTH];
#ifdef FREEBSD
    sprintf(msg, "Cannot open object file");
#else
    sprintf(msg, "Cannot open object file (%s)", dlerror());
#endif
    Err(msg, str);
  }
  
  if ((init_fct = (void (*)()) dlsym(handle, fct_name)) == NULL) {
    char msg[MAX_PATH_LENGTH];
    
    sprintf(msg, "Cannot find function \"%s\" in object file", fct_name);
    Err(msg, NIL);
  }
  /* Call the init code */
  (*init_fct)();
  
  list_of_files = Cons(str, list_of_files);
}
#endif

#if defined(LINUX_DLD)
/* 
 * This code is for Linux, using the dld package. This code should not be used 
 * anymore when ELF will be completely accepted under Linux. In the meanwhile...
 *
 * This code is inspired from a code sent by Patrick Nguyen pnguyen@elde.epfl.ch.
 *
 */

static void load_and_call(char *path, char *fct_name)
{
  void *handle;
  void (*init_fct)();
  SCM str;
  static dld_already_initialized = FALSE;

  /* Try to avoid multiple loading */
  if (!list_of_files) {
    STk_gc_protect(&list_of_files);
    list_of_files = NIL;
  }
  str = STk_makestring(path);
  if (STk_member(str, list_of_files) != Ntruth) {
    Err("Module is already (dynamically) loaded", str);
  }

  /* Dld must be initialized at first call */
  if(!dld_already_initialized) {
    if (dld_init (dld_find_executable (STk_Argv0)))
      dld_perror("dld: failed to init dld");
    else 
      dld_already_initialized = TRUE;
  }
  
  /* Load file */
  if (dld_link(path)) dld_perror("dld: cannot link");

  /* And get a pointer on function "fct_name" */
  init_fct = (void (*) ()) dld_get_func(fct_name);
  if (init_fct) {
    /* When loading a function. It can induce some unresolved references 
     * Most of the time, the unresolved references come from fucntions
     * which are in the libc but which are not used by the core interpreter.
     * For instance, if loaded module uses fork, which is not loaded in
     * the interpreter, we will have an unresolve reference for _fork.
     * To avoid this problem, we make again a link against standard libc
     * Note: in general situation is more complicated than this, unresolved 
     * symbols could be elsewhere than the libc....
     * Those situation are not handled by current code, but I hope that ELF 
     * will supplant this way of doing dynamic loading soon...
     */
    if (dld_function_executable_p (fct_name))
      /* Call the init code */
      (*init_fct) ();
    else {
      /* Function is notexecutable = we have unresolved references */
      if (dld_link("/usr/lib/libc.a") && dld_link("/usr/lib/libc.sa")) 
	dld_perror("dld: cannot link"); 

      if (dld_function_executable_p (fct_name)) {
	init_fct = (void (*) ()) dld_get_func(fct_name);
	(*init_fct) ();
      }
      else {
	char **unresolved;
	extern int dld_undefined_sym_count; 
	int i;	  
	fprintf(STk_stderr, "dld: function %s not executable!\n", fct_name);
	fprintf(STk_stderr, "Unresolved symbols are:\n");
	
	unresolved= dld_list_undefined_sym();
	for (i = 0; i < dld_undefined_sym_count; i++)
	  fprintf(STk_stderr, "\t%s\n",unresolved[i]);
	free(unresolved);
	Err("dld: link aborted", NIL);
      }
    }
  }
  else {
    char msg[MAX_PATH_LENGTH];
    sprintf(msg, "Cannot find function \"%s\" in object file", fct_name);
    Err(msg, NIL);
  }

  list_of_files = Cons(str, list_of_files);
}
#endif /* LINUX_DLD */

#ifdef HPUX
static void load_and_call(char *path, char *fct_name)
{
  shl_t handle;
  void (*init_fct)();
 
  if ((handle = shl_load(path, BIND_IMMEDIATE | BIND_VERBOSE, 0L)) == NULL) 
    Err("Cannot open file", STk_makestring(path));
  
  handle = NULL;
  if (shl_findsym(&handle, fct_name, TYPE_PROCEDURE, &init_fct) == -1) {
    char msg[MAX_PATH_LENGTH];
    
    sprintf(msg, "Cannot find function %s in object file", fct_name);
    Err(msg, NIL);
  }
  /* Call the init code */
  (*init_fct)();
}
#endif

void STk_load_object_file(char *path)
{
  char fct_name[1024], *p, *slash, *dot;

  /* Load the file as an object one */

  for (p = path, slash = p-1; *p; p++)		/* Find position of last '/' */
    if (*p == '/') slash = p;

#if defined(NETBSD1) || defined(FREEBSD)
  sprintf(fct_name, "_STk_init_%s", slash + 1);
#else
  sprintf(fct_name, "STk_init_%s", slash + 1);
#endif

  for (p = fct_name; *p; p++)			/* Delete suffix it it exists */
      if (*p == '.') { *p = '\0'; break; }

  load_and_call(path, fct_name);
}

#else /* not DYNLOAD */
void STk_load_object_file(char *path)
{
  Err("load: Loading of object file is not defined on this architecture", NIL);
}
#endif
