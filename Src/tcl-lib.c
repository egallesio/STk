/*
 *
 * t c l - l i b . c 		- A library remplacement for simulating 
 *				  a Tcl interpreter in Scheme
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 19-Feb-1993 22:15
 * Last file update: 29-Aug-1996 12:49
 *
 */


#include "stk.h"

#ifdef USE_TK
#  include "tk-glue.h"
#else
#  include "tclInt.h"
#endif


#ifdef USE_TK
#define BUFF_SIZE 100
/******************************************************************************
 *
 *				Eval functions
 *
 ******************************************************************************/

int Tcl_GlobalEval(interp, s)
     Tcl_Interp *interp;
     char *s;
{
  char buffer[BUFF_SIZE+3], *ptr = buffer;
  SCM result;

  /* 
   * If the callback is nor surrounded by parenthesis, add them. We
   * don't have parenthesis when the callback is a closure. In this
   * case, the callback is simply #p12345. Note that this allow Tk to
   * add some parameters to the callback when needed (on bindings, or
   * scrollbars for instance). To recognize such cases, we look at
   * first character: if it is not an open parenthesis, we add a pair
   * of () around the callback string
   *     
   */
  if (*s == '\0') return TCL_OK;
  if (*s != '(') {
    /* Build the command to evaluate by adding a pair of parenthesis */
    int len = strlen(s);
    
    if (len > BUFF_SIZE) 
      ptr = (char *) must_malloc(len+3);
    sprintf(ptr, "(%s)", s);
    s = ptr;
  }
  result = STk_internal_eval_string(s, ERR_TCL_BACKGROUND, NIL);
  Tcl_ResetResult(interp);
    
  if (ptr != buffer) free(ptr);

  if (result != EVAL_ERROR) {
    SCM dumb;

    Tcl_SetResult(interp, 
		  STk_stringify(STk_convert_for_Tk(result, &dumb), 0), 
		  TCL_DYNAMIC);
    /* 
     * Store also the "true" result in STk_last_Tk_result
     * Warning: This pointer inot GC protected. We have to use it very soon 
     * This is a kludge (used in text window)
     */
     STk_last_Tk_result = result;
       
     return (result == Sym_break) ? TCL_BREAK : TCL_OK;
  }
  
  return TCL_ERROR;
}

int Tcl_Eval(interp, s) 	/* very simplist. */
     Tcl_Interp *interp;	/* But do we need something more clever? */
     char *s;
{
  return Tcl_GlobalEval(interp, s);
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_VarEval --
 *
 *	Given a variable number of string arguments, concatenate them
 *	all together and execute the result as a Tcl command.
 *
 * Results:
 *	A standard Tcl return result.  An error message or other
 *	result may be left in interp->result.
 *
 * Side effects:
 *	Depends on what was done by the command.
 *
 *----------------------------------------------------------------------
 */
	/* VARARGS2 */ /* ARGSUSED */
int
Tcl_VarEval TCL_VARARGS_DEF(Tcl_Interp *,arg1)
{
    va_list argList;
    Tcl_DString buf;
    char *string;
    Tcl_Interp *interp;
    int result;

    /*
     * Copy the strings one after the other into a single larger
     * string.  Use stack-allocated space for small commands, but if
     * the command gets too large than call ckalloc to create the
     * space.
     */

    interp = TCL_VARARGS_START(Tcl_Interp *,arg1,argList);
    Tcl_DStringInit(&buf);
    while (1) {
	string = va_arg(argList, char *);
	if (string == NULL) {
	    break;
	}
	Tcl_DStringAppend(&buf, string, -1);
    }
    va_end(argList);

    result = Tcl_Eval(interp, Tcl_DStringValue(&buf));
    Tcl_DStringFree(&buf);
    return result;
}

/******************************************************************************
 *
 *	      Variable accesses (GetVar, GetVar2, SetVar, SetVar2)
 *
 ******************************************************************************/

char *Tcl_GetVar(interp, var, flags)
     Tcl_Interp *interp;	/* not used */
     char *var;
     int flags;
{
  SCM dumb, V = VCELL(Intern(var));
  return (V == UNBOUND) ? NULL : STk_convert_for_Tk(V, &dumb);
}

char *Tcl_GetVar2(interp, name1, name2, flags)
     Tcl_Interp *interp;	/* not used */
     char *name1, *name2;
     int flags;
{
  if (name2 && *name2) {
    char *res, *s = must_malloc(strlen(name1) + strlen(name2) + 3);

    sprintf(s, "%s{%s}", name1, name2);
    res = Tcl_GetVar(interp, s, flags);
    free(s);
    return res;
  }
  return Tcl_GetVar(interp, name1, flags);
}

char *Tcl_SetVar(interp, var, val, flags)
     Tcl_Interp *interp;
     char *var, *val;
     int flags;
{
  register SCM tmp, value;

  tmp = Intern(var);
  if (flags & STk_STRINGIFY) {
    /* Val is already a string, since it comes from Tk */
    value = STk_makestring(val);
  }
  else {
    if (*val) {
      SCM port;
      int eof;
      
      port  = STk_internal_open_input_string(val);
      value = STk_internal_read_from_string(port, &eof, TRUE);
      if (value == EVAL_ERROR) return NULL;
    }
    else 
      value =  STk_makestring("");
  }

  VCELL(tmp) = value;
  if (TRACED_VARP(tmp)) STk_change_value(tmp, NIL);

  return val;
}

char *Tcl_SetVar2(interp, name1, name2, val, flags)
     Tcl_Interp *interp;
     char *name1, *name2, *val;
     int flags;
{ 
  if (name2 && *name2) {
    char *res, *s = must_malloc(strlen(name1) + strlen(name2) + 3);

    sprintf(s, "%s{%s}", name1, name2);
    res = Tcl_SetVar(interp, s, val, flags);
    free(s);
    return res;
  }
  return Tcl_SetVar(interp, name1, val, flags);
}

/******************************************************************************
 *
 *			    Tcl command management
 *
 ******************************************************************************/

int Tcl_internal_DeleteCommand(interp, cmdName)
  Tcl_Interp *interp;
  char *cmdName;
{
  struct Tk_command *W;
  Interp *iPtr = (Interp *) interp;
  Tcl_HashEntry *hPtr;
  
  /* the internal DeleteCommand do all the job (except setting cmdName to
   * UNBOUND, because this implies to use intern which can allocate a cell)
   * Consequently, GC calls this function, whereas Tk call the true 
   * DeleteCommand
   */
  hPtr = Tcl_FindHashEntry(&iPtr->commandTable, cmdName);
  if (hPtr == NULL) return -1;

  W = (struct Tk_command *) Tcl_GetHashValue(hPtr);
  if (W->deleted) return -1;

  /* Execute the delete handler */
  if (W->delproc != NULL) (*W->delproc)(W->deldata);

  /* Note: W will be freed by the GC */
  Tcl_DeleteHashEntry(hPtr);

  /* Delete the command from the callbacks hash table */
  STk_delete_callback(cmdName);

  /* Remember that the command has been deleted */
  W->deleted = 1;

  return 0;
}
  

int Tcl_DeleteCommand(interp, cmdName)
     Tcl_Interp *interp;
     char *cmdName;
{
  int result;

  if (result = Tcl_internal_DeleteCommand(interp, cmdName))
    return result;
  
  /* Undefine "cmdName" by doing a (set! cmdname #<unbound>) */
  VCELL(Intern(cmdName)) = UNBOUND;

  return 0;
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateCommand --
 *
 *	Define a new command in a command table.
 *
 * Results:
 *	The return value is a token for the command, which can
 *	be used in future calls to Tcl_NameOfCommand.
 *
 * Side effects:
 *	If a command named cmdName already exists for interp, it is
 *	deleted.
 *----------------------------------------------------------------------
 */
Tcl_Command
Tcl_CreateCommand(interp, cmdName, proc, clientData, deleteProc)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmdName;		/* Name of command. */
    Tcl_CmdProc *proc;		/* Command procedure to associate with
				 * cmdName. */
    ClientData clientData;	/* Arbitrary one-word value to pass to proc. */
    Tcl_CmdDeleteProc *deleteProc;
				/* If not NULL, gives a procedure to call when
				 * this command is deleted. */
{
  struct Tk_command * W;
  Interp *iPtr = (Interp *) interp;
  register struct Tk_command *cmdPtr;
  Tcl_HashEntry *hPtr;
  int new;
  SCM z;

  /* 
   * There are two things to do here.  
   * 	- We must create a new variable and associate the newly
   * 	  created Tk object to it. This is the Scheme part.
   *    - We must  store the cmd in a hashtable contained in the interp data
   *      structure. This is necessary to delete all the command associated
   *      to the interpreter. This is important at least for the "send"
   *      command which un-register the interpreter from the X server.
   * 
   */

  /* Initialize the Tk_command structure */
  W = (struct Tk_command *)must_malloc(sizeof(struct Tk_command)+strlen(cmdName));
  W->ptr	= clientData;
  W->fct 	= proc;
  W->delproc    = deleteProc;
  W->deldata	= clientData;
  W->deleted	= 0;
  strcpy(W->Id, cmdName);

  /* Register the command in the Tcl command hash table */
  hPtr = Tcl_CreateHashEntry(&iPtr->commandTable, cmdName, &new);
  if (!new) {
    /* Command already exists: delete the old one */
    Tcl_DeleteCommand(interp, cmdName); /* not efficient but safer */
  }
  Tcl_SetHashValue(hPtr, W);

  /* Define a Tk-command cell for this new command */
  NEWCELL(z, tc_tkcommand);
  z->storage_as.tk.data   = W;
  z->storage_as.tk.l_data = Ntruth;
    
  /* Define a variable whose name is the command name */
  VCELL(Intern(cmdName)) = z;

  return (Tcl_Command) W;
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetCommandName --
 *
 *	Given a token returned by Tcl_CreateCommand, this procedure
 *	returns the current name of the command (which may have changed
 *	due to renaming).
 *
 * Results:
 *	The return value is the name of the given command.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetCommandName(interp, command)
    Tcl_Interp *interp;		/* Interpreter containing the command. */
    Tcl_Command command;	/* Token for the command, returned by a
				 * previous call to Tcl_CreateCommand.
				 * The command must not have been deleted. */
{
  return ((struct Tk_command *) command)->Id;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetCommandInfo --
 *
 *	Returns various information about a Tcl command.
 *
 * Results:
 *	If cmdName exists in interp, then *infoPtr is modified to
 *	hold information about cmdName and 1 is returned.  If the
 *	command doesn't exist then 0 is returned and *infoPtr isn't
 *	modified.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetCommandInfo(interp, cmdName, infoPtr)
    Tcl_Interp *interp;			/* Interpreter in which to look
					 * for command. */
    char *cmdName;			/* Name of desired command. */
    Tcl_CmdInfo *infoPtr;		/* Where to store information about
					 * command. */
{
    SCM v = Intern(cmdName);
    struct Tk_command *p;
    
    if (NTKCOMMP(VCELL(v))) return 0;

    p =  VCELL(v)->storage_as.tk.data;

    infoPtr->proc       = p->fct;
    infoPtr->clientData = p->ptr;
    infoPtr->deleteProc = p->delproc;;
    infoPtr->deleteData = NULL;
    return 1;
}


/******************************************************************************
 *
 *			  Tcl interpreter management
 *
 ******************************************************************************/

Tcl_Interp *Tcl_CreateInterp()
{
  register Interp *iPtr = (Interp *) ckalloc(sizeof(Interp));
  
  iPtr->result		 = iPtr->resultSpace;
  iPtr->freeProc	 = 0;
  iPtr->errorLine	 = 0;
  iPtr->resultSpace[0]   = 0;
  
  iPtr->appendResult	 = NULL;
  iPtr->appendAvl	 = 0;
  iPtr->appendUsed	 = 0;

  strcpy(iPtr->pdFormat, "%g");

  iPtr->assocData 	 = (Tcl_HashTable *) NULL;

  /* See Tcl_CreateCommand for this table utility  */
  Tcl_InitHashTable(&iPtr->commandTable, TCL_STRING_KEYS);

  return (Tcl_Interp *) iPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetAssocData --
 *
 *	Creates a named association between user-specified data, a delete
 *	function and this interpreter. If the association already exists
 *	the data is overwritten with the new data. The delete function will
 *	be invoked when the interpreter is deleted.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Sets the associated data, creates the association if needed.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetAssocData(interp, name, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter to associate with. */
    char *name;			/* Name for association. */
    Tcl_InterpDeleteProc *proc;	/* Proc to call when interpreter is
                                 * about to be deleted. */
    ClientData clientData;	/* One-word value to pass to proc. */
{
    Interp *iPtr = (Interp *) interp;
    AssocData *dPtr;
    Tcl_HashEntry *hPtr;
    int new;

    if (iPtr->assocData == (Tcl_HashTable *) NULL) {
	if (iPtr->flags & DELETED) {
	    /*
	     * Don't create new entries after interpreter deletion
	     * has started;  it isn't even safe to muck with the
	     * interpreter right now
	     */

	    return;
	}
        iPtr->assocData = (Tcl_HashTable *) ckalloc(sizeof(Tcl_HashTable));
        Tcl_InitHashTable(iPtr->assocData, TCL_STRING_KEYS);
    }
    hPtr = Tcl_CreateHashEntry(iPtr->assocData, name, &new);
    if (new == 0) {
        dPtr = (AssocData *) Tcl_GetHashValue(hPtr);
    } else {
        dPtr = (AssocData *) ckalloc(sizeof(AssocData));
    }
    dPtr->proc = proc;
    dPtr->clientData = clientData;

    Tcl_SetHashValue(hPtr, dPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteAssocData --
 *
 *	Deletes a named association of user-specified data with
 *	the specified interpreter.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Deletes the association.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DeleteAssocData(interp, name)
    Tcl_Interp *interp;			/* Interpreter to associate with. */
    char *name;				/* Name of association. */
{
    Interp *iPtr = (Interp *) interp;
    AssocData *dPtr;
    Tcl_HashEntry *hPtr;

    if (iPtr->assocData == (Tcl_HashTable *) NULL) {
        return;
    }
    hPtr = Tcl_FindHashEntry(iPtr->assocData, name);
    if (hPtr == (Tcl_HashEntry *) NULL) {
        return;
    }
    dPtr = (AssocData *) Tcl_GetHashValue(hPtr);
    ckfree((char *) dPtr);
    Tcl_DeleteHashEntry(hPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetAssocData --
 *
 *	Returns the client data associated with this name in the
 *	specified interpreter.
 *
 * Results:
 *	The client data in the AssocData record denoted by the named
 *	association, or NULL.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ClientData
Tcl_GetAssocData(interp, name, procPtr)
    Tcl_Interp *interp;			/* Interpreter associated with. */
    char *name;				/* Name of association. */
    Tcl_InterpDeleteProc **procPtr;	/* Pointer to place to store address
					 * of current deletion callback. */
{
    Interp *iPtr = (Interp *) interp;
    AssocData *dPtr;
    Tcl_HashEntry *hPtr;

    if (iPtr->assocData == (Tcl_HashTable *) NULL) {
        return (ClientData) NULL;
    }
    hPtr = Tcl_FindHashEntry(iPtr->assocData, name);
    if (hPtr == (Tcl_HashEntry *) NULL) {
        return (ClientData) NULL;
    }
    dPtr = (AssocData *) Tcl_GetHashValue(hPtr);
    if (procPtr != (Tcl_InterpDeleteProc **) NULL) {
        *procPtr = dPtr->proc;
    }
    return dPtr->clientData;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_InterpDeleted --
 *
 *	Returns nonzero if the interpreter has been deleted with a call
 *	to Tcl_DeleteInterp.
 *
 * Results:
 *	Nonzero if the interpreter is deleted, zero otherwise.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_InterpDeleted(interp)
    Tcl_Interp *interp;
{
    return (((Interp *) interp)->flags & DELETED) ? 1 : 0;
}
/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteInterp --
 *
 *	Delete an interpreter and free up all of the resources associated
 *	with it.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The interpreter is destroyed.  The caller should never again
 *	use the interp token.
 *
 *----------------------------------------------------------------------
 */
void Tcl_DeleteInterp(interp)
    Tcl_Interp *interp;
{
  Interp *iPtr = (Interp *) interp;
  Tcl_HashEntry *hPtr;
  Tcl_HashSearch search;
  struct Tk_command *W;

  if (!iPtr || (iPtr->flags & DELETED)) return;

  /* Mark the interpreter as deleted. No further evals will be allowed. */
  iPtr->flags |= DELETED;
  
  /* Delete result space */
  if (iPtr->appendResult != NULL) {
    ckfree(iPtr->appendResult);
  }
  
  /* delete hash table of Tk commands (see Tcl_CreateCommand) */
  for (hPtr = Tcl_FirstHashEntry(&iPtr->commandTable, &search);
       hPtr != NULL; 
       hPtr = Tcl_NextHashEntry(&search)) {
    W = (struct Tk_command *) Tcl_GetHashValue(hPtr);
    Tcl_DeleteCommand(interp, W->Id);
  }
  Tcl_DeleteHashTable(&iPtr->commandTable);

  ckfree((char *) iPtr);
}



/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetOpenFile --
 *
 *	Given a name of a channel registered in the given interpreter,
 *	returns a FILE * for it.
 *
 * Results:
 *	A standard Tcl result. If the channel is registered in the given
 *	interpreter and it is managed by the "file" channel driver, and
 *	it is open for the requested mode, then the output parameter
 *	filePtr is set to a FILE * for the underlying file. On error, the
 *	filePtr is not set, TCL_ERROR is returned and an error message is
 *	left in interp->result.
 *
 * Side effects:
 *	May invoke fdopen to create the FILE * for the requested file.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetOpenFile(interp, string, forWriting, checkUsage, filePtr)
    Tcl_Interp *interp;		/* Interpreter in which to find file. */
    char *string;		/* String that identifies file. */
    int forWriting;		/* 1 means the file is going to be used
				 * for writing, 0 means for reading. */
    int checkUsage;		/* 1 means verify that the file was opened
				 * in a mode that allows the access specified
				 * by "forWriting". Ignored, we always
                                 * check that the channel is open for the
                                 * requested mode. */
    ClientData *filePtr;	/* Store pointer to FILE structure here. */
{
  SCM port;

  if ((string[0] == '#') && (string[1] == 'f') && (string[2] == 'i')
      && (string[3] == 'l') && string[4] == 'e') {
    char *end;
    
    port = (SCM) strtoul(string+5, &end, 16);
    if ((end != string+5) && (*end == 0)) {
      /* Verify the given address is a port */
      if (STk_valid_address(port) && (IPORTP(port) || OPORTP(port))) {
	/* Verify the port usage  */
	if (checkUsage && (PORT_FLAGS(port) & PORT_CLOSED)) {
	  Tcl_AppendResult(interp, "\"", string,
			   "\" is closed", (char *) NULL);
	  return TCL_ERROR;
	}
	if (forWriting) {
	  if (checkUsage && !OPORTP(port)) {
	    Tcl_AppendResult(interp, "\"", string,
		    "\" wasn't opened for writing", (char *) NULL);
	    return TCL_ERROR;
	  }
	}
	else {
	   if (!IPORTP(port)) {
	     Tcl_AppendResult(interp, "\"", string,
			      "\" wasn't opened for reading", (char *) NULL);
	     return TCL_ERROR;
	   }
	}
	
	/* File is correct; return it in filePtr */
	*filePtr = PORT_FILE(port);
	return TCL_OK;
      }
    }
  }
  Tcl_AppendResult(interp, "Bad file specification \"", string,
		    "\"", (char *) NULL);
  return TCL_ERROR; 
}
#endif

/******************************************************************************
 *
 * Tcl channels simulation.
 *
 *	Current version is very minimal. It should probably be extended to be 
 *	more Tcl compatible, since Tcl model is very neat.
 *
 ******************************************************************************/

Tcl_Channel Tcl_OpenFileChannel(Tcl_Interp *interp, char *fileName, 
				char *modeString, int permissions)
{
  return (Tcl_Channel) fopen(fileName, modeString);
}

int Tcl_Close(Tcl_Interp *interp, Tcl_Channel chan)
{
  return fclose((FILE *) chan);
}

int Tcl_Read(Tcl_Channel chan, char *bufPtr, int toRead)
{
  return read(fileno((FILE *)chan), bufPtr, toRead);
}

int Tcl_Write(Tcl_Channel chan, char *s, int slen)
{
  return write(fileno((FILE *)chan), s, slen);
}


int Tcl_Seek(Tcl_Channel chan, int offset, int mode)
{
  return fseek((FILE*) chan, (long) offset, mode);
}

int Tcl_Flush(Tcl_Channel chan)
{
  return fflush((FILE *) chan);
}

Tcl_Channel Tcl_GetStdChannel(int type) /*  TCL_STDIN, TCL_STDOUT, TCL_STDERR. */
{
  switch (type) {
    case TCL_STDIN:  return (Tcl_Channel) STk_stdin;
    case TCL_STDOUT: return (Tcl_Channel) STk_stdout;
    case TCL_STDERR: return (Tcl_Channel) STk_stderr;
  }
  return NULL;
}
