#include	"stk.h"
#include	<fcntl.h>
#include	<errno.h>
#include	<sys/param.h>
#include	<sys/wait.h>
#include  <unistd.h>
#include	<signal.h>


#define MAX_PROC_NUM                            256 /*enough eh?*/
#define	MAX_ARGS_NO				256

#define	NO_REDIRECTION	       	0
#define	REDIRECTION_BY_FILE    	1
#define	REDIRECTION_BY_STREAM	2


/******** SIGUSR1 handler *******/
static void su1_handler(){
/*  printf("SIGUSR1 arrived\n"); */
}
/*********************************/

/**** Registering processes ****/
static SCM proc_arr[MAX_PROC_NUM];


static init_proc_table(){
  int i;
  for(i = 0; i<MAX_PROC_NUM; i++)
    proc_arr[i] = ntruth;
}

static int find_process(SCM prc){
  int i;
  int ret = -1;
  for(i = 0; i<MAX_PROC_NUM; i++){
    if(prc==proc_arr[i]){
      ret = i;
      break;
    }
  }
  return ret;
}

static int reg_process(SCM prc){
  int i;
  /* find slot */
  i = find_process(ntruth);
  if(i<0){
    gc_for_newcell();
    i = find_process(ntruth);
  }
  if (i < 0){
    err("Too many processes", NIL);
    return -1;
  }
  proc_arr[i] = prc;
  return 0;
}

static 
int find_slot(){
  int i;
  /* find slot */
  i = find_process(ntruth);
  if(i<0){
    gc_for_newcell();
    i = find_process(ntruth);
  }
  if (i < 0){
    err("Too many processes", NIL);
    return -1;
  }
  return i;
}
       
static int remove_process(SCM prc){
  int i;
  /* find slot */
  i = find_process(prc);
  if(i<0){
    err("unregistered process", prc);
    return -1;
  }
  proc_arr[i] = ntruth;
  return 0;
}

 



/**** gc-helpers *****/
static void free_process( SCM process );
static void mark_process( SCM process );



static int tc_process;

static extended_scheme_type process_type = {
  "process",		/* name */
  0,			/* is_procp */
  mark_process,		/* gc_mark_fct */
  free_process,	        /* gc_sweep_fct */
  NULL,			/* apply_fct */
  NULL			/* display_fct */
};


 
struct process_info {
  int pid;                          /* pid */
  char  *commandLine;		/* Cmdline used to start process */
  char  redirection[3];		/* Types of redirection	       */
  struct obj   *stream[3];
};

#define PROCESS(x) ((struct process_info *)(x->storage_as.extension.data))
#define PROCESSP(x) (TYPEP (x, tc_process))
#define NPROCESSP(x) (NTYPEP (x, tc_process))
#define PROCPID(x)  PROCESS(x)->pid


extern char **sys_errlist;

static char	*stdStreams[3] = {
	"standard input",	"standard output",	"standard error",
};

static char *strName[3] = {
	"stdin", "stdout", "stderr",
};

static PRIMITIVE
fork_process( SCM command, SCM args, SCM redirection, int run_async );

PRIMITIVE
run_process( SCM command, SCM args, SCM redirection ) {
  return fork_process(command, args, redirection, 1);
}

PRIMITIVE
run_sync( SCM command, SCM args, SCM redirection ) {
  return fork_process(command, args, redirection, 0);
}
 
static PRIMITIVE
fork_process( SCM command, SCM args, SCM redirection, int run_async ) {
  SCM		pinfo, arg, pnames, ptypes;
  char	*argv[MAX_ARGS_NO], msg[256], *files[3];
  int		argc, pid, i;
  long	flag;
  int		pipes[3][2];
  int		redirectionType[3];
  struct	process_info	*info;
  void *old_chld_sig_action;
  int svMask, usermask; int ok;
  int svMask1, mypid;  
        usermask = (sigmask(SIGUSR1));

  /* Checking arguments and creating UNIX-style */
  /* arguments list */
  
  if( NSTRINGP( command ) )
    err("run-process: bad program name", command);
  i = find_slot();  
  if( i < 0)
    return ntruth;
  
  NEWCELL(pinfo, tc_process);
  proc_arr[i] = pinfo;

  info	= (struct process_info *) malloc( sizeof( struct process_info ) );
  PROCESS(pinfo) = info;
  /*
   *
   *	Initializing info structure
   *
   */
  
  info->commandLine		= strdup(CHARS( command ) );

  for( i = 0; i < 3; i++ ) {
    info->redirection[i]  = NO_REDIRECTION;
    info->stream[i]      = NIL;
  }
  
  argv[0]    = CHARS( command );
  
  for( argc = 1; argc < MAX_ARGS_NO && NNULLP( args ); ++argc ) {
    if( NCONSP( args ) )
      err("run-process: bad arguments list", args);

    arg    = CAR( args );
    args  = CDR( args );

    if( NSTRINGP( arg ) ) {
      /* In future, may be I implement conversion from */
      /* non-string argument to the string, but today */
      /* I don't want to do that :) */
      
      err("run-process: bad argument -- must be string", arg);
    }

    argv[argc]    = CHARS( arg );
  }

  if( argc == MAX_ARGS_NO )
    err("run-process: too many arguments (limit is 256)", args);

  argv[argc]    = NULL;


  /* Parsing redirection's list and creating communication */

  if( NNULLP( redirection ) ) {

    for( i = 0; i < 3; ++i ) {
      if( NCONSP( redirection ) )
        err("run-process: wrong redirection's list", redirection);
      
      if( STRINGP( CAR( redirection ) ) ) {

        info->redirection[i]  = REDIRECTION_BY_FILE;
        info->stream[i]      = string_copy( CAR( redirection ) );
        
        /* redirectionType[i]    = REDIRECTION_BY_FILE;
	   files[i]        = CHARS( CAR( redirection ) ); */
        
        pipes[i][0]        = open(CHARS( CAR( redirection ) ),
				  i == 0 ? O_RDONLY : O_WRONLY);
        if( pipes[i][0] < 0 ) {
          sprintf(msg, "run-process: can't redirect %s to file %s",
		  stdStreams[i], CHARS( CAR( redirection ) ));
          
          err( msg, NIL );
        }

        redirection  = CDR( redirection );
        continue;
      }

      if( BOOLEANP( CAR( redirection ) ) ) {

        if( CAR( redirection ) == truth ) {
          if( pipe( pipes[i] ) < 0 ) {
            
            sprintf(msg, "run-process: can't create stream for %s\n",
		    stdStreams[i]

		    );
            perror("Process");
            err( msg, NIL );
          }
          
          /* redirectionType[i]  = REDIRECTION_BY_STREAM; */
          
          info->redirection[i]  = REDIRECTION_BY_STREAM;
        }

        redirection    = CDR( redirection );
        continue;
      }

      err("run-process: bad redirection type", CAR( redirection ));
    }
  }

    
  /* set handler to catch SIGUSR1 */
  signal(SIGUSR1,su1_handler); 

  /* block user1 signal till parent will be ready */
  svMask1 = sigblock(usermask);
  mypid = getpid();

  /* Now, forking and catching the errors */
  pid    = fork();

  if( pid < 0 ) {
    char  msg[256];

    sprintf(msg,
	    "run-process: can't create child process because of (see stderr)"
	    );
    perror("CHILD process");
    err( msg, NIL );
  }

  /* Processing child's behavior */

  if( pid == 0 ) {
    if(run_async){
      svMask = sigblock(usermask);
      signal(SIGUSR1,su1_handler); 
      /* send notification to parent that I'm ready */
      ok = kill(mypid,SIGUSR1);
      if(ok < 0) perror( "Sending to parent");
      sigpause(0); 
      sigsetmask(svMask);
/*
 *      fprintf(stderr, "Mask: %x\n", usermask);
 *      fprintf(stderr, "Child continues...");
 *      perror("Child:");
 */

      setsid();
    }
      
    for( i = 0; i < 3; ++i ) {
      switch( info->redirection[i] ) {
  
      case REDIRECTION_BY_FILE:
	dup2( pipes[i][0], i );
	close( pipes[i][0] );
	break;
  
      case REDIRECTION_BY_STREAM:
	dup2( pipes[i][ i == 0 ? 0 : 1], i );
	close( pipes[i][0] );
	close( pipes[i][1] );
	break;
  
      default:
	break;
      }
    }
    
    for( i = 3; i < NOFILE; ++i )
      close( i );
    
    
    /*  And then, EXEC'ing...  */
    
    execvp( argv[0], argv );
    
    /* Unfortunatelly, we can't exec this process -- but */
    /* we can't tell 'bout this fact to our daddy. :( */
    
    fprintf(stderr, "Can't exec!");
    exit( 1 );
  }
  
  /*  Ok, guys, we are still in the parent process. Making redirection */
  /*  and filling-up PROCESS structure */
  PROCPID( pinfo )  = pid;
  if(!run_async) waitpid(pid);
  else {
    for( i = 0; i < 3; ++i ) {
      switch( info->redirection[i] ) {
      case REDIRECTION_BY_FILE:
	close( pipes[i][0] );
	break;
	
      case REDIRECTION_BY_STREAM:
	close( pipes[i][ i == 0 ? 0 : 1 ] );
	
	flag  = no_interrupt(1);
	
	NEWCELL( info->stream[i], i == 0 ? tc_oport : tc_iport );
	
	if( (info->stream[i]->storage_as.port.f =
	     fdopen( pipes[i][ i == 0 ? 1 : 0],
		    i == 0 ? "w" : "r" )) == NULL )
	  err("process-input: can't FDOPEN stream", pinfo);
	
	sprintf(msg, "*%s-%d*", strName[i], pid);
	
	info->stream[i]->storage_as.port.name  = must_malloc( strlen( msg ) + 1 );
	strcpy( info->stream[i]->storage_as.port.name, msg );  
	
	no_interrupt( flag );
	break;
	
      default:
	break;
      }
    }
    /** all house keeping is done... notyfy child to go ***/
#if 1
      sigpause(0); /* wait for child notification */
      sigsetmask(svMask1); 
    /* notify child */
    ok = kill(pid,SIGUSR1);
    if(ok < 0) perror("Parent sigusr");
/*    else fprintf(stderr, "Parent sending SIGUSR1 to %d\n",pid); */
#endif
  }
  PROCESS( pinfo )  = info;
  return  pinfo;
}


/*** INTERFACE ****/

PRIMITIVE
  processp( SCM process ) {
       return PROCESSP( process ) ? truth : ntruth;
     }


PRIMITIVE
process_alivep( SCM process ) {

  if( NPROCESSP( process ) )
    err("process-alive?: wrong argument type", process);

  return  kill( PROCPID( process ), 0 ) == 0 ? truth : ntruth;
}

PRIMITIVE
process_pid( SCM process ) {

  if( NPROCESSP( process ) )
    err("process-pid: wrong argument type", process);

  return  makeinteger( PROCPID( process ) );
}

static char  *rtFile    = "*File*";
static char *rtStream  = "*Stream*";
static char  *rtNone    = "*None*";

static PRIMITIVE
get_internal_redirection( SCM process, int i ) {
  SCM    rType, rName;
  struct process_info  *info;
  
  if( NPROCESSP( process ) )
    err("process-stream-type: wrong argument type", process);

  info  = PROCESS( process );
  
  switch( info->redirection[i] ) {

    case REDIRECTION_BY_FILE:
    rType  = makestrg( strlen( rtFile ), rtFile );
    rName  = string_copy( info->stream[i] );
    break;

    case NO_REDIRECTION:
    rType  = makestrg( strlen( rtNone ), rtNone );
    rName  = NIL;
    break;

    default: /* REDIRECTION_BY_STREAM */
    rType  = makestrg( strlen( rtStream ), rtStream );
    rName  = makestrg( strlen( stdStreams[i] ), stdStreams[i] );
    break;
  }

  return  cons( rType, rName );
}


/*** enumerate ***/
PRIMITIVE 
process_list(){
  int i;
  SCM lst = NIL;
  for(i = 0; i<MAX_PROC_NUM; i++)
    if(proc_arr[i] != ntruth)
      lst = cons(proc_arr[i],lst);
  return lst;
}
  

    
PRIMITIVE
process_input_info( SCM process ) {

  return  get_internal_redirection( process, 0 );
}

PRIMITIVE
process_output_info( SCM process ) {
  
  return  get_internal_redirection( process, 1 );
}

PRIMITIVE
process_error_info( SCM process ) {
  
  return  get_internal_redirection( process, 2 );
}

PRIMITIVE
process_command( SCM process ) {
  struct process_info *info;
  
  if( NPROCESSP( process ) )
    err("process-command: wrong argument type", process);

  info  = PROCESS( process );
  
  return  makestrg( strlen( info->commandLine ), info->commandLine );
}


/*
 * Creating and returning ports to opened streams
 */

PRIMITIVE
process_input( SCM process ) {
  struct process_info  *info;
  
  if( NPROCESSP( process ) )
    err("process-input: wrong argument type", process);
  
  info  = PROCESS( process );
  
  if( info->redirection[0] != REDIRECTION_BY_STREAM ) {
    return  NIL;
  }

  return  info->stream[0];
}

PRIMITIVE
process_output( SCM process ) {
  struct process_info  *info;
  
  if( NPROCESSP( process ) )
    err("process-input: wrong argument type", process);
  
  info  = PROCESS( process );
  
  if( info->redirection[1] != REDIRECTION_BY_STREAM ) {
    return  NIL;
  }

  return  info->stream[1];
}

PRIMITIVE
process_error( SCM process ) {
  struct process_info  *info;
  
  if( NPROCESSP( process ) )
    err("process-input: wrong argument type", process);
  
  info  = PROCESS( process );
  
  if( info->redirection[2] != REDIRECTION_BY_STREAM ) {
    return  NIL;
  }

  return  info->stream[2];
}


void 
mark_process( SCM process ){
  struct process_info *info;
  int i;
  info = PROCESS(process);
  for(i=0; i<3 ; i++)
   gc_mark(info->stream[i]);
}


void
free_process( SCM process ) {
  int    i;
  struct process_info *info;
  info  = PROCESS( process );
  i = remove_process(process);
  if(i < 0)
    err("cannot unregister process", process);
  if( info->commandLine )
    free( info->commandLine );
  
  for( i = 0; i < 3; ++i ) {
    if( info->redirection[i] == REDIRECTION_BY_STREAM && info->stream[i] != NIL ) {
      freeport( info->stream[i] );
    }
  }
  free(info); /* A.T. ++ */
}


PRIMITIVE
process_kill( SCM process ) {
  struct process_info *info;  
  int i;
  if( NPROCESSP( process ) )
    err("process-kill: wrong argument", process);
  
  info  = PROCESS( process );  
#if 1
  for( i = 0; i < 3; ++i ) {
    if( info->redirection[i] == REDIRECTION_BY_STREAM && 
       info->stream[i] != NIL ) {
      freeport( info->stream[i] );
      info->stream[i]=NIL;
    }
  }
#endif
  kill( PROCPID( process ), 15 );
  return  truth;
}


/******* run-time initialization ********/
void init_process(void)
{
  tc_process = add_new_type(&process_type);
  init_proc_table();

  add_new_primitive("run-process", tc_subr_3, run_process);    /* + */
  add_new_primitive("run-sync", tc_subr_3, run_sync);    /* + */
  add_new_primitive("process?", tc_subr_1, processp);    /* + */
  add_new_primitive("process-alive?", tc_subr_1,  process_alivep);  /* + */
  add_new_primitive("process-input-info", tc_subr_1, process_input_info);  /* + */
  add_new_primitive("process-output-info", tc_subr_1, process_output_info);  /* + */
  add_new_primitive("process-error-info", tc_subr_1, process_error_info);  /* + */
  add_new_primitive("process-command", tc_subr_1, process_command);    /* + */
  add_new_primitive("process-pid", tc_subr_1, process_pid);    /* + */
  add_new_primitive("process-input", tc_subr_1, process_input);    /* + */
  add_new_primitive("process-output", tc_subr_1, process_output);  /* + */
  add_new_primitive("process-error", tc_subr_1, process_error);    /* + */  
  add_new_primitive("process-kill", tc_subr_1,process_kill);    /* + */
  add_new_primitive("process-list", tc_subr_0,process_list);    /* + */

}
