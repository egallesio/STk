/*
 *
 * u n i x . c					-- Some Unix primitives
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 29-Mar-1994 10:57
 * Last file update: 15-Sep-1999 18:05 (eg)
 */
#ifndef WIN32
#  include <unistd.h>
#  include <pwd.h>
#else
#  include <windows.h>
#  include <io.h>
#  define F_OK   00
#  define X_OK   01
#  define W_OK   02
#  define R_OK   04
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
   /* One of above files includes <stdarg.h> with BC++  (and stdarg and 
    * vararg are not compatible 
    */
#  undef __STDARG_H 

#  ifdef _MSC_VER
#  include <direct.h>
#  include <process.h>
#  include <sys/stat.h>
#  define S_ISDIR(mode) ((mode & _S_IFMT) == _S_IFDIR)
#  define S_ISREG(mode) ((mode & _S_IFMT) == _S_IFREG)
#  else
#     ifdef BCC
        /* Borland defines the opendir/readdir/closedir functions. Use them. */
#       include <dirent.h>
#     endif
#  endif
#else
#  include <dirent.h>
#endif
 
#include "stk.h"


#ifdef SUNOS4
/* I avoid to use the POSIX getcwd since it is implemented using popen(3) and
 * pwd(1) on SunOS 4.1.3 ==> It is VERY SLOW.
 */

#define getcwd my_getcwd
static char *my_getcwd(char *path, int size)
{
  if (!path) path = (char *) must_malloc(size);
  getwd(path);
  return path;
}
#endif  /* SUNOS4 */

/******************************************************************************
 *
 * Utilities
 *
 ******************************************************************************/

/*
 * TILDE-EXPAND		-- expand '~' and '~user' string prefix
 *
 */

static char *tilde_expand(char *name, char *result)
{
  if (name[0] != '~') {
    strcpy(result, name);
    return name;
  }
  
#ifdef WIN32
  if ((name[1] == '/') || (name[1] == '\\') || (name[1] == '\0')) {
#else
  if ((name[1] == '/') || (name[1] == '\0')) {
#endif
    char *dir = getenv("HOME");
    
    if (dir == NULL)
      Err("couldn't find HOME in environment when expanding", STk_makestring(name));

    sprintf(result, "%s%s", dir, name+1);
  } 
  else {
#ifdef WIN32
    Err("Form '~user' not allowed on Win32", STk_makestring(name));
#else
    char *p;
    struct passwd *pwPtr;
    register int len;
    for (p=&name[1]; (*p != 0) && (*p != '/'); p++) {
      /* Null body;  just find end of name. */
    }
    len = p-(name+1);
    strncpy(result, name+1, (size_t) len);
    result[len] = '\0';

    pwPtr = getpwnam(result);
    if (pwPtr == NULL) {
      endpwent();
      Err("User does not exist", STk_makestring(result));
    }
    sprintf(result, "%s%s", pwPtr->pw_dir, p);
    endpwent();
#endif
  }
  return result;
}

/*
 * ABSOLUTE	-- Given a file name, return its (mostly clean) absolute path name
 *
 */
static void absolute(char *s, char *pathname)	
{
  char *p = pathname;
  char *t;

  if (!ISABSOLUTE(s)) {
    getcwd(pathname, MAX_PATH_LENGTH);
    p = &pathname[strlen(pathname)];     /* place p at end of pathname */ 
#ifdef WIN32 
    *p = DIRSEP;
#endif
  }
#ifdef WIN32
  else *p = *s++;
#else
  *p = DIRSEP;
#endif
  
  for ( ; *s; s++) {
    t = s;
    switch (*s) {
      case '.' : if (*(s+1)) {
		   switch (*++s) {
		     case '.' : if (ISDIRSEP(*p) && (*(s+1)=='\0' || 
						     ISDIRSEP(*(s+1)))) {
		                  /* We must go back to the parent */
		                  if (ISDIRSEP(*p) && p > pathname)    p --;
				  while (p > pathname && !ISDIRSEP(*p)) p--;
		                }
		                else {
				  /* There is a suit of dot. Copy it */
				  for (s = t; *s == '.'; s++) *++p = '.';
				  s -= 1;
				}
		                break;
#ifdef WIN32
		     case '\\':
#endif
		     case '/' : if (!ISDIRSEP(*p)) {
		                   *++p = '.';
		       		   *++p = DIRSEP;
		     	        }
		                break;
		     default  : *++p = '.'; *++p = *s; break;
		   }
                 }
                 else { /* We have a final (single) dot */
		   if (!ISDIRSEP(*p)) *++p = '.';
		 }
                 break;
#ifdef WIN32
      case '\\':
#endif
      case '/' : if (!ISDIRSEP(*p)) *++p = DIRSEP; break;
      default  : *++p = *s;
    }
  }
  
  /* Place a \0 at end. If path ends with a "/", delete it */
  if (p == pathname || !ISDIRSEP(*p)) p++;
  *p = '\0';

#ifdef WIN32
  /* Replace all "/" by "\" */
  for (p = pathname; *p; p++) 
    if (*p == '/') *p = '\\';
#endif
}


#define MAXLINK 50	/* Number max of link before declaring we have a loop */

SCM STk_resolve_link(char *path, int count)
{
#ifdef WIN32
  return STk_internal_expand_file_name(path);
#else
  char link[MAX_PATH_LENGTH], dst[MAX_PATH_LENGTH], *s, *d=dst;
  int n;
  SCM p;
  
  p  = STk_internal_expand_file_name(path);
  
  for (s=CHARS(p)+1, *d++='/' ;       ; s++, d++) {
    switch (*s) {
      case '\0':
      case '/' : *d = '\0';
	if ((n=readlink(dst, link, MAX_PATH_LENGTH-1)) > 0) {
	  link[n] = '\0';
	  if (link[0] == '/') 
	    /* link is absolute */
	    d = dst;
	  else {
	    /* relative link. Delete last item */
	    while (*--d != '/') {
	    }
	    d += 1;
	  }
		   
	  /* d points the place where the link must be placed */
	  if (d - dst + strlen(link) + strlen(s) < MAX_PATH_LENGTH - 1) {
	    /* we have enough room */
	    sprintf(d, "%s%s", link, s); 
	    /* Recurse. Be careful for loops (a->b and b->a) */
	    if (count < MAXLINK) 
	      return STk_resolve_link(dst, count+1);
	  }
	  return Ntruth;
	}
	else {
	  if (errno != EINVAL) 
	    /* EINVAL = file is not a symlink (i.e. it's a true error) */
	    return Ntruth;
	  else
	    if (*s) *d = '/'; 
	    else return STk_makestring(dst);		       
	}
      default:   *d = *s;
    }
  }
#endif
}

   
/*
 *----------------------------------------------------------------------
 *
 * fileglob --
 *      *****							        ******
 * 	***** This function is an adaptation of the Tcl function DoGlob ******
 *      ***** Adaptated to use true lists rather than string as in Tcl  ******
 *      *****							        ******
 *	
 *
 *      This recursive procedure forms the heart of the globbing
 *      code.  It performs a depth-first traversal of the tree
 *      given by the path name to be globbed.
 *
 * Results:
 *      The return value is a standard Tcl result indicating whether
 *      an error occurred in globbing.  After a normal return the
 *      result in interp will be set to hold all of the file names
 *      given by the dir and rem arguments.  After an error the
 *      result in interp will hold an error message.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

static SCM fileglob(char *dir, char *rem, SCM result)
/* dir: Name of a directory at which to start glob expansion.  This name
 * is fixed: it doesn't contain any globbing chars. 
 * rem: Path to glob-expand.
 */
{
  /*
   * When this procedure is entered, the name to be globbed may
   * already have been partly expanded by ancestor invocations of
   * fileglob.  The part that's already been expanded is in "dir"
   * (this may initially be empty), and the part still to expand
   * is in "rem".  This procedure expands "rem" one level, making
   * recursive calls to itself if there's still more stuff left
   * in the remainder.
   */
  
  Tcl_DString newName;                /* Holds new name consisting of
				       * dir plus the first part of rem. */
  register char *p;
  register char c;
  char *openBrace, *closeBrace, *name, *dirName;
  int gotSpecial, baseLength;
  struct stat statBuf;

  /*
   * Make sure that the directory part of the name really is a
   * directory.  If the directory name is "", use the name "."
   * instead, because some UNIX systems don't treat "" like "."
   * automatically. Keep the "" for use in generating file names,
   * otherwise "glob foo.c" would return "./foo.c".
   */
  
  dirName = (*dir == '\0') ? ".": dir;
  if ((stat(dirName, &statBuf) != 0) || !S_ISDIR(statBuf.st_mode))
    return result;

  Tcl_DStringInit(&newName);

  /*
   * First, find the end of the next element in rem, checking
   * along the way for special globbing characters.
   */
  
  gotSpecial = 0;
  openBrace = closeBrace = NULL;
  for (p = rem; ; p++) {
    c = *p;
    if ((c == '\0') || ((openBrace == NULL) && (c == '/'))) break;
    if ((c == '{')  && (openBrace == NULL)) openBrace = p;
    if ((c == '}')  && (openBrace != NULL) && (closeBrace == NULL)) closeBrace = p;
    if ((c == '*') || (c == '[') || (c == '\\') || (c == '?')) gotSpecial = 1;
  }

  /*
   * If there is an open brace in the argument, then make a recursive
   * call for each element between the braces.  In this case, the
   * recursive call to fileglob uses the same "dir" that we got.
   * If there are several brace-pairs in a single name, we just handle
   * one here, and the others will be handled in recursive calls.
   */

  if (openBrace != NULL) {
    char *element;
    
    if (closeBrace == NULL) {
      Tcl_DStringFree(&newName);      
      Err("unmatched open-brace in file name", NIL);
    }

    Tcl_DStringAppend(&newName, rem, openBrace-rem);
    baseLength = newName.length;
    for (p = openBrace; *p != '}'; ) {
      element = p+1;
      for (p = element; ((*p != '}') && (*p != ',')); p++) {}
      Tcl_DStringAppend(&newName, element, p-element);
      Tcl_DStringAppend(&newName, closeBrace+1, -1);
      result = fileglob(dir, newName.string, result);
      newName.length = baseLength;
    }
    goto done;
  }

  /*
   * Start building up the next-level name with dir plus a slash if
   * needed to separate it from the next file name.
   */

  Tcl_DStringAppend(&newName, dir, -1);
  if ((dir[0] != 0) && (newName.string[newName.length-1] != '/')) {
    Tcl_DStringAppend(&newName, SDIRSEP, 1);
  }
  baseLength = newName.length;

  /*
   * If there were any pattern-matching characters, then scan through
   * the directory to find all the matching names.
   */
  if (gotSpecial) {
#ifdef _MSC_VER
    WIN32_FIND_DATA wfd; /* VC++ support from Caleb Deupree <cdeupree@erinet.com> */
    HANDLE handle;
    Tcl_DString msvcname;
    char savedChar;
    BOOL bFound = TRUE;

    Tcl_DStringInit(&msvcname);
    Tcl_DStringAppend(&msvcname, dirName, -1);
    Tcl_DStringAppend(&msvcname, SDIRSEP, 1);
    Tcl_DStringAppend(&msvcname, "*\0", -1);
       
    handle = FindFirstFile(Tcl_DStringValue(&msvcname), &wfd);
    if (handle == INVALID_HANDLE_VALUE) {
      Tcl_DStringFree(&msvcname);
      Err("Cannot find files, error = ", STk_makestring((char *) GetLastError()));
    }
   
    savedChar = *p;
    *p = 0;
    
    while (bFound) {
      /*
       * Don't match names starting with "." unless the "." is
       * present in the pattern.
       */
      if ((wfd.cFileName == '.') && (*rem != '.')) continue;
      
      if (Tcl_StringMatch(wfd.cFileName, rem)) {
	newName.length = baseLength;
	Tcl_DStringAppend(&newName, wfd.cFileName, -1);
	if (savedChar == 0)
	  result = Cons(STk_makestring(newName.string), result);
	else {
	  result = fileglob(newName.string, p+1, result);
	  if (result != TCL_OK) break;
	}
      }
      bFound = FindNextFile(handle, &wfd);
    }
    FindClose(handle);
    Tcl_DStringFree(&msvcname);
    *p = savedChar;
    goto done;
#else
    DIR *d;
    struct dirent *entryPtr;
    char savedChar;

    d = opendir(dirName);
    if (d == NULL) {
      Tcl_DStringFree(&newName);
      Err("cannot read directory", STk_makestring(dirName));
    }

    /*
     * Temporarily store a null into rem so that the pattern string
     * is now null-terminated.
     */

    savedChar = *p;
    *p = 0;

    while (1) {
      entryPtr = readdir(d);
      if (entryPtr == NULL) break;
      /*
       * Don't match names starting with "." unless the "." is
       * present in the pattern.
       */
      if ((*entryPtr->d_name == '.') && (*rem != '.')) continue;

      if (Tcl_StringMatch(entryPtr->d_name, rem)) {
	newName.length = baseLength;
	Tcl_DStringAppend(&newName, entryPtr->d_name, -1);
	if (savedChar == 0)
	  result = Cons(STk_makestring(newName.string), result);
	else {
	  result = fileglob(newName.string, p+1, result);
	  if (result != TCL_OK) break;
	}
      }
      
    }
    closedir(d);
    *p = savedChar;
    goto done;
#endif
  }

  /*
   * The current element is a simple one with no fancy features.  Add
   * it to the new name.  If there are more elements still to come,
   * then recurse to process them.
   */
  
  Tcl_DStringAppend(&newName, rem, p-rem);
  if (*p != 0) {
    result = fileglob(newName.string, p+1, result);
    goto done;
  }

  /*
   * There are no more elements in the pattern.  Check to be sure the
   * file actually exists, then add its name to the list being formed
   * in main_interp-result.
   */

  name = newName.string;
  if (*name == 0) name = ".";
  if (access(name, F_OK) != 0) goto done;
  result = Cons(STk_makestring(name), result);
done:
  Tcl_DStringFree(&newName);
  return result;
}

SCM STk_internal_expand_file_name(char *s)
{
  char expanded[2 * MAX_PATH_LENGTH], abs[2 * MAX_PATH_LENGTH];  
  /* Warning: absolute makes no control about path overflow. Hence the "2 *" */

  absolute(tilde_expand(s, expanded), abs);
  return STk_makestring(abs);
}


void STk_whence(char *exec, char *path)
{
  char *p, *q, dir[MAX_PATH_LENGTH];
  struct stat buf;
 
  if (ISABSOLUTE(exec)) {
    strncpy(path, exec, MAX_PATH_LENGTH);
    return;
  }

#ifdef FREEBSD 
  /* I don't understand why this is needed */
  if (access(path, X_OK) == 0) {
    stat(path, &buf);
    if (!S_ISDIR(buf.st_mode)) return;
  }  
#endif

  p = getenv("PATH");
  if (p == NULL) {
    p = "/bin:/usr/bin";
  }

  while (*p) {
    /* Copy the stuck of path in dir */
    for (q = dir; *p && *p != PATHSEP; p++, q++) *q = *p;
    *q = '\000';

    if (!*dir) { 
      /* patch suggested by Erik Ostrom <eostrom@vesuvius.ccs.neu.edu> */
      getcwd(path, MAX_PATH_LENGTH);
      sprintf(path + strlen(path), "%c%s", DIRSEP, exec);
    }
    else
      sprintf(path, "%s%c%s", dir, DIRSEP, exec);

    sprintf(path, "%s%c%s", dir, DIRSEP, exec);
    if (access(path, X_OK) == 0) {
      stat(path, &buf);
      if (!S_ISDIR(buf.st_mode)) return;
    }
	 
    /* Try next path */
    if (*p) p++;
  }
  /* Not found. Set path to "" */
  path[0] = '\0';
}

int STk_dirp(const char *path)
{
  struct stat buf;

  if (stat(path, &buf) >= 0) 
    return S_ISDIR(buf.st_mode);
  return FALSE;
}


/******************************************************************************
 *
 * Primitives
 *
 ******************************************************************************/

PRIMITIVE STk_expand_file_name(SCM s)
{
  if (NSTRINGP(s)) Err("expand-file-name: bad string", s);
  return STk_internal_expand_file_name(CHARS(s));
}

PRIMITIVE STk_canonical_path(SCM str)
{
  if (NSTRINGP(str)) Err("canonical-path: not a string", str);
#ifdef WIN32
  return str;
#else
  return STk_resolve_link(CHARS(str), 0);
#endif
}

PRIMITIVE STk_getcwd(void)
{
  
  char *buf = (char *)getcwd(NULL, MAX_PATH_LENGTH);
  SCM z;

  if (!buf) Err("getcwd: cannot allocate space", NIL);
  z = STk_makestring(buf);
  free(buf);

  return z;
}

PRIMITIVE STk_chdir(SCM s)
{
  if (NSTRINGP(s)) Err("chdir: bad string", s);
  
  if (chdir(CHARS(STk_internal_expand_file_name(CHARS(s)))))
    Err("chdir: cannot change directory to", s);
  return UNDEFINED;
}

PRIMITIVE STk_getpid(void)
{
  return (STk_makeinteger((int) getpid()));
}

PRIMITIVE STk_system(SCM com)
{
  if (NSTRINGP(com)) Err("system: not a string", com);
  return STk_makeinteger(system(CHARS(com)));
}
	 
PRIMITIVE STk_getenv(SCM str)
{
  char *tmp;
  if (NSTRINGP(str)) Err("getenv: not a string", str);
  tmp = getenv(CHARS(str));
  return tmp ? STk_makestring(tmp) : Ntruth;
}

PRIMITIVE STk_setenv(SCM var, SCM value)
{
  char *s;
  if (NSTRINGP(var)) Err("setenv!: variable is not a string", var);
  if (strchr(CHARS(var), '=')) Err("setenv!: variable contains a '='", var);
  if (NSTRINGP(value)) Err("setenv!: value is not a string", value);

  s = STk_must_malloc(strlen(CHARS(var))+ 
		      strlen(CHARS(value)) + 2); /* 2 cause  '=' & \0 */
  sprintf(s, "%s=%s", CHARS(var), CHARS(value));
  putenv(s);
  return UNDEFINED;
}


/******************************************************************************
 *
 * file-is-xxx? primitives
 *
 ******************************************************************************/

static SCM my_access(SCM path, int mode, char *who)
{
  if (NSTRINGP(path)) {
    char buff[100];
    sprintf(buff, "%s: bad string", who);
    Err(buff, path);
  }
  
  return (access(CHARS(path), mode) == 0) ? Truth: Ntruth;
}


static SCM my_stat(SCM path, int mode, char *who)
{
  struct stat info;
  
  if (NSTRINGP(path)) {
    char buff[100];
    sprintf(buff, "%s: bad string", who);
    Err(buff, path);
  }

  if (stat(CHARS(path), &info) != 0) return Ntruth;

  switch (mode) {
    case 1: return (S_ISDIR(info.st_mode)) ? Truth : Ntruth;
    case 2: return (S_ISREG(info.st_mode)) ? Truth : Ntruth;
  }
  return UNDEFINED; /* never reached */
}

PRIMITIVE STk_file_is_directoryp(SCM f)
{
  return my_stat(f, 1, "file-is-directory?");
}

PRIMITIVE STk_file_is_regularp(SCM f)
{
  return my_stat(f, 2, "file-is-regular?");
}

PRIMITIVE STk_file_is_readablep(SCM f)
{
  return my_access(f, R_OK, "file-is-readable?");
}

PRIMITIVE STk_file_is_writablep(SCM f)
{
  return my_access(f, W_OK, "file-is-writable?");
}

PRIMITIVE STk_file_is_executablep(SCM f)
{
  return my_access(f, X_OK, "file-is-executable?");
}

PRIMITIVE STk_file_existp(SCM f)
{
  return my_access(f, F_OK, "file-exists?");
}


PRIMITIVE STk_file_glob(SCM l, int len) /* len is unused here */
{
  SCM res = NIL;
  char s[2*MAX_PATH_LENGTH];
  
  for ( ; NNULLP(l); l = CDR(l)) {
    if (NSTRINGP(CAR(l))) Err("glob: bad string", CAR(l));
    
    tilde_expand(CHARS(CAR(l)), s);

    res = STk_append2(res, (ISDIRSEP(*s)) ? fileglob(SDIRSEP, s+1, NIL) :
		                            fileglob("", s, NIL));
  }
  return res;
}

PRIMITIVE STk_remove_file(SCM filename)
{
  ENTER_PRIMITIVE("remove-file");
  if (NSTRINGP(filename)) Serror("bad string", filename);
  if (remove(CHARS(filename)) != 0)
    Serror("cannot remove file", filename);
  return UNDEFINED;
}

PRIMITIVE STk_rename_file(SCM filename1, SCM filename2)
{
  ENTER_PRIMITIVE("rename-file");
  if (NSTRINGP(filename1)) Serror("bad string", filename1);
  if (NSTRINGP(filename2)) Serror("bad string", filename2);
  if (rename(CHARS(filename1), CHARS(filename2)) != 0)
    Serror("cannot rename file", filename1);
  return UNDEFINED;
}

PRIMITIVE STk_temporary_file_name(void)
{
  char *s;

  s = tmpnam(NULL);
  return s ? STk_makestring(s) : Ntruth;
}


  
    
