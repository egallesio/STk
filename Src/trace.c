/*
 *
 * t r a c e . c 				-- Variable Tracing
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
 *
 * Variable tracing is important in Tk since some widgets intensively use this
 * mechanism. For instance, a check-button has a variable associated to it.
 * When the button is clicked, the variable is set and when the variable is 
 * modified, button state is consequntly changed (last case is done with a trace
 * over the associated variable). Tcl trace mechanism is more general than this;
 * the mechanism implemented here is just intended to mimic the trace over 
 * variable writing (reading a var is not used by Tk and procedure tracing is a
 * common thing easy to do in the Lisp world).
 * Note: a single variable can be associated to several C functions (For instance
 * when a radio-button associated variable is changed, a C function is used to 
 * clear the selector and another to hilight the new selector). So, traces are 
 * stored in a linked list (all traces are called on variable changement).
 *
 * Note:
 *    - Implementation use Tcl hash tables to see if a variable is traced.
 *
 *
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 24-Feb-1993 13:07
 * Last file update:  3-Sep-1999 21:02 (eg)
 *
 */

#include "stk.h"


#define TRACING 	(1<<20)
#define STk_TRACE 	TCL_TRACE_READS /* We use TCL_TRACE_READS, which is 	*/
					/* not used by Tk, for tagging		*/
					/* traces set with the trace-var 	*/
					/* procedure 				*/

typedef struct STk_VarTrace {
  Tcl_VarTraceProc *traceProc; /* Procedure to call when operations given
				* by flags are performed on variable. */
  ClientData clientData;       /* Argument to pass to proc. */
  int flags;		       /* What events the trace procedure is
				* interested in:  OR-ed combination of
				* TCL_TRACE_READS, TCL_TRACE_WRITES, and
				* TCL_TRACE_UNSETS. */
  struct STk_VarTrace *nextPtr;/* Next in list of traces associated with
				* a particular variable. */
  SCM env;		       /* Environment of this variable */
} STk_var_trace;

static Tcl_HashTable VarTable;	/* Global hash table retaining traced variables */

/******************************************************************************
 *
 * Scheme part
 *
 ******************************************************************************/
/********** L O W   L E V E L **********/

static int TraceVar(SCM var, int flags, Tcl_VarTraceProc *proc, 
		    ClientData clientData, SCM env)
{
  int 		new;
  Tcl_HashEntry	*entry;
  STk_var_trace	*data;
  
  entry = Tcl_CreateHashEntry(&VarTable, PNAME(var), &new);
  /* Create the value associated to the "var" key */
  data= (STk_var_trace *) ckalloc((unsigned) sizeof (STk_var_trace));
  data->flags	   = flags  & ~TCL_TRACE_UNSETS; /* Unset has no meaning in stk */;
  data->traceProc  = proc;
  data->clientData = clientData;
  data->env	   = env;
  data->nextPtr	   = (STk_var_trace *) (new ? NULL : Tcl_GetHashValue(entry));

  /* Put it in table */
  Tcl_SetHashValue(entry, (ClientData) data);

  /* Retain that it exist a traced variable for this symbol */
  var->cell_info |= CELL_INFO_TRACED_VAR;

  return TCL_OK;
}
 

static void UntraceVar(SCM var, int flags, Tcl_VarTraceProc *proc,
		       ClientData clientData)
{
  Tcl_HashEntry *entry;
  register STk_var_trace *p, *prev;
  
  if ((entry = Tcl_FindHashEntry(&VarTable, PNAME(var)))) {
    /* Variable is traced. Try to find correponding trace function */
    flags &= ~TCL_TRACE_UNSETS; /* Unset has no meaning for us */

    p = (STk_var_trace *) Tcl_GetHashValue(entry);    
    for (prev=NULL; p ; prev=p, p=p->nextPtr) {
      if (p->traceProc == proc && p->flags == flags && p->clientData == clientData)
	break;
    }
    if (p) {
      if (prev == NULL) {
	if (p->nextPtr)
	  Tcl_SetHashValue(entry, (ClientData *) p->nextPtr);
	else 
	  Tcl_DeleteHashEntry(entry);
      }
      else
	prev->nextPtr = p->nextPtr;
      ckfree(p);
    }
    var->cell_info &= (short) ~CELL_INFO_TRACED_VAR;
  }
}


static char *TraceVarFct(ClientData clientData, Tcl_Interp *interp, 
			 char *name1, char *name2, int flags)
{
  /* 
   * ClientData is the only field which of interest here. It contains the
   * thunk to call
   */
  Apply0((SCM) clientData);

  return NULL; /* to make the compiler happy */
}


/*
 * STk_complete_untrace
 *
 * Delete all the traces associated to a variable (used by ``untrace-var'')
 *
 */

static void complete_untrace(char *var)
{
  Tcl_HashEntry *entry;
  register STk_var_trace *p, *q;
  
  if ((entry = Tcl_FindHashEntry(&VarTable, var))) {
    /* Variable is traced. Try to find correponding trace function */
    for (p = (struct STk_VarTrace *) Tcl_GetHashValue(entry); p; p=q) {
      q = p=p->nextPtr;
      ckfree(p);
    }
    Tcl_DeleteHashEntry(entry);
  }
}

/*
 * STk_change_value
 *
 * This function is called by Scheme when a there's a change on a traced global
 * variable (using a set! or a define). 
 *
 */

void STk_change_value(SCM var, SCM env)
{ 
  Tcl_HashEntry *entry;
  register STk_var_trace *data, *p;

  if ((entry = Tcl_FindHashEntry(&VarTable, PNAME(var)))) {
    /* Variable is traced. Call all the associated traces */
    data = (STk_var_trace *) Tcl_GetHashValue(entry);
    
    for (p = data; p ; p = p->nextPtr) {
      /* Invoke trace procedure if not already active */
      if (p->flags & TRACING) 
	continue;

      p->flags |= TRACING;
#ifdef USE_TK
      (*p->traceProc)(p->clientData, STk_main_interp, PNAME(var), "", p->flags);
#else
      (*p->traceProc)(p->clientData, NULL, PNAME(var), "", p->flags);
#endif
      /* Unset our flag */
      p->flags &= ~TRACING;
    }
  }
}

void STk_mark_tracevar_table(void)
{
  Tcl_HashEntry     *ent;
  Tcl_HashSearch    tmp;
  register STk_var_trace *p;

  for (ent=Tcl_FirstHashEntry(&VarTable, &tmp); ent;  ent=Tcl_NextHashEntry(&tmp)) {
    for (p = (STk_var_trace *) Tcl_GetHashValue(ent); p; p=p->nextPtr) {
      if (p->flags & STk_TRACE) 
	/* This is a trace done in Scheme (with trace-var). Consequently,
	 * clientData is a closure that we must mark */
	STk_gc_mark((SCM)(p->clientData));
    }
  }
}


/********** U S E R   I N T E R F A C E **********/

PRIMITIVE STk_trace_var(SCM var, SCM code)
{
  if (NSYMBOLP(var)) 	   Err("trace-var: bad variable name", var);
  if (!STk_is_thunk(code)) Err("trace-var: bad thunk", var);
  
  /* Add the trace */
  TraceVar(var, TCL_TRACE_WRITES|STk_TRACE, TraceVarFct, (ClientData) code, NIL);
  return UNDEFINED;
}

PRIMITIVE STk_untrace_var(SCM var)
{
  if (NSYMBOLP(var)) Err("untrace-var: bad variable name", var);
  complete_untrace(PNAME(var));

  return UNDEFINED;
}



#ifdef USE_TK
/*
 *
 * Tcl Part
 *
 */

int Tcl_TraceVar(interp, var, flags, proc, clientData)
     Tcl_Interp *interp; char *var; int flags;
     Tcl_VarTraceProc *proc; ClientData clientData;
{
  return TraceVar(Intern(var), flags, proc, clientData, NIL);
}

int Tcl_TraceVar2(interp, name1, name2, flags, proc, clientData)
     Tcl_Interp *interp; char *name1, *name2; int flags; 
     Tcl_VarTraceProc *proc; ClientData clientData;
{
  return TraceVar(Intern(name1), flags, proc, clientData, NIL);
}

void Tcl_UntraceVar(interp, var, flags, proc, clientData)
     Tcl_Interp *interp; char *var; int flags;
     Tcl_VarTraceProc *proc; ClientData clientData;
{
  UntraceVar(Intern(var), flags, proc, clientData);
}

void Tcl_UntraceVar2(interp, name1, name2, flags, proc, clientData)
     Tcl_Interp *interp; char *name1, *name2; int flags; 
     Tcl_VarTraceProc *proc; ClientData clientData;
{
  UntraceVar(Intern(name1), flags, proc, clientData);
}
#endif

void STk_init_tracevar(void)
{
  Tcl_InitHashTable(&VarTable, TCL_ONE_WORD_KEYS);
}
