/*
 * i o . c                                      -- Low level I/O
 * 
 * Copyright © 1993-2019 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: ????
 * Last file update: 31-Oct-2019 15:55 (eg)
 */

#ifdef WIN32
#  include <windows.h>
#  include <io.h>
#  include <fcntl.h>
#  define TCL_EVENT_FLAG TCL_DONT_WAIT /* (FIXME) */
#else
#  include <sys/ioctl.h>
#  include <sys/time.h>
#  include <ctype.h>
#  define TCL_EVENT_FLAG 0
#  ifdef HAVE_SYS_SELECT_H
#    include <sys/select.h>     /* This seems to be useful only for AIX */
#  endif

#  ifndef NO_FD_SET
#    define SELECT_MASK fd_set
#  else
#     ifndef _AIX
        typedef long fd_mask;
#     endif
#     if defined(_IBMR2)
#         define SELECT_MASK void
#     else
#         define SELECT_MASK int
#     endif
#  endif
#endif


#include "stk.h"
#include "sport.h"
#include "vport.h"

#define BUFFER_SIZE     512
#define SYSTEM(instr)   { instr; }


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
#if (!defined (READ_DATA_PENDING)) && defined __GLIBC__
#    define READ_DATA_PENDING(fp) (max(0,(fp)->_IO_read_end - (fp)->_IO_read_ptr))
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


static char static_buffer[BUFFER_SIZE+1], *buffer = static_buffer;
static int bufidx=0;
static int count=-1;
static int filled=0;


void STk_StdinProc()
{
  buffer = static_buffer;
  for ( ; ; ) {
    errno = 0;
    SYSTEM(count = read(fileno(PORT_FILE(STk_stdin)), buffer, BUFFER_SIZE););
    if (count != -1 || errno != EINTR) break;
  }
  filled = 1;
  STk_control_C = 0; /* neglect ^C which arose when waiting an input */
}


void STk_fill_stdin_buffer(char *s)
{
  /* This procedure is called by the console to simulate the arrival of
   * characters on stdin. The string is GC protected                       */
  count  = strlen(s);
  buffer = s;
  filled = 1;
}

 
static int file_data_available(SCM port)
{
  return ( (PORT_UNGETC(port) != EOF)                   ||
           ((port == STk_stdin) && (bufidx < count))    ||
           (READ_DATA_PENDING(PORT_FILE(port)))          );
}


/*****************************************************************************/


int STk_getc(SCM port)
{
  int result = PORT_UNGETC(port);

  /* Ungetted character ? */
  if (result != EOF) {
    PORT_UNGETC(port) = EOF;
    return result;
  }

  /* Stdin */
  if (port == STk_stdin) {
    if (bufidx < count) return buffer[bufidx++];
    else {
#ifdef USE_TK
      if (Tk_initialized && STk_interactivep) {
        filled = 0;
        while (!filled && !STk_control_C) {
          Tcl_DoOneEvent(TCL_EVENT_FLAG);
          if (Tk_GetNumMainWindows() <= 0) return EOF;
        }
      }
      else
        STk_StdinProc();
#else 
      STk_StdinProc();
#endif
      if (count <= 0) return EOF;
      else {
        bufidx = 1;
        return *buffer;
      }
    }
  }

  /* Otherwise */
  switch (TYPE(port)) {
    case tc_iport : for ( ; ; ) {
                      errno = 0; /* OSF does not set errno to 0 before a read ! */
                      SYSTEM(result=getc(PORT_FILE(port)));
                      if (result != EOF || errno != EINTR) break;
                    }
                    return result;
    case tc_isport: {
                      register struct str_iob *f;

                      f = (struct str_iob *) PORT_FILE(port);
                      return (--(f->cnt)>=0? ((int)*f->ptr++): EOF);
                    }
    case tc_ivport: {
                      register struct virtual_iob *f;
                      SCM proc_eof, proc_getc, res;

                      f    = (struct virtual_iob *) PORT_FILE(port);

                      proc_eof  = f->eofp;
                      proc_getc = f->getc;

                      if (proc_eof == Ntruth || proc_getc == Ntruth) {
                        /* No eof or getc_procedure => always EOF */
                        return EOF;
                      } 

                      if (Apply1(proc_eof, port) == Ntruth) {
                        /* Not eof */
                        res = Apply1(proc_getc, port);
                        return (res == STk_eof_object) ? EOF:  CHAR(res);
                      } else {
                        /* eof procedure says we have an eof */
                        return EOF;
                      }
                    }
    default:        Err("INTERNAL ERROR in STk_getc", NIL); 
                    return 0;
  }
}

int STk_ungetc(int c, SCM port)
{
  int result = PORT_UNGETC(port);
  
  if (result != EOF) STk_err("INTERNAL ERROR: cannot unget character", NIL);
  PORT_UNGETC(port) = c;
  return c;
}

int STk_putc(int c, SCM port)
{
  ENTER_PRIMITIVE("write-char");
  switch (TYPE(port)) {
    case tc_oport :  SYSTEM(c=fputc(c, PORT_FILE(port)));
                     /* Signal an error if write fails. We can't do better here */
                     if (c == EOF) Serror("write error", NIL);
                     break;
    case tc_osport: {
                      register struct str_iob *f;
                      register unsigned int tmp;
    
                      f =  (struct str_iob *) PORT_FILE(port);
                      if (++f->cnt == f->bufsiz) {
                        tmp       = f->bufsiz;
                        tmp      += tmp/2;
                        f->base   = must_realloc(f->base, tmp);
                        f->ptr    = f->base + f->bufsiz - 1; /* base can move */
                        f->bufsiz = tmp;
                      }
                      *f->ptr++ = (unsigned char) c;
                      break;
                    }
    case tc_ovport: {
                      register struct virtual_iob *f;
                      SCM res, proc;

                      f    = (struct virtual_iob *) PORT_FILE(port);
                      proc = f->putc;
                      if (proc == Ntruth) /* No putc function: return EOF */
                        return EOF;
                      else {
                        res = Apply2(proc, STk_makechar(c), port);
                        if (res == STk_eof_object) Serror("write error", NIL);
                      }
                    }
  }
  return c;
}

int STk_puts(char *s, SCM port)
{
  ENTER_PRIMITIVE("write");

  switch (TYPE(port)) {
    case tc_oport :  {
                       int result;

                       SYSTEM(result = fputs(s, PORT_FILE(port)));
                       if (result == EOF) Serror("write-error", NIL);
                       return result;
                     }
    case tc_osport:  {
                       register struct str_iob *f;
                       register unsigned int tmp;
    
                       f =  (struct str_iob *) PORT_FILE(port);
                       for ( ; *s; s++) {
                         if (++f->cnt == f->bufsiz) {
                           tmp     = f->bufsiz;
                           tmp    += tmp/2;
                           f->base = must_realloc(f->base, tmp);
                           f->ptr  = f->base + f->bufsiz - 1; /* base can move */
                           f->bufsiz = tmp;
                         }
                         *f->ptr++ = (unsigned char) *s;
                       }
                       return 0;
                    }
                       
    case tc_ovport: {
                      register struct virtual_iob *f;
                      SCM res, proc;

                      f    = (struct virtual_iob *) PORT_FILE(port);
                      proc = f->puts;
                      if (proc == Ntruth) /* No putc function: return EOF */
                        return EOF;
                      else {
                        res = Apply2(proc, STk_makestring(s), port);
                        if (res ==STk_eof_object) Serror("write error", NIL);
                      }
                    }
                    /* NO BREAK */
    default:        return 0;
  }
}

int STk_eof(SCM port)
{
  switch (TYPE(port)) {
    case tc_iport :  {
                       int result;
                       SYSTEM(result = feof(PORT_FILE(port)));
                       return result;
                     }
    case tc_isport:  {
                       register struct str_iob *f;
                       f =  (struct str_iob *) PORT_FILE(port);
                       return f->cnt <= 0;
                     }
                       
    case tc_ivport: {
                      register struct virtual_iob *f;
                      SCM proc;

                      f    = (struct virtual_iob *) PORT_FILE(port);
                      proc = f->eofp;
                      return (proc == Ntruth) ? 1 : (Apply1(proc, port) != Ntruth);
                    }
    default:        return 1; /* always EOF on output files */
  }
}

int STk_internal_flush(SCM port)
{
  switch (TYPE(port)) {
    case tc_oport :  {
                       int result;
                       SYSTEM(result = fflush(PORT_FILE(port)));
                       return result;
                     }
    case tc_ovport: {
                      register struct virtual_iob *f;
                      SCM proc;

                      f    = (struct virtual_iob *) PORT_FILE(port);
                      proc = f->flush;
                      return (proc == Ntruth) ? 0 : (Apply1(proc, port) != Ntruth);
                    }
    default:        return 0; /* i.e. always works on other ports */
  }
}

int STk_internal_char_readyp(SCM port)
{
  switch (TYPE(port)) {
    case tc_iport : {
                      if (file_data_available(port)) return 1;
#ifdef WIN32
                      panic("char-ready?: Not yet implemented!");
#else
#  ifdef HAVE_SELECT
                      {
                        SELECT_MASK readfds;
                        struct timeval timeout;
                        int f = fileno(PORT_FILE(port));

                        FD_ZERO(&readfds); 
                        FD_SET(f, &readfds);
                        timeout.tv_sec = timeout.tv_usec = 0;
                        return (select(f+1, &readfds, NULL, NULL, &timeout));
                      }
#  else
#    ifdef FIONREAD
                      {
                        int result;

                        ioctl(fileno(PORT_FILE(port)), FIONREAD, &result);
                        return result;
                      }
#    else
                      return 1;
#    endif
#  endif
#endif
                    }
    case tc_isport: return 1;
    case tc_ivport: {
                      register struct virtual_iob *f;
                      SCM proc;

                      f    = (struct virtual_iob *) PORT_FILE(port);
                      proc = f->readyp;
                      return (proc == Ntruth) ? 1 : (Apply1(proc, port) != Ntruth);
                    }
    default:        return 1; /* always EOF on output files */
  }
}
   

void STk_close(SCM port)
{
  if (PORT_FLAGS(port) & PORT_CLOSED) /* already closed */ return;

  STk_internal_flush(port);
  
  switch (TYPE(port)) {
    case tc_iport : 
    case tc_oport : STk_close_file_port(port); break;

    case tc_ivport:
    case tc_ovport: {
                      register struct virtual_iob *f;
                      SCM proc;

                      f    = (struct virtual_iob *) PORT_FILE(port);
                      proc = f->close;
                      if (proc != Ntruth) Apply1(proc, port);
                    }
  }
  PORT_FLAGS(port) |= PORT_CLOSED;
}


void STk_fprintf(SCM port, char *format, ...)
{
  va_list ap;
  char buffer[MAX_FPRINTF];

  va_start(ap, format);
  vsprintf(buffer, format, ap);
  Puts(buffer, port);
}


char *STk_line_bufferize_io(SCM port)
{
  if (IPORTP(port) || OPORTP(port)) {
    char *buff= STk_must_malloc(BUFFER_SIZE);

    setvbuf(PORT_FILE(port), buff, _IOLBF, BUFFER_SIZE);
    return buff;
  }
  return NULL;
}


void STk_init_io(void)
{
  STk_stdin = STk_stdout = STk_stderr = NULL;
  STk_curr_iport = STk_curr_oport = STk_curr_eport = NULL;
}

