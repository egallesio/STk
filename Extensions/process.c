/*
 * p r o c e s s . c 		-- Access to processes from STk
 *
 * Copyright © 1994-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *            Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: ??-???-1994 ??:??
 * Last file update: 14-Sep-1999 15:24 (eg)
 *
 *
 * The implementation for Win32 is a contribution of people from Grammatech 
 * (Paul Anderson <paul@grammatech.com> and Sarah Calvo <sarah@grammatech.com>
 *
 * The main function run-process has been duplicated because the #ifdef were 
 * too "intricated".
 *
 */


/******************************************************************************
 *
 * Process extended type definition
 *
 ******************************************************************************/

#if defined(_WIN32) && !defined(__CYGWIN__)
#  define PURE_WIN32
#endif

#ifdef PURE_WIN32
#  include <windows.h>
#  include <process.h>
#  include <stdlib.h>
#  include <stdio.h>
#  include <time.h>
#  include <io.h>
#  include <fcntl.h>
#  include <sys/stat.h>
#  include "stk.h"

#  define close _close
#  define stat _stat
#  define pipe _pipe
#  define WEXITSTATUS(n) n
#else 					/* ! PURE_WIN32 */
#  include "stk.h"
#  include <fcntl.h>
#  include <errno.h>
#  include <sys/param.h>
#  include <sys/wait.h>
#  include <sys/stat.h>
#  include <unistd.h>
#  include <signal.h>
#endif

static int tc_process;				/* Process signature */

/*
 * Data 
 */

#define MAX_PROC_NUM      40 		/* (simultaneous processes) enough? */

struct process_info {
  int pid;                      /* Process id */
  int index;			/* index of process in the table of processes */
  SCM stream[3];		/* Redirections for stdin stdout and stderr */
  int exited;			/* Process is terminated */
  int exit_status;		/* Exit status of the processus */
  int waited_on;		/* non zero if the process is being
				   waited on by a waitpid(..,..,0) */
};

#define PROCESS(x)        ((struct process_info *)((x)->storage_as.extension.data))
#define LPROCESS(x)	  ((x)->storage_as.extension.data)
#define PROCESSP(x)       (TYPEP ((x), tc_process))
#define NPROCESSP(x)      (NTYPEP ((x), tc_process))
#define PROCPID(x)        (PROCESS(x)->pid)

static SCM proc_arr[MAX_PROC_NUM];   	/* for registering processes */

static char *stdStreams[3] = {
  "input",	
  "output",	
  "error",
};

static char key_inp[]  = ":input";
static char key_out[]  = ":output";
static char key_err[]  = ":error";
static char key_wit[]  = ":wait";
static char key_hst[]  = ":host";
static char key_hide[] = ":hide";
#ifndef WIN32
static char key_fork[] = ":fork";
#endif


#if defined(SIGCHLD) && !defined(HPUX)
#  define USE_SIGCHLD 1 /* What's the problem with HP? */
#endif

#ifdef USE_SIGCHLD
#  define PURGE_PROCESS_TABLE()				    /* Nothing to do */
#else
#  define PURGE_PROCESS_TABLE() process_terminate_handler(0)/* Simulate a SIGCHLD */
#endif


/******************************************************************************/

static void init_proc_table(void)
{
  int i;

  for(i = 0; i<MAX_PROC_NUM; i++) proc_arr[i] = Ntruth;
}


static int find_process(SCM prc)
{
  int i;

  for(i = 0; i<MAX_PROC_NUM; i++)
    if(prc==proc_arr[i]) return i;
  return (-1);
}

static int internal_process_alivep(SCM process)
{
  if (PROCESS(process)->exited) 
    return FALSE;
  else if (PROCESS(process)->waited_on)
    return TRUE;
  else {
#ifdef PURE_WIN32
    int info;

    GetExitCodeProcess((HANDLE)PROCPID(process), &info);
    if (info == STILL_ACTIVE)
      return TRUE;
    else {
      /* process has terminated and we must save this information */
      PROCESS(process)->exited      = TRUE;
      PROCESS(process)->exit_status = info;
      return FALSE;
    }
#else
    int info, res;

    /* Use waitpid to gain the info. */
    res = waitpid(PROCPID(process), &info, WNOHANG);
    if (res == 0)
      /* process is still running */
      return TRUE;
    else 
      if (res == PROCPID(process)) {
	/* process has terminated and we must save this information */
	PROCESS(process)->exited      = TRUE;
	PROCESS(process)->exit_status = info;
	return FALSE;
      }
      else {
	/* might not have found process because we've already waited for it */
	/* if so, then status has already been updated */
	return FALSE;
      }
#endif
  }
}

static void process_terminate_handler(int sig) /* called when a child dies */
{
  register int i;
  SCM proc;

#if defined(USE_SIGCHLD) && !defined(HAVE_SIGACTION)
  static int in_handler = 0;

  signal(SIGCHLD, process_terminate_handler); /* Necessary on System V */
  if (in_handler++) /* Execution is re-entrant */ return;
  
  do {
#endif
    /* Find the process which is terminated 
     * Note that this loop can find:
     *      - nobody: if the process has been destroyed by GC
     *      - 1 process: This is the normal case
     *	    - more than one process: This can arise when:
     *		   - we use signal rather than sigaction 
     *		   - we don't have SIGCHLD and this function is called by
     *		     PURGE_PROCESS_TABLE
     * Sometimes I think that life is a little bit complicated.... 
     */
    for(i = 0; i<MAX_PROC_NUM; i++) {
      proc = proc_arr[i];
      if (PROCESSP(proc) && !internal_process_alivep(proc))
	/* This process has exited. We can delete it from the table */
	proc_arr[i] = Ntruth;
    }

#if defined(USE_SIGCHLD) && !defined(HAVE_SIGACTION)
    /* Since we can be called recursively, we have perhaps forgot to delete 
     * some dead process from the table. So, we have perhaps to scan 
     * the process array another time
     */
  } while (--in_handler > 0);
#endif
}


static SCM make_process(void)
{
  int i;
  SCM z;

  PURGE_PROCESS_TABLE();

  /* find slot */
  i = find_process(Ntruth);
  if (i < 0){
    STk_gc_for_newcell();
    i = find_process(Ntruth);
  }
  if (i < 0) Err("Too many processes", NIL);

  NEWCELL(z, tc_process);
  LPROCESS(z) = (struct process_info *) must_malloc(sizeof(struct process_info));
  PROCESS(z)->index = i;
  PROCESS(z)->stream[0] =  PROCESS(z)->stream[1] = PROCESS(z)->stream[2] = Ntruth;
  PROCESS(z)->waited_on = PROCESS(z)->exit_status = PROCESS(z)->exited = 0;
  /* Enter this process in the process table */
  proc_arr[i] = z;
  return z;
}

#ifdef PURE_WIN32
static void cannot_run(HANDLE pipes[3][2], char **argv, char *msg, SCM obj)
{
  int i;

  for (i=0; i<3; i++) {
    if (pipes[i][0]) CloseHandle(pipes[i][0]);
    if (pipes[i][1]) CloseHandle(pipes[i][1]);
  }
  free(argv);
  STk_procedure_error("run-process", msg, obj);
}
#else
static void cannot_run(int pipes[3][2], char **argv, char *msg, SCM obj)
{
  int i;

  for (i=0; i<3; i++) {
    if (pipes[i][0] != -1) close(pipes[i][0]);
    if (pipes[i][1] != -1) close(pipes[i][1]);
  }
  free(argv);
  STk_procedure_error("run-process", msg, obj);
}
#endif

#ifdef PURE_WIN32
/*===========================================================================*\
 *
 * 		Implementation of run-process for Win32
 *
 \*==========================================================================*/

char *Win32Err(char *msg)
{
  static char errMsg[1024];
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);
  sprintf(errMsg, "%s: %s", msg, lpMsgBuf);
  LocalFree( lpMsgBuf );
  return &errMsg[0];
}

void ShowError(HANDLE pipes[3][2], char **argv, SCM obj, char *msg)
{
  char *errMsg;
  errMsg = Win32Err(msg);
  cannot_run(pipes, argv, errMsg, NIL);
}

static BOOL SameFile(LPTSTR f1, LPTSTR f2)
{
  HANDLE h1;
  HANDLE h2;
  BY_HANDLE_FILE_INFORMATION b1;
  BY_HANDLE_FILE_INFORMATION b2;
    
  h1 = CreateFile(f1, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (h1 == INVALID_HANDLE_VALUE)
    return FALSE;
  h2 = CreateFile(f2, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (h2 == INVALID_HANDLE_VALUE)
    return FALSE;

  if (!GetFileInformationByHandle(h1, &b1))
    return FALSE;
  if (!GetFileInformationByHandle(h2, &b2))
    return FALSE;
  
  return b1.dwVolumeSerialNumber == b2.dwVolumeSerialNumber &&
         b1.nFileIndexHigh == b2.nFileIndexHigh &&
         b1.nFileIndexHigh == b2.nFileIndexHigh;
}

/*
 * Here it is 
 *
 */
static PRIMITIVE run_process(SCM l, int len)
{
  SCM proc, tmp, redirection[3];
  int pid, i, argc, waiting, hidden;
  struct process_info *info;
  char host[100], msg[256], **argv, **argv_start;

  /* Initializations */
  HANDLE pipes[3][2];
  DWORD handleKeys[] = { STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE };
  SECURITY_ATTRIBUTES saAttr;
  STARTUPINFO startupInfo;
  PROCESS_INFORMATION processInfo;
  char *sCmdLine, **aPtr;
  int cmdLineLen;
  BOOL bResult;

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
  startupInfo.dwFlags = STARTF_USESTDHANDLES;
  startupInfo.lpReserved2 = NULL;
  startupInfo.wShowWindow = SW_HIDE; 

  argc = 0; waiting = FALSE; hidden = FALSE;
  argv_start = (char**)must_malloc((len+3)*sizeof(char *)); /* 3= NULL+rsh+host */
  argv = argv_start + 2;
  
  for (i = 0; i < 3; i++) {
    redirection[i] = NIL;
    pipes[i][0] =  pipes[i][1] = NULL;
  }

  /* Checking arguments and creating UNIX-style arguments list */
  for (  ; NNULLP(l); l = CDR(l)) {
    tmp = CAR(l);
    if (KEYWORDP(tmp)) {
      /* Manage :input, :output, :error and :no-wait keywords */
      int i = -1;

      if (NCONSP(CDR(l))) 
	cannot_run(pipes, argv_start,"no argument after keyword", tmp);
      
      l = CDR(l); /* Go to next item */
      
      if (STk_eqv(tmp, STk_makekey(key_hst)) == Truth) {
	/* :host keyword processing */
	if (NSTRINGP(CAR(l)))
	  cannot_run(pipes, argv_start, "string expected. It was", CAR(l));
	strcpy(host, CHARS(CAR(l))); /* to avoid GC problems */
	/* Shift argv to point the start of allocated zone. This avoid a copy
	 * of arguments already processed.
	 */
	argv    = argv_start;
	argc   += 2;
	argv[0] = "rsh";
	argv[1] = host;
      }
      else {
	if (STk_eqv(tmp, STk_makekey(key_wit)) == Truth) {
	  /* :wait option processing */
	  if (NBOOLEANP(CAR(l))) 
	    cannot_run(pipes, argv_start, "boolean expected. It was", CAR(l));
	  
	  waiting = (CAR(l) == Truth);
	}
        else if (STk_eqv(tmp, STk_makekey(key_hide)) == Truth) {
	  /* :hide option processing */
	  if (NBOOLEANP(CAR(l))) 
	    cannot_run(pipes, argv_start, "boolean expected. It was", CAR(l));
	  
	  hidden = (CAR(l) == Truth);
	}
	else {
	  /* :input, :output, :error option processing */
	  if (STk_eqv(tmp, STk_makekey(key_inp)) == Truth) i = 0; else
	  if (STk_eqv(tmp, STk_makekey(key_out)) == Truth) i = 1; else
	  if (STk_eqv(tmp, STk_makekey(key_err)) == Truth) i = 2;
	  
	  if (i < 0) cannot_run(pipes, argv_start, "bad keyword", tmp);
	  redirection[i] = CAR(l);
	  
	  if (STRINGP(redirection[i])) {
	    /* Redirection in a file */
	    int j;

	    /* 
	     * First try to look if this redirecttion has not already done
	     * This can arise by doing
	     *     :output "out" :error "out"       which is correct
	     *     :output "out" :input "out"       which is obviously incorrect
	     */
	    for (j = 0; j < 3; j++) {
	      if (j != i && STRINGP(redirection[j])) {
		if (SameFile(CHARS(redirection[i]), CHARS(redirection[j]))) {
		  if (i == 0 || j == 0) {
		    sprintf(msg, "read/write on the same file: %s", 
			    CHARS(redirection[i]));
		    cannot_run(pipes, argv_start, msg, NIL);
		  }
		  break;
		}
	      }
	    }

	    /* 
	     * Two cases are possible here:
	     * 	  - We have stdout and stderr redirected on the same file (j != 3)
	     *	  - We have not found current file in list of redirections (j == 3)
	     */
	    if (j == 3) {
	      pipes[i][0] = CreateFile (CHARS(redirection[i]),
					i==0 ? GENERIC_READ : GENERIC_WRITE,
					FILE_SHARE_READ,
					&saAttr,
					i==0 ? OPEN_EXISTING : TRUNCATE_EXISTING,
					0L, NULL);
	      if (pipes[i][0] == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND &&
		    i > 0 &&
		    (pipes[i][0] = CreateFile(CHARS(redirection[i]), 
					      GENERIC_WRITE, FILE_SHARE_READ,
					      &saAttr, CREATE_ALWAYS, 0L, NULL)) 
		                 != INVALID_HANDLE_VALUE)
		  /* nothing */;
		else
		  ShowError(pipes, argv_start, NIL, "CreateFile");
	      }
	    }
	    
	    if(pipes[i][0] < 0) {
	      sprintf(msg, "can't redirect standard %s to file %s",
		      stdStreams[i], CHARS(redirection[i]));
	      cannot_run(pipes, argv_start, msg, NIL);
	    }
	  }
	  else 
	    if (KEYWORDP(redirection[i])) {
	      /* Redirection in a pipe */
	      BOOL bSuccess;
	      HANDLE hProcess;
	      HANDLE hRead;
	      HANDLE hWrite;
	      bSuccess = CreatePipe(&hRead,
				    &hWrite,
				    &saAttr,
				    0);
	      if (!bSuccess) {
		ShowError(pipes, argv_start, NIL, "CreatePipe");
	      }
	      hProcess = GetCurrentProcess();
	      /* This duplicate is necessary in order to prevent the child process
		 inheriting the other end of the pipe.  For example, for the stdin
		 pipe, I want the child to inherit the read end, but NOT the
		 write end. */
	      bSuccess = DuplicateHandle(hProcess,
					 i==0 ? hWrite : hRead,
					 hProcess,
					 &pipes[i][i==0 ? 1 : 0],
					 0,
					 FALSE,
					 DUPLICATE_SAME_ACCESS);
	      if (!bSuccess) {
		ShowError(pipes, argv_start, NIL, 
			  "DuplicateHandle for Pipe (read)");
	      }
	      pipes[i][i==0 ? 0 : 1] = i==0 ? hRead : hWrite;
	      CloseHandle(i==0 ? hWrite : hRead);
	    }
	}
      }
    }
    else {
      /* Normal arg. Put it in argv */
      if (NSTRINGP(tmp)) 
	cannot_run(pipes, argv_start, "bad string", tmp);
      argv[argc++] = CHARS(tmp);
    }
  }
  argv[argc] = NULL;
  
  if (argc == 0) cannot_run(pipes, argv_start,"no command given", NIL);

  /* Build a process object */
  proc = make_process();
  info  = PROCESS(proc);
  
  if (pipes[0][0])
    startupInfo.hStdInput = pipes[0][0];
  else
    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

  if (pipes[1][KEYWORDP(redirection[1]) ? 1 : 0])
    startupInfo.hStdOutput = pipes[1][KEYWORDP(redirection[1]) ? 1 : 0];
  else
    startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  
  if (pipes[2][KEYWORDP(redirection[2]) ? 1 : 0])
    startupInfo.hStdError = pipes[2][KEYWORDP(redirection[2]) ? 1 : 0];
  else
    startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

  if (hidden)
    startupInfo.dwFlags |= STARTF_USESHOWWINDOW;

  cmdLineLen = strlen(*argv) + 1;
  sCmdLine = must_malloc(cmdLineLen);
  strcpy(sCmdLine, *argv);
  for (aPtr = argv+1 ; *aPtr != NULL ; aPtr++) {
    char *t;
    
    cmdLineLen += strlen(*aPtr) + 2;
    t = must_malloc(cmdLineLen);
    sprintf(t, "%s %s", sCmdLine, *aPtr);
    free(sCmdLine);
    sCmdLine = t;
  }
  
  bResult = CreateProcess
      (NULL,
       sCmdLine, /* the command line */
       NULL,     /* pointer to process security attributes */
       NULL,     /* pointer to thread security attributes */
       TRUE,     /* Inherit */
       0L,       /* creation flags */
       NULL,     /* pointer to new environment */
       NULL,     /* pointer to current directory name */
       &startupInfo,
       &processInfo);

  
  if (!bResult) {/* zero means failure */
    ShowError(pipes, argv_start, proc, "CreateProcess");
  }
  
  free(sCmdLine);
  info->pid = pid = (int)processInfo.hProcess;

  for(i = 0; i < 3; i++) {
    if (STRINGP(redirection[i])) {
      /* Redirection in a file */
      bResult = CloseHandle(pipes[i][0]);
      if (!bResult) 
	Err(Win32Err("CloseHandle(pipes][i][0])"), NIL);
    }
    else 
      if (KEYWORDP(redirection[i])) {
	/* Redirection in a pipe */
	bResult = CloseHandle(pipes[i][i == 0 ? 0 : 1]);
	if (!bResult) 
	  Err(Win32Err("CloseHandle(pipes][i][0])"), NIL);
	
	/* Make a new file descriptor to access the pipe */
	{
	  char *s;
	  FILE *f;
	  HANDLE h;
	  int flags;

	  int fd;
	  h = i == 0 ? pipes[i][1] : pipes[i][0];
	  flags = i==0 ? _O_APPEND : _O_RDONLY;
	  /* see (Q99173) with the title 'Types of File I/O Under Win32' */
	  fd = _open_osfhandle((long) h, flags);

	  f = fdopen(fd, i==0 ? "w" : "r");
	  if (f == NULL)
	    cannot_run(pipes, argv_start, "run-process: cannot fdopen", proc);

	  sprintf(msg, "pipe-%s-%d", stdStreams[i], pid);

	  s = (char *) must_malloc(strlen(msg)+1);
	  strcpy(s, msg);

	  info->stream[i] = STk_Cfile2port(s,
					   f,
					   (i==0) ? tc_oport : tc_iport,
					   0);
	}
      }
  }
  if (waiting) {
    if (WaitForSingleObject(processInfo.hProcess, INFINITE) == WAIT_FAILED) {
      char *errMsg;
      errMsg = Win32Err("WaitForSingleObject");
      Err(errMsg, NIL);
    }
    else {
      GetExitCodeProcess(processInfo.hProcess, &(info->exit_status));
      info->exited = TRUE;
    }
  } 

  free(argv_start);
  return proc;
}

#else 
/*===========================================================================*\
 *
 * 		Implementation of run-process for Unix
 *
 \*==========================================================================*/

static PRIMITIVE run_process(SCM l, int len)
{
  SCM proc, tmp, redirection[3];
  int pid, i, argc, waiting, hidden, do_fork;
  struct process_info *info;
  char host[100], msg[256], **argv, **argv_start;

  /* Initializations */
  int pipes[3][2];

  argc = 0; waiting = FALSE; hidden = FALSE; do_fork = TRUE;
  argv_start = (char**)must_malloc((len+3)*sizeof(char *)); /* 3= NULL+rsh+host */
  argv = argv_start + 2;
  
  for (i = 0; i < 3; i++) {
    redirection[i] = NIL;
    pipes[i][0] =  pipes[i][1] = -1;
  }

  /* Checking arguments and creating UNIX-style arguments list */
  for (  ; NNULLP(l); l = CDR(l)) {
    tmp = CAR(l);
    if (KEYWORDP(tmp)) {
      /* Manage :input, :output, :error and :no-wait keywords */
      int i = -1;

      if (NCONSP(CDR(l))) 
	cannot_run(pipes, argv_start,"no argument after keyword", tmp);
      
      l = CDR(l); /* Go to next item */
      
      if (STk_eqv(tmp, STk_makekey(key_hst)) == Truth) {
	/* :host keyword processing */
	if (NSTRINGP(CAR(l)))
	  cannot_run(pipes, argv_start, "string expected. It was", CAR(l));
	strcpy(host, CHARS(CAR(l))); /* to avoid GC problems */
	/* Shift argv to point the start of allocated zone. This avoid a copy
	 * of arguments already processed.
	 */
	argv    = argv_start;
	argc   += 2;
	argv[0] = "rsh";
	argv[1] = host;
      }
      else {
	if (STk_eqv(tmp, STk_makekey(key_wit)) == Truth) {
	  /* :wait option processing */
	  if (NBOOLEANP(CAR(l))) 
	    cannot_run(pipes, argv_start, "boolean expected. It was", CAR(l));
	  
	  waiting = (CAR(l) == Truth);
	}
        else if (STk_eqv(tmp, STk_makekey(key_hide)) == Truth) {
	  /* :hide option processing */
	  if (NBOOLEANP(CAR(l)))
	    cannot_run(pipes, argv_start, "boolean expected. It was", CAR(l));
	  
	  hidden = (CAR(l) == Truth);
	}
        else if (STk_eqv(tmp, STk_makekey(key_fork)) == Truth) {
	  /* :fork option processing */
	  if (NBOOLEANP(CAR(l)))
	    cannot_run(pipes, argv_start, "boolean expected. It was", CAR(l));
	  
	  do_fork = (CAR(l) == Truth);
	}
	else {
	  /* :input, :output, :error option processing */
	  if (STk_eqv(tmp, STk_makekey(key_inp)) == Truth) i = 0; else
	  if (STk_eqv(tmp, STk_makekey(key_out)) == Truth) i = 1; else
	  if (STk_eqv(tmp, STk_makekey(key_err)) == Truth) i = 2;
	  
	  if (i < 0) cannot_run(pipes, argv_start, "bad keyword", tmp);
	  redirection[i] = CAR(l);
	  
	  if (STRINGP(redirection[i])) {
	    /* Redirection in a file */
	    int j;

	    /* 
	     * First try to look if this redirecttion has not already done
	     * This can arise by doing
	     *     :output "out" :error "out"       which is correct
	     *     :output "out" :input "out"       which is obviously incorrect
	     */
	    for (j = 0; j < 3; j++) {
	      if (j != i && STRINGP(redirection[j])) {
		struct stat stat_i, stat_j;
		
		/* Do a stat to see if we try to open the same file 2 times     */
		/* if stat == -1 this is probably because file doen't exist yet */
		if (stat(CHARS(redirection[i]), &stat_i) == -1) continue;
		if (stat(CHARS(redirection[j]), &stat_j) == -1) continue;
		
		if (stat_i.st_dev==stat_j.st_dev && stat_i.st_ino==stat_j.st_ino) {
		  /* Same file was cited 2 times */
		  if (i == 0 || j == 0) {
		    sprintf(msg, "read/write on the same file: %s", 
			         CHARS(redirection[i]));
		    cannot_run(pipes, argv_start, msg, NIL);
		  }
		  
		  /* assert(i == 1 && j == 2 || i == 2 && j == 1); */
		  pipes[i][0] = dup(pipes[j][0]);
		  break;
		}
	      }
	    }		

	    /* 
	     * Two cases are possible here:
	     * 	  - We have stdout and stderr redirected on the same file (j != 3)
	     *	  - We have not found current file in list of redirections (j == 3)
	     */
	    if (j == 3) {
	      pipes[i][0] = open(CHARS(redirection[i]), 
				 i==0 ? O_RDONLY:(O_WRONLY|O_CREAT|O_TRUNC),
				 0666);
	    }
	    
	    if(pipes[i][0] < 0) {
	      sprintf(msg, "can't redirect standard %s to file %s",
		      stdStreams[i], CHARS(redirection[i]));
	      cannot_run(pipes, argv_start, msg, NIL);
	    }
	  }
	  else 
	    if (KEYWORDP(redirection[i])) {
	      /* Redirection in a pipe */
	      if (pipe(pipes[i]) < 0) {
		sprintf(msg, "can't create stream for standard %s",
			stdStreams[i]);
		cannot_run(pipes, argv_start, msg, NIL);
	      }
	    }
	}
      }
    }
    else {
      /* Normal arg. Put it in argv */
      if (NSTRINGP(tmp)) 
	cannot_run(pipes, argv_start, "bad string", tmp);
      argv[argc++] = CHARS(tmp);
    }
  }
  argv[argc] = NULL;

  if (argc == 0) cannot_run(pipes, argv_start,"no command given", NIL);

  /* Build a process object */
  proc = make_process();
  info  = PROCESS(proc);
  pid   = do_fork? fork(): 0;

  /* Fork another process */
  switch (pid) {
    case -1: cannot_run(pipes,argv,"can't create child process", NIL);
    case 0:  /* Child */
      	     for(i = 0; i < 3; i++) {
	       if (STRINGP(redirection[i])) {
		 /* Redirection in a file */
		 close(i);
		 dup(pipes[i][0]);
		 close(pipes[i][0]);
	       }
	       else 
		 if (KEYWORDP(redirection[i])) {
		   /* Redirection in a pipe */
		   close(i);
		   dup(pipes[i][i==0? 0 : 1]);
		   close(pipes[i][0]);
		   close(pipes[i][1]);
		 }
	     }

	     for(i = 3; i < NOFILE; i++) close(i);

	     /*  And then, EXEC'ing...  */
	     execvp(*argv, argv);
	     
	     /* Cannot exec if we are here */
	     Fprintf(STk_curr_eport, "**** Cannot  exec %s!\n", *argv);
	     exit(1);
    default: /* Father */
      	     info->pid = pid;
      	     for(i = 0; i < 3; i++) {
	       if (STRINGP(redirection[i]))
		 /* Redirection in a file */
		 close(pipes[i][0]);
	       else 
		 if (KEYWORDP(redirection[i])) {
		   /* Redirection in a pipe */
		   close(pipes[i][i == 0 ? 0 : 1]);
		   
		   /* Make a new file descriptor to access the pipe */
		   {
		     char *s;
		     FILE *f;

		     f = (i == 0)? fdopen(pipes[i][1],"w"):fdopen(pipes[i][0],"r");
		     if (f == NULL)
		       cannot_run(pipes, argv, "cannot fdopen", proc);

		     sprintf(msg, "pipe-%s-%d", stdStreams[i], pid);

		     s = (char *) must_malloc(strlen(msg)+1);
		     strcpy(s, msg);

		     info->stream[i] = STk_Cfile2port(s,
						      f,
						      (i==0) ? tc_oport : tc_iport,
						      0);
		   }
		 }
	     }
	     if	(waiting) {
	       info->waited_on = 1;
	       waitpid(pid, &(info->exit_status), 0);
	       info->waited_on = 0;
	       info->exited = TRUE;
	     }
  }
  free(argv_start);
  return proc;
}
#endif /* PURE_WIN32 */





static PRIMITIVE processp(SCM process) 
{
  return PROCESSP(process) ? Truth : Ntruth;
}

static PRIMITIVE process_alivep(SCM process)
{
  if (NPROCESSP(process)) Err("process-alive?: bad process", process);
  return internal_process_alivep(process)? Truth: Ntruth;
}

static PRIMITIVE process_pid(SCM process)
{
  if (NPROCESSP(process)) Err("process-pid: bad process", process);
  return  STk_makeinteger(PROCPID(process));
}

static PRIMITIVE process_list(void)
{
  int i;
  SCM lst = NIL;

  PURGE_PROCESS_TABLE();

  for(i = 0; i < MAX_PROC_NUM; i++)
    if (proc_arr[i] != Ntruth)
      lst = Cons(proc_arr[i], lst);
  return lst;
}


static PRIMITIVE process_input(SCM process)
{
  if(NPROCESSP(process)) Err("process-input: bad process", process);

  return PROCESS(process)->stream[0];
}

static PRIMITIVE process_output(SCM process)
{
  if(NPROCESSP(process)) Err("process-output: bad process", process);

  return PROCESS(process)->stream[1];
}

static PRIMITIVE process_error(SCM process)
{
  if(NPROCESSP(process)) Err("process-error: bad process", process);

  return PROCESS(process)->stream[2];
}

static PRIMITIVE process_wait(SCM process)
{
  struct process_info *proc;
  
  PURGE_PROCESS_TABLE();

  if(NPROCESSP(process)) Err("process-wait: bad process", process);

  proc = PROCESS(process);
  if (proc->exited) return Ntruth;
  else {
    int res, info;
    SCM ret_val;
    
    proc->waited_on = 1;
#ifdef PURE_WIN32
    WaitForSingleObject((HANDLE)PROCPID(process), INFINITE);
    GetExitCodeProcess((HANDLE)PROCPID(process), &proc->exit_status);
    
    ret_val = Truth;
#else
    res = waitpid(PROCPID(process), &info, 0);
    if (res == PROCPID(process)) {
      proc->exit_status = info;
      ret_val =  Truth;
    }
    else
      ret_val = Ntruth;
#endif
    proc->waited_on = 0;
    proc->exited    = TRUE;
    return ret_val;
  }
}


static PRIMITIVE process_xstatus(SCM process)
{
  int info, n, res;

  PURGE_PROCESS_TABLE();

  if (NPROCESSP(process)) Err("process-exit-status: bad process", process);
  
  if (PROCESS(process)->exited) {
#ifdef PURE_WIN32
    n = PROCESS(process)->exit_status;
#else
    if (WIFSIGNALED(PROCESS(process)->exit_status))
      n = WCOREDUMP(PROCESS(process)->exit_status);
    else
      n = WEXITSTATUS(PROCESS(process)->exit_status);
#endif
  }
  else {
#ifdef PURE_WIN32
    GetExitCodeProcess((HANDLE)PROCPID(process), &res);
    info = res;
    if (res == STILL_ACTIVE) /* Process is still running */
      return Ntruth;
    else {
      /* Process is now terminated */
      PROCESS(process)->exited      = TRUE;
      PROCESS(process)->exit_status = info;
      n = WEXITSTATUS(info);
    }
#else
    res = waitpid(PROCPID(process), &info, WNOHANG);
    if (res == 0) {
      /* Process is still running */
      return Ntruth;
    }
    else if (res == PROCPID(process)) {
      /* Process is now terminated */
      PROCESS(process)->exited      = TRUE;
      PROCESS(process)->exit_status = info;
      n = WEXITSTATUS(info);
    }
    else
      return Ntruth;
#endif
  }
  return STk_makeinteger((long) n);
}


#ifndef PURE_WIN32
static PRIMITIVE process_send_signal(SCM process, SCM signal)
{
  ENTER_PRIMITIVE("process-send-signal");
  PURGE_PROCESS_TABLE();

  if (NPROCESSP(process)) Serror("bad process", process);
  if (NINTEGERP(signal))  Serror("bad integer", signal);

  kill(PROCPID(process), STk_integer_value(signal));
  return UNDEFINED;
}
#endif 


static PRIMITIVE process_kill(SCM process)
{
#ifdef PURE_WIN32
  TerminateProcess((HANDLE)PROCPID(process), 0);
  return UNDEFINED;
#else
  if (NPROCESSP(process)) Err("process-kill: bad process", process);
  return process_send_signal(process, STk_makeinteger(SIGTERM));
#endif
}


#ifdef SIGSTOP
static PRIMITIVE process_stop(SCM process)
{
  if (NPROCESSP(process)) Err("process-stop: bad process", process);
  return process_send_signal(process, STk_makeinteger(SIGSTOP));
}
#endif

#ifdef SIGCONT
static PRIMITIVE process_continue(SCM process)
{
  if (NPROCESSP(process)) Err("process-continue: bad process", process);
  return process_send_signal(process, STk_makeinteger(SIGCONT));
}
#endif


/******************************************************************************/
static void mark_process(SCM process)
{
  struct process_info *info;
  
  info = PROCESS(process);
  STk_gc_mark(info->stream[0]); 
  STk_gc_mark(info->stream[1]); 
  STk_gc_mark(info->stream[2]);
}
  
static void free_process(SCM process)
{
  int i;

  /* Kill process; close its associated file, delete it from the process table 
   * and free the memory it uses 
   */
  process_kill(process);
  for(i = 0; i < 3; i++) {
    SCM p = PROCESS(process)->stream[i];
    if (IPORTP(p) || OPORTP(p)) STk_close_port(p);
  }
  proc_arr[PROCESS(process)->index] = Ntruth;
  free(PROCESS(process));
}

static void process_display(SCM obj, SCM port, int mode)
{
  sprintf(STk_tkbuffer, "#<process PID=%d>", PROCPID(obj));
  Puts(STk_tkbuffer, port);
}


static STk_extended_scheme_type process_type = {
  "process",		/* name */
  0,			/* is_procp */
  mark_process,		/* gc_mark_fct */
  free_process,        	/* gc_sweep_fct */
  NULL,			/* apply_fct */
  process_display	/* display_fct */
};



/******************************************************************************/

PRIMITIVE STk_init_process(void)
{
  tc_process = STk_add_new_type(&process_type);
  init_proc_table();

#ifdef USE_SIGCHLD
  /* 
   * On systems which support SIGCHLD, the processes table is cleaned up
   * as soon as a process terminate. On other systems this is done from time
   * to time to avoid filling the table too fast
   */

# ifdef HAVE_SIGACTION
  {
    /* Use the secure Posix.1 way */
    struct sigaction sigact;
    
    sigemptyset(&(sigact.sa_mask));
    sigact.sa_handler = process_terminate_handler;
    sigact.sa_flags   = SA_NOCLDSTOP;     /* Ignore SIGCHLD generated by SIGSTOP */
#  ifdef SA_RESTART
    /* Thanks to Harvey J. Stein <hjstein@MATH.HUJI.AC.IL> for the fix */
    sigact.sa_flags  |= SA_RESTART;
#  endif
    sigaction(SIGCHLD, &sigact, NULL);
  }
# else
  /* Use "classical" way. (Only Solaris 2 seems to have problem with it */
  signal(SIGCHLD, process_terminate_handler);
# endif
#endif

  STk_add_new_primitive("run-process",	   	   tc_lsubr,  run_process);
  STk_add_new_primitive("process?", 	   	   tc_subr_1, processp);
  STk_add_new_primitive("process-alive?", 	   tc_subr_1, process_alivep);
  STk_add_new_primitive("process-pid", 	   	   tc_subr_1, process_pid);
  STk_add_new_primitive("process-list", 	   tc_subr_0, process_list);
  STk_add_new_primitive("process-input", 	   tc_subr_1, process_input);
  STk_add_new_primitive("process-output", 	   tc_subr_1, process_output);
  STk_add_new_primitive("process-error", 	   tc_subr_1, process_error);
  STk_add_new_primitive("process-wait", 	   tc_subr_1, process_wait);    
  STk_add_new_primitive("process-exit-status", 	   tc_subr_1, process_xstatus);
  STk_add_new_primitive("process-kill", 	   tc_subr_1, process_kill);

#ifndef PURE_WIN32
  STk_add_new_primitive("process-send-signal",     tc_subr_2, process_send_signal);
#endif
#ifdef SIGSTOP
  STk_add_new_primitive("process-stop", 	   tc_subr_1, process_stop);
#endif
#ifdef SIGCONT
  STk_add_new_primitive("process-continue",	   tc_subr_1, process_continue);
#endif
  return UNDEFINED;
}
