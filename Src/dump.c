/*
 * d u m p . c				-- Image creation
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: ??-Jul-1993 ??:??
 * Last file update:  3-Sep-1999 20:19 (eg)
 */

#include "stk.h"

int STk_dumped_core = 0;

#if defined (SUNOS4) || defined(FREEBSD) || defined(LINUX)
static caddr_t current_break 	= (caddr_t) -1;
static long data_size 		= 0;
static long data_start		= 0;
static int restoring_image	= 0;

#include <a.out.h>
#include <fcntl.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/time.h>

#define TEXT_START(x)	(N_TXTADDR(x)+(sizeof(x)-N_TXTOFF(x)))
#define TEXT_SIZE(x)	((x).a_text - (sizeof(x)-N_TXTOFF(x)))
#define DATA_START(x)	(N_DATADDR(x))


static int dump_data_file(char *argv0, char *name)
{
  int fd1, fd2;
  struct exec header;

  /* find the header of current running program */
  if ((fd1 = open(argv0, O_RDONLY)) < 0) 
    Err("dump: cannot open myself", STk_makestring(argv0));

  read(fd1, &header, sizeof header);
  
  /* Now that this header is read, create the new file */
  if ((fd2=open(name, O_WRONLY|O_CREAT|O_TRUNC, 0755)) < 0) {
    close(fd1);
    Err("dump: cannot open file", STk_makestring(name));
  }

  /* write in fd2 the current sbrk followed by data segment size */
  current_break = (caddr_t) sbrk(0);
  data_size     = (char *)current_break - (char *) DATA_START(header);
  data_start    = DATA_START(header);

  write(fd2, &current_break, sizeof(caddr_t));
  write(fd2, &data_size,     sizeof(long));
  write(fd2, &data_start,    sizeof(long));

  /* Copy data segment */
  write(fd2, (void *)DATA_START(header), data_size);
  
  close(fd1); close(fd2);
  return 1;
}


static int Restore_data_file(char *name)
{
  int fd;

  /* find the header of current running program */
  if ((fd = open(name, O_RDONLY)) < 0) 
    Err("Cannot open image file", STk_makestring(name));

  /* read the break we have to set and data segment size */
  read(fd, &current_break, sizeof(caddr_t));
  read(fd, &data_size,     sizeof(long));
  read(fd, &data_start,    sizeof(long));

  /* read data segment */
  brk(current_break);
  read(fd, (void *)data_start, data_size);

  close(fd); 
  return 1;
}


static void internal_dump(char *s)
{
  /* Store current continuation in a global Scheme variable */
  STk_eval_C_string("(define *global-continuation* (call/cc (lambda(e) e)))", NIL);

  if (restoring_image) {
    /* 
     * Since the primitive dump is in the call stack when we saved 
     * continuation, we go back here on image restoration. If restoring_image is
     * is equal to 1, we are restoring an image, so we can return.
     */
    return;
  }

  STk_dumped_core = 1;
  dump_data_file(STk_Argv0, CHARS(STk_internal_expand_file_name(s)));
  STk_dumped_core = 0;
}


void STk_restore_image(char *s)
{
  SCM gcont;

  Restore_data_file(s);
  STk_dumped_core =  restoring_image = 1;

  gcont = STk_lookup_variable("*global-continuation*", NIL);
  /* After reading the file we must have a continuation in *global-continuation* */
  if (NCONTINUATIONP(gcont)) {
    Err("restore: file loaded is corrupted. DANGER.", NIL);
  }
  Apply1(gcont, Ntruth);
}

/******************************************************************************
 *
 * d u m p   p r i m i t i v e 
 *
 ******************************************************************************/

PRIMITIVE STk_dump(SCM s)
{
  if (NSTRINGP(s)) Err("dump: bad file name", s);
#ifdef USE_TK
  if (Tk_initialized) Err("dump: cannot dump an image if you have "
			  "not used the `-no-tk' option.\nSorry.", NIL);
#endif
  internal_dump(CHARS(s));
  return UNDEFINED;
}

#else

/********************************************/
/****					 ****/
/**** Architectures without Dump support ****/
/****					 ****/
/********************************************/

static void dump_error()
{
  Err("dump/restore not available on this architecture", NIL);
}

void STk_restore_image(char *s)
{
  dump_error();
}

PRIMITIVE STk_dump(SCM s)
{
  dump_error();
  return UNDEFINED; /* never reached */
}
#endif
