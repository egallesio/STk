/*
 *
 * p o s i x . c			-- Provide some POSIX.1 functions 
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
 * $Id: posix.c 1.2 Thu, 10 Sep 1998 23:44:28 +0200 eg $ 
 *
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 14-Mar-1995 20:14
 * Last file update: 10-Sep-1998 15:06
 *
 * This file contains also contains code additions from Shiro Kawai 
 * <shiro@sqush.squareusa.com>
 */
 

#include <stk.h>
#include <sys/types.h>
#include <sys/utsname.h>

#define DefineConst(c) {STk_define_scheme_variable(#c, STk_makeinteger(c));}

/******************************************************************************
 * 
 * Error management
 *
 ******************************************************************************/
extern int errno;

static SCM get_errno(char *s)
{
  return STk_makeinteger((long) errno);
}

static void set_errno(char *s, SCM value)
{
  long n = STk_integer_value_no_overflow(value);

  if (n == LONG_MIN) Err("setting *errno*: bad integer", value);
  errno = n;
}

static PRIMITIVE posix_perror(SCM str)
{
  if (NSTRINGP(str)) Err("posix-perror: bad string", str);
  perror(CHARS(str));
  return UNDEFINED;
}

/******************************************************************************
 *
 * File and Directory functions
 *
 ******************************************************************************/
#include <sys/stat.h>

static int Cpointer_stat;

static PRIMITIVE posix_stat(SCM filename)
{
  struct stat *p;

  if (NSTRINGP(filename)) Err("posix-stat: bad string", filename);
  
  p = (struct stat *) must_malloc(sizeof(struct stat));
  if (stat(CHARS(filename), p) == -1) return Ntruth;
  
  return STk_make_Cpointer(Cpointer_stat, (void *) p, FALSE);
}

static PRIMITIVE posix_stat2vector(SCM descr)
{
  SCM z;
  struct stat *info;

  if (NCPOINTERP(descr) || EXTID(descr) != Cpointer_stat) 
    Err("posix-stat->vector: bad structure ", descr);
    
  info = (struct stat *) EXTDATA(descr);

  z = STk_makevect(10, NIL);	/* NIL because some integers are bignums */
  VECT(z)[0] = STk_makeinteger((long)info->st_dev);
  VECT(z)[1] = STk_makeinteger((long)info->st_ino);
  VECT(z)[2] = STk_makeinteger((long)info->st_mode);
  VECT(z)[3] = STk_makeinteger((long)info->st_nlink);
  VECT(z)[4] = STk_makeinteger((long)info->st_uid);
  VECT(z)[5] = STk_makeinteger((long)info->st_gid);
  VECT(z)[6] = STk_makeinteger((long)info->st_size);
  VECT(z)[7] = STk_makeinteger((long)info->st_atime);
  VECT(z)[8] = STk_makeinteger((long)info->st_mtime);
  VECT(z)[9] = STk_makeinteger((long)info->st_ctime);

  return z;
}

static PRIMITIVE posix_access(SCM filename, SCM mode)
{
  long m;

  if (NSTRINGP(filename)) Err("posix-access?: bad string", filename);
  if ((m=STk_integer_value_no_overflow(mode)) == LONG_MIN)
    Err("posix-access?: bad integer", mode);
  return (access(CHARS(filename), (int) m) == 0) ? Truth: Ntruth;
}

static PRIMITIVE posix_pipe(void)
{
  int fd[2];
  FILE *f0, *f1;

  if (pipe(fd) == -1) return Ntruth;
  
  if ((f0 = fdopen(fd[0], "r")) == NULL || (f1 = fdopen(fd[1], "w")) == NULL) {
    fclose(f0);   fclose(f1);
    close(fd[0]); close(fd[1]);
    return Ntruth;
  }

  return Cons(STk_Cfile2port("pipe (input)",  f0, tc_iport, 0),
	      STk_Cfile2port("pipe (output)", f1, tc_oport, 0));
}

/** Added by Shiro K. **/

static PRIMITIVE posix_unlink(SCM filename)
{
  int r;
  
  if (NSTRINGP(filename)) Err("posix-unlink: bad string", filename);
  r = unlink(CHARS(filename));
  return (r < 0)? Ntruth : Truth;
}

static PRIMITIVE posix_symlink(SCM to, SCM from)
{
  int r;
  if (NSTRINGP(from)) Err("posix-symlink: bad string", from);
  if (NSTRINGP(to))   Err("posix-symlink: bad string", to);
  r = symlink(CHARS(to), CHARS(from));
  return (r < 0)? Ntruth : Truth;
}


static PRIMITIVE posix_chmod(SCM path, SCM mode)
{
  int r;
  
  if (NSTRINGP(path)) Err("posix-chmod: bad string", path);
  if (NINTEGERP(mode)) Err("posix-chmod: bad mode", mode);
  r = chmod(CHARS(path), (mode_t)INTEGER(mode));
  return (r < 0)? Ntruth : Truth;
}

static PRIMITIVE posix_rename(SCM old, SCM new)
{
  int r;
  
  if (NSTRINGP(old)) Err("posix-rename: bad string", old);
  if (NSTRINGP(new)) Err("posix-rename: bad string", new);
  r = rename(CHARS(old), CHARS(new));
  return (r < 0)? Ntruth : Truth;
}

static PRIMITIVE posix_getlogin()
{
  char *s = getlogin();
  
  return (s == NULL) ?  Ntruth : STk_makestring(s);
}

static PRIMITIVE posix_mkdir(SCM path, SCM mode)
{
    int r;
  
    if (NSTRINGP(path))  Err("posix-mkdir: bad path", path);
    if (NINTEGERP(mode)) Err("posix-mkdir: bad mode", mode);
    r = mkdir(CHARS(path), (mode_t) INTEGER(mode));
    return (r < 0)? Ntruth : Truth;
}

static PRIMITIVE posix_rmdir(SCM path)
{
    int r;
    
    if (NSTRINGP(path))  Err("posix-rmdir: bad path", path);
    r = rmdir(CHARS(path));
    return (r < 0)? Ntruth : Truth;
}
/** End of addition by Shiro K. **/

/******************************************************************************
 *
 * Time functions 
 *
 ******************************************************************************/
#include <time.h>

#ifdef SUNOS4
#define mktime(c) timegm(c)
#endif

static int Cpointer_tm;

static void display_Cpointer_tm(SCM obj, SCM port, int mode)
{
  struct tm *p = (struct tm *) EXTDATA(obj);

  sprintf(STk_tkbuffer, "#<C-struct tm %02d/%02d/%02d %02d:%02d:%02d>", 
	  		p->tm_mon,  p->tm_mday, p->tm_year,
	  		p->tm_hour, p->tm_min,  p->tm_sec);
  Puts(STk_tkbuffer, PORT_FILE(port));
}

static PRIMITIVE posix_time(void)
{
  return STk_makeinteger((long) time(NULL));
}

static PRIMITIVE posix_ctime(SCM seconds)
{
  long sec;

  sec = (seconds == UNBOUND) ? time(NULL)
			     : STk_integer_value_no_overflow(seconds);
  if (sec == LONG_MIN) Err("posix-ctime: bad time value", seconds);
  
  return STk_makestring(ctime((time_t *) &sec));
}


static PRIMITIVE posix_localtime(SCM timer)
{
  long t = STk_integer_value_no_overflow(timer);

  if (t == LONG_MIN) Err("posix-localtime: bad time value", timer);

  return STk_make_Cpointer(Cpointer_tm, (void *) localtime((time_t *) &t), TRUE);
}

static PRIMITIVE posix_gmtime(SCM timer)
{
  long t = STk_integer_value_no_overflow(timer);

  if (t == LONG_MIN) Err("posix-gmtime: bad time value", timer);

  return STk_make_Cpointer(Cpointer_tm, (void *) gmtime((time_t *) &t), TRUE);
}

static PRIMITIVE posix_mktime(SCM t)
{
  time_t sec;
  if (NCPOINTERP(t) || EXTID(t) != Cpointer_tm) 
    Err("posix-mktime: bad time structure", t);
  
  sec = (time_t) mktime(EXTDATA(t));
  return STk_makeinteger((long) sec);
}

static PRIMITIVE posix_tm2vector(SCM t)
{
  SCM z;
  struct tm *p;
  
  if (NCPOINTERP(t) || EXTID(t) != Cpointer_tm) 
    Err("posix-tm->vector: bad time structure", t);
  
  z = STk_makevect(9, NIL); 	/* NULL could work here. But this is safer */
  p = (struct tm *) EXTDATA(t);

  VECT(z)[0] = STk_makeinteger(p->tm_sec);
  VECT(z)[1] = STk_makeinteger(p->tm_min);
  VECT(z)[2] = STk_makeinteger(p->tm_hour); 
  VECT(z)[3] = STk_makeinteger(p->tm_mday);
  VECT(z)[4] = STk_makeinteger(p->tm_mon);
  VECT(z)[5] = STk_makeinteger(p->tm_year);
  VECT(z)[6] = STk_makeinteger(p->tm_wday);
  VECT(z)[7] = STk_makeinteger(p->tm_yday);
  VECT(z)[8] = (p->tm_isdst) ? Truth: Ntruth;
 
  return z;
}

static PRIMITIVE vector2posix_tm(SCM v)
{
  struct tm *p;

  if (NVECTORP(v) || VECTSIZE(v) != 9)
    Err("vector->posix-tm: bad vector", v);

  p = (struct tm *) must_malloc(sizeof(struct tm));
  p->tm_sec   = STk_integer_value_no_overflow(VECT(v)[0]);
  p->tm_min   = STk_integer_value_no_overflow(VECT(v)[1]);
  p->tm_hour  = STk_integer_value_no_overflow(VECT(v)[2]);
  p->tm_mday  = STk_integer_value_no_overflow(VECT(v)[3]);
  p->tm_mon   = STk_integer_value_no_overflow(VECT(v)[4]);
  p->tm_year  = STk_integer_value_no_overflow(VECT(v)[5]);
  p->tm_wday  = STk_integer_value_no_overflow(VECT(v)[6]);
  p->tm_yday  = STk_integer_value_no_overflow(VECT(v)[7]);
  p->tm_isdst = (VECT(v)[8] == Truth);
  
  return STk_make_Cpointer(Cpointer_tm, p, FALSE);
}

static PRIMITIVE posix_strftime(SCM format, SCM t)
{
  char buffer[1024];
  struct tm *p;
  int len;

  if (NSTRINGP(format)) 
    Err("posix-strftime: Bad string", format);
    
  /* If t is not provided, assume that we want current localtime */
  if (t == UNBOUND) {
    time_t t = time(NULL);
    p = localtime(&t);
  }
  else {
    if (NCPOINTERP(t) || EXTID(t) != Cpointer_tm) 
      Err("posix-strftime: bad time structure", t);
    p = EXTDATA(t);
  }
  
  if (len=strftime(buffer, 1023, CHARS(format), p))
    return STk_makestring(buffer);
  else
    Err("posix-strftime: buffer too short", NIL);
  return UNDEFINED; /* never reached */
}

/******************************************************************************
 *
 * Processes stuff
 *
 ******************************************************************************/

static PRIMITIVE posix_fork(void)
{
  pid_t pid = fork();

  if (pid == 0) {
    /* Child process must delete Tcl interpreter (if it exists) */
    STk_delete_Tcl_child_Interp();
  }
  
  return (pid == -1) ? Ntruth: STk_makeinteger((long) pid);
}

static PRIMITIVE posix_wait(void)
{
  pid_t pid;
  int status;
  
  pid = wait(&status);
  if (pid == -1)
    return Ntruth;
  else
    return Cons(STk_makeinteger((long) pid), 
		STk_makeinteger((long) status));
}

/******************************************************************************
 *
 * System infos
 *     gethostname and getdomainname:  POSIX.1  does  not define these
 *     functions, but ISO/IEC 9945-1:1990 mentions them in B.4.4.1. 
 *			-- Linux documentation
 *
 ******************************************************************************/

static PRIMITIVE posix_uname(void)
{
  struct utsname buff;
  SCM v;

  if (uname(&buff) == -1)
    Err("posix-uname: cannot stat", NIL);
  
  v = STk_makevect(5, NIL);
  VECT(v)[0] = STk_makestring(buff.sysname);
  VECT(v)[1] = STk_makestring(buff.nodename);
  VECT(v)[2] = STk_makestring(buff.release);
  VECT(v)[3] = STk_makestring(buff.version);
  VECT(v)[4] = STk_makestring(buff.machine);

  return v;
}

static PRIMITIVE posix_host_name(void)
{
  char name[100];
  
  if (gethostname(name, 100) != 0)
    Err("posix-host-name: cannot determine name", NIL);

  return STk_makestring(name);
}

static PRIMITIVE posix_domain_name(void)
{
  char name[100];
  
  if (getdomainname(name, 100) != 0)
    Err("posix-domain-name: cannot determine domain", NIL);
  
  return STk_makestring(name);
}


/******************************************************************************
 *
 * Initialization code
 *
 ******************************************************************************/

PRIMITIVE STk_init_posix(void)
{
  /* Error management */
  STk_define_C_variable("*errno*", get_errno, set_errno);
  STk_add_new_primitive("posix-perror",	      tc_subr_1,    posix_perror);

  /* File and directories */
  Cpointer_stat = STk_new_Cpointer_id(NULL);
  STk_add_new_primitive("posix-stat",	      tc_subr_1,    posix_stat);
  STk_add_new_primitive("posix-stat->vector", tc_subr_1,    posix_stat2vector);
  STk_add_new_primitive("posix-access?",      tc_subr_2,    posix_access);
  STk_add_new_primitive("posix-pipe",       tc_subr_0, 	    posix_pipe);

  DefineConst(F_OK);  	DefineConst(R_OK);  	DefineConst(W_OK);
  DefineConst(X_OK);

  /** Addition by SK **/
  STk_add_new_primitive("posix-unlink",       tc_subr_1,    posix_unlink);
  STk_add_new_primitive("posix-symlink",      tc_subr_2,    posix_symlink);
  STk_add_new_primitive("posix-chmod",        tc_subr_2,    posix_chmod);
  STk_add_new_primitive("posix-rename",       tc_subr_2,    posix_rename);
  STk_add_new_primitive("posix-getlogin",     tc_subr_0,    posix_getlogin);
  STk_add_new_primitive("posix-mkdir",        tc_subr_2,    posix_mkdir);
  STk_add_new_primitive("posix-rmdir",        tc_subr_1,    posix_rmdir);

  DefineConst(S_ISUID);
  DefineConst(S_ISGID);
  DefineConst(S_ISVTX);
  DefineConst(S_IRWXU);
  DefineConst(S_IRUSR);
  DefineConst(S_IWUSR);
  DefineConst(S_IXUSR);
  DefineConst(S_IRWXG);
  DefineConst(S_IRGRP);
  DefineConst(S_IWGRP);
  DefineConst(S_IXGRP);
  DefineConst(S_IRWXO);
  DefineConst(S_IROTH);
  DefineConst(S_IWOTH);
  DefineConst(S_IXOTH);
  DefineConst(S_IFMT);
  DefineConst(S_IFIFO);
  DefineConst(S_IFCHR);
  DefineConst(S_IFDIR);
  DefineConst(S_IFBLK);
  DefineConst(S_IFREG);
  DefineConst(S_IFLNK);
  DefineConst(S_IFSOCK);
  /** End addition by SK **/

  /* Time */
  Cpointer_tm = STk_new_Cpointer_id(display_Cpointer_tm);
  STk_add_new_primitive("posix-time",       tc_subr_0,      posix_time);
  STk_add_new_primitive("posix-ctime",      tc_subr_0_or_1, posix_ctime);  
  STk_add_new_primitive("posix-localtime",  tc_subr_1,      posix_localtime);
  STk_add_new_primitive("posix-gmtime",     tc_subr_1,      posix_gmtime);
  STk_add_new_primitive("posix-mktime",     tc_subr_1,      posix_mktime);
  STk_add_new_primitive("posix-tm->vector", tc_subr_1,      posix_tm2vector);
  STk_add_new_primitive("vector->posix-tm", tc_subr_1,      vector2posix_tm);
  STk_add_new_primitive("posix-strftime",   tc_subr_1_or_2, posix_strftime);

  /* Processes */
  STk_add_new_primitive("posix-fork",       tc_subr_0, 	    posix_fork);
  STk_add_new_primitive("posix-wait",       tc_subr_0, 	    posix_wait);
  
  /* System information */
  STk_add_new_primitive("posix-uname",      tc_subr_0, 	    posix_uname);
  STk_add_new_primitive("posix-host-name",  tc_subr_0, 	    posix_host_name);
  STk_add_new_primitive("posix-domain-name",tc_subr_0, 	    posix_domain_name);
  
  return UNDEFINED;
}
