/*
 * i o . c					-- Low level I/O
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
 *	     Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: ????
 * Last file update: 26-Apr-1998 11:09
 */

#ifdef WIN32
#  include <windows.h>
#  include <io.h>
#  include <fcntl.h>
#  define TCL_EVENT_FLAG TCL_DONT_WAIT /* (FIXME) */
#else
#  define TCL_EVENT_FLAG 0
#endif

#include "stk.h"
#include "sport.h"

#define BUFFER_SIZE	512
#define SYSTEM(instr)	{ instr; }
#define STRING_PORTP(f) (((struct str_iob *) f)->signature == SPORT_SIGNATURE)

#ifdef max
#   undef max
#endif
#define max(a,b) ((a)>(b)? (a) : (b))

#ifdef _STDIO_USES_IOSTREAM  /* GNU libc */
#  if defined(_IO_STDIO_H) || defined (linux)
#    define READ_DATA_PENDING(fp) (max(0,(fp)->_IO_read_end - (fp)->_IO_read_ptr))
#  else
#    define READ_DATA_PENDING(fp) (max(0,(fp)->_egptr - (fp)->_gptr))
#  endif
#endif
#if (!defined (READ_DATA_PENDING)) && defined __SLBF
#  define READ_DATA_PENDING(fp) (max(0,fp->_r))
#endif
#if !defined (READ_DATA_PENDING)
#  ifdef WIN32
#    define READ_DATA_PENDING(fp) (1)
#  else 
#    define READ_DATA_PENDING(fp) (fp->_cnt)
#  endif
#endif


#ifdef WIN32
  FILE *STk_stdin, *STk_stdout, *STk_stderr;
#endif

static char buffer[BUFFER_SIZE+1];
static int bufidx=0;
static int count=-1;
static int previous_char;
static int ungetted = 0;
static int filled=0;


static void badport(int read)
{
  Err(read ? "String port is not open for reading"
           : "String port is not open for writing",
      NIL);
}

static int nop(Tcl_Event *unused1, int unused2){ }

#ifdef WIN32
static insert_dummy_event(void)
{
  struct Tcl_Event *p;

  p = (Tcl_Event *) must_malloc(sizeof(struct Tcl_Event));
  p->proc =  nop;
  Tcl_QueueEvent(p, TCL_QUEUE_TAIL);
}

static DWORD Kbd_Thread(LPDWORD dumb)
{
  unsigned long size;

  for ( ; ; ) {
    ReadFile(GetStdHandle(STD_INPUT_HANDLE), buffer, BUFFER_SIZE, &size, NULL);
    buffer[size] = '\0';
    count	 = (int) size;
    filled       = 1;
  }
  /* Never readched */
  return 0;
}
#endif

void STk_StdinProc()
{
  for ( ; ; ) {
    SYSTEM(count = read(fileno(STk_stdin), buffer, BUFFER_SIZE););
    if (count != -1 || errno != EINTR) break;
  }
  filled = 1;
}


int STk_file_data_available(FILE *f)
{
  if ((f == STk_stdin) && (bufidx < count)) return 1;
  if (READ_DATA_PENDING(f))		    return 1;
  return 0;
}


int STk_getc(FILE *f)
{
  if (f == STk_stdin) {
    if (ungetted) {
      ungetted = 0;
      return previous_char;
    }
    if (bufidx < count) return buffer[bufidx++];
    else {
#ifdef USE_TK
      if (Tk_initialized && STk_interactivep) {
	filled = 0;
	while (!filled) {
	  Tcl_DoOneEvent(TCL_EVENT_FLAG);
	  if (Tk_GetNumMainWindows() <= 0) return EOF;
	}
      }
#  ifndef WIN32
      else
	STk_StdinProc();
#  endif
#else 
      /* This code is for snow only (Unix and Win32 flavours) */
      STk_StdinProc();
#endif
      if (count <= 0) return EOF;
      else {
	bufidx = 1;
	return *buffer;
      }
    }
  }
  else
    if (STRING_PORTP(f)) {
      register struct str_iob *g = (struct str_iob *)f;
      if (!(g->flag & READING)) badport(TRUE);
      return (--(g->cnt)>=0? ((int)*g->ptr++): EOF);
    }
    else {
      int result;

      for ( ; ; ) {
	SYSTEM(result=getc(f));
	if (result != EOF || errno != EINTR) break;
      }
      return result;
    }
}

int STk_ungetc(int c, FILE *f)
{
  if (f == STk_stdin) {
    ungetted = 1;
    previous_char = c;
  }
  else
    if (STRING_PORTP(f)) {
      register struct str_iob *g = (struct str_iob *)f;
      if (g->ptr == g->base) Err("INTERNAL ERROR: cannont unget char", NIL);
      if (!(g->flag & READING)) badport(TRUE);
      g->ptr--;
      g->cnt++;
    }
    else
      SYSTEM(ungetc(c, f));
  return c;
}

int STk_putc(int c, FILE *f)
{
  if (STRING_PORTP(f)) {
    register struct str_iob *g = (struct str_iob *)f;
    register int tmp;
    
    if (!(g->flag & WRITING)) badport(FALSE);
    if (++g->cnt == g->bufsiz) {
      tmp	 = g->bufsiz;
      tmp	+= tmp/2;
      g->base	 = must_realloc(g->base, tmp);
      g->ptr	 = g->base + g->bufsiz - 1; /* since base can have been moved */
      g->bufsiz	 = tmp;
    }
    *g->ptr++ = (unsigned char) c;
  }
  else {
    SYSTEM(c=fputc(c, f));
    /* Signal an error if write fails. We can't be very cute here */
    if (c == EOF) Err("write-char: write error", NIL);
  }
  return c;
}

int STk_puts(char *s, FILE *f)
{
  if (STRING_PORTP(f)) {
    while (*s) STk_putc(*s++, f);
    return 0;
  }
  else {
    int result;
    SYSTEM(result = fputs(s, f));
    if (result == EOF) Err("write: write-error", NIL);
    return result;
  }
}

int STk_eof(FILE *f)
{
  if (STRING_PORTP(f))
    return (((struct str_iob *)f)->cnt <= 0);
  else {
    int result;
    SYSTEM(result=feof(f));
    return result;
  }
}


char * STk_line_bufferize_io(FILE *f)
{
  char *buff;

  if ((buff=malloc(BUFFER_SIZE)) == NULL)
    panic("Cannot allocate a line buffer for output file");
  setvbuf(f, buff, _IOLBF, BUFFER_SIZE);
  return buff;
}


#ifdef WIN32
  /* 
   * Windows apps don't have an associated console. The following code
   * associate a console to the 3 standard files.  However, for some
   * reason, even when closing stdin stdout and stderr, I was not able
   * to remap console files on the first enties. Hence, the file name
   * change from stdin to STk_stdin ...	 
   */
#  include <windows.h>
#  include <io.h>
#  include <fcntl.h>


  void STk_init_io(void)
  {
    HANDLE Fin, Fout, Ferr;
    unsigned long dumb;
    
    if (AllocConsole()) {
      Fin  = GetStdHandle(STD_INPUT_HANDLE);
      Fout = GetStdHandle(STD_OUTPUT_HANDLE);
      Ferr = GetStdHandle(STD_ERROR_HANDLE);
      
      fclose(stdin); fclose(stdout); fclose (stderr);
      
      STk_stdin	 = fdopen(_open_osfhandle((long) Fin,  O_RDONLY), "r");
      STk_stdout = fdopen(_open_osfhandle((long) Fout, O_APPEND), "w");
      STk_stderr = fdopen(_open_osfhandle((long) Ferr, O_APPEND), "w");
      
      fflush(STk_stdout);
      SetConsoleTitle("  *** STk console ***  ");
      
      /* Create a thread for reading keyboard */
#ifdef USE_TK
      CreateThread(NULL, 100, (LPTHREAD_START_ROUTINE) Kbd_Thread, NULL, 0, &dumb);
#endif
    }
    else
      STk_panic("Cannot create Win32 console");
  }
#endif
