/******************************************************************************
 *
 * Process extended type definition
 *
 ******************************************************************************/

#include "stk.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

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

static char key_inp[] = ":input";
static char key_out[] = ":output";
static char key_err[] = ":error";
static char key_wit[] = ":wait";
static char key_hst[] = ":host";

#if defined(SIGCHLD) && !defined(HPUX)
#  define USE_SIGCHLD 1 /* What's the problem with HP? */
#endif

#ifdef USE_SIGCHLD
#define PURGE_PROCESS_TABLE()	/* Nothing to do */
#else
#define PURGE_PROCESS_TABLE() process_terminate_handler(0) /* Simulate a SIGCHLD */
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
  int info, res;

  if (PROCESS(process)->exited) 
    return FALSE;
  else {
    /* Use waitpid to gain the info. */
    res = waitpid(PROCPID(process), &info, WNOHANG);
    if (res == 0) 
      /* process is still running */
      return TRUE;
    else {
      /* process has terminated and we must save this information */
      PROCESS(process)->exited      = TRUE;
      PROCESS(process)->exit_status = info;
      return FALSE;
    }
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
  PROCESS(z)->exit_status = PROCESS(z)->exited = 0;
  /* Enter this process in the process table */
  proc_arr[i] = z;
  return z;
}


static void cannot_run(int pipes[3][2], char **argv, char *msg, SCM obj)
{
  int i;

  for (i=0; i<3; i++) {
    if (pipes[i][0] != -1) close(pipes[i][0]);
    if (pipes[i][1] != -1) close(pipes[i][1]);
  }
  free(argv);
  Err(msg, obj);
}


static PRIMITIVE run_process(SCM l, int len)
{
  SCM proc, tmp, redirection[3];
  int pid, i, argc, waiting, pipes[3][2];
  struct process_info *info;
  char host[100], msg[256], **argv, **argv_start;

  /* Initializations */
  argc = 0; waiting = FALSE;
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
	cannot_run(pipes, argv_start,"run-process: no argument after keyword", tmp);
      
      l = CDR(l); /* Go to next item */
      
      if (STk_eqv(tmp, STk_makekey(key_hst)) == Truth) {
	/* :host keyword processing */
	if (NSTRINGP(CAR(l)))
	  cannot_run(pipes, argv_start, 
		     "run-process: string expected. It was", CAR(l));
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
	    cannot_run(pipes, argv_start,
		       "run-process: boolean expected. It was", CAR(l));
	  
	  waiting = (CAR(l) == Truth);
	}
	else {
	  /* :input, :output, :error option processing */
	  if (STk_eqv(tmp, STk_makekey(key_inp)) == Truth) i = 0; else
	  if (STk_eqv(tmp, STk_makekey(key_out)) == Truth) i = 1; else
	  if (STk_eqv(tmp, STk_makekey(key_err)) == Truth) i = 2;
	  
	  if (i < 0) cannot_run(pipes, argv_start, "run-process: bad keyword", tmp);
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
		    sprintf(msg, "run-process: read/write on the same file: %s",
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
	      sprintf(msg, "run-process: can't redirect standard %s to file %s",
		      stdStreams[i], CHARS(redirection[i]));
	      cannot_run(pipes, argv_start, msg, NIL);
	    }
	  }
	  else 
	    if (KEYWORDP(redirection[i])) {
	      /* Redirection in a pipe */
	      if (pipe(pipes[i]) < 0) {
		sprintf(msg, "run-process: can't create stream for standard %s",
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
	cannot_run(pipes, argv_start, "run-process: bad string", tmp);
      argv[argc++] = CHARS(tmp);
    }
  }
  argv[argc] = NULL;
  
  if (argc == 0) cannot_run(pipes, argv_start,"run-process: no command given", NIL);

  /* Build a process object */
  proc = make_process();
  info  = PROCESS(proc);
  
  /* Fork another process */
  switch (pid = fork()) {
    case -1: cannot_run(pipes,argv,"run-process: can't create child process", NIL);
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
	     fprintf(STk_stderr, "**** Cannot  exec %s!\n", *argv);
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
		       cannot_run(pipes, argv, "run-process: cannot fdopen", proc);

		     sprintf(msg, "pipe-%s-%d", stdStreams[i], pid);

		     STk_disallow_sigint();

		     s = (char *) must_malloc(strlen(msg)+1);
		     strcpy(s, msg);

		     info->stream[i] = STk_Cfile2port(s,
						      f,
						      (i==0) ? tc_oport : tc_iport,
						      0);
		     STk_allow_sigint();
		   }
		 }
	     }
	     if	(waiting) {
	       waitpid(pid, &(info->exit_status), 0);
	       info->exited = TRUE;
	     }
  }
  free(argv_start);
  return proc;
}


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
  PURGE_PROCESS_TABLE();

  if(NPROCESSP(process)) Err("process-wait: bad process", process);
  
  if (PROCESS(process)->exited) return Ntruth;
  else {
    int ret = waitpid(PROCPID(process), &PROCESS(process)->exit_status, 0);

    PROCESS(process)->exited = TRUE;
    return (ret == 0) ? Ntruth : Truth;
  }
}


static PRIMITIVE process_xstatus(SCM process)
{
  int info, n;

  PURGE_PROCESS_TABLE();

  if (NPROCESSP(process)) Err("process-exit-status: bad process", process);
  
  if (PROCESS(process)->exited) n = PROCESS(process)->exit_status;
  else {
    if (waitpid(PROCPID(process), &info, WNOHANG) == 0) {
      /* Process is still running */
      return Ntruth;
    }
    else {
      /* Process is now terminated */
      PROCESS(process)->exited      = TRUE;
      PROCESS(process)->exit_status = info;
      n = WEXITSTATUS(info);
    }
  }
  return STk_makeinteger((long) n);
}

static PRIMITIVE process_send_signal(SCM process, SCM signal)
{
  PURGE_PROCESS_TABLE();

  if (NPROCESSP(process)) Err("process-send-signal: bad process", process);
  if (NINTEGERP(signal))  Err("process-send-signal: bad integer", signal);

  kill(PROCPID(process), STk_integer_value(signal));
  return UNDEFINED;
}

static PRIMITIVE process_kill(SCM process)
{
  if (NPROCESSP(process)) Err("process-kill: bad process", process);
  return process_send_signal(process, STk_makeinteger(SIGTERM));
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
  Puts(STk_tkbuffer, PORT_FILE(port));
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
  STk_add_new_primitive("process-send-signal",     tc_subr_2, process_send_signal);
  STk_add_new_primitive("process-kill", 	   tc_subr_1, process_kill);       
#ifdef SIGSTOP
  STk_add_new_primitive("process-stop", 	   tc_subr_1, process_stop);       
#endif
#ifdef SIGCONT
  STk_add_new_primitive("process-continue",	   tc_subr_1, process_continue);   
#endif
  return UNDEFINED;
}
