/*
 *
 * t k - g l u e . c 		- Glue function between the scheme and Tk worlds
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 19-Feb-1993 22:15
 * Last file update:  4-Sep-1999 14:26 (eg)
 *
 */

#ifdef USE_TK
#include "stk.h"
#include "tk-glue.h"
#include "gc.h"
#include "tkInt.h"

#define MAXARG 64	/* Max args on stack. Use malloc if greater */

/* Scheme objects used to represent the "." pseudo widget and its name */
SCM STk_root_window; ;	/* kept in a global to avoid application 
			 * termination if redfined as UNBOUND */

/* Last result of Tcl_GlobalEval (as a SCM object rather than a string) */
SCM STk_last_Tk_result;


SCM STk_convert_tcl_list_to_scheme(SCM l)
{
  if (!l) return NIL;
  if (NCONSP(l)) return TCLOBJP(l) ? TCLOBJDATA(l) : l;
  return Cons(STk_convert_tcl_list_to_scheme(CAR(l)),
	      STk_convert_tcl_list_to_scheme(CDR(l)));
}

static SCM TkResult2Scheme(Tcl_Interp *interp, int objproc)
{
  SCM res;

  if (objproc) {
    register SCM data = TCLOBJDATA((SCM) Tcl_GetObjResult(interp));
    res = data ? STk_convert_tcl_list_to_scheme(data) : NIL;
  }
  else {
    register char *s = interp->result;
    res = (*s) ? STk_convert_Tcl_string2list(s) : NIL;
  }
  Tcl_ResetResult(interp);

  return res;
}

SCM STk_execute_Tcl_lib_cmd(SCM cmd, SCM args, SCM env, int eval_args)
{
  char *buffer[MAXARG+2];
  int tkres;
  char **argv 	       = buffer;
  int argc  	       = STk_llength(args);
  SCM conv_res, start  = args;
  struct Tk_command *W = cmd->storage_as.tk.data;
  int objproc;

  objproc = W->objproc;

  if (argc >= MAXARG) {
    /* allocate dynamically the argv array (one extra for argv[0] and one 
     * for the NULL terminator -dsf
     */
    argv=(char **) must_malloc((argc+2) * sizeof(char *));
  }

  /* First initialize an argv array */
  if (objproc) {
    /* Objproc procedure (Tcl_8.0+) */
    argv[0] = (char*) Tcl_NewStringObj(cmd->storage_as.tk.data->Id, -1);
    for (argc = 1; NNULLP(args); argc++, args=CDR(args)) {
      if (NCONSP(args)) Err("Malformed list of arguments", start);
      argv[argc] = (char*) STk_create_tcl_object(eval_args ?
						 STk_eval(CAR(args), env):
						 CAR(args));
    }
  }
  else {
    /* Classical Tcl procedure */
    /* 
     * conv_res is (roughly) a vector of the values returned by convert_for_Tcl.
     * It serves only to have pointers in the stack on the converted values. 
     * This permits to avoid GC problems (i.e. a GC between 1 and argc 
     * whereas convert_for_Tcl has created new cells in a previous iteration 
     */
    conv_res = STk_makevect(argc+2, NIL);

    argv[0] = cmd->storage_as.tk.data->Id;
  
    for (argc = 1; NNULLP(args); argc++, args=CDR(args)) {
      if (NCONSP(args)) Err("Malformed list of arguments", start);
      argv[argc] = STk_convert_for_Tcl(eval_args?
				           STk_eval(CAR(args),env):
				           CAR(args), 
				       &(VECT(conv_res)[argc]));
    }
  }
  argv[argc] = NULL;

  /* Now, call the Tk library function */
  Tcl_ResetResult(STk_main_interp);
  
  tkres = (*W->fct)(W->ptr, STk_main_interp, argc, argv);

  if (argv != buffer) {
    /* argv was allocated dynamically. Dispose it */
    free(argv);
  }

  /* return result as a string or "evaluated" depending of string_result field */
  if (tkres == TCL_OK)
    return TkResult2Scheme(STk_main_interp, objproc);
    
  /* if we are here, we had an error. Signal it */
  if (objproc) {
    Tcl_Obj *mess = Tcl_GetObjResult(STk_main_interp);
    SCM msg;

    msg = TCLOBJDATA((SCM) mess);

    if (!msg) msg = NIL;

    if (NSTRINGP(msg))
      Err("an error occured in a Tk procedure", msg);
    else 
      Err(CHARS(msg), NIL);
  }
  else
    Err(STk_main_interp->result, NIL);
  return UNDEFINED; /* never reached */
}

/******************************************************************************
 *
 * Callback management
 *
 ******************************************************************************/

static Tcl_HashTable Tk_callbacks;


int STk_valid_callback(char *s, void **closure)
{
  /* A callback is valid iff it is of the form "#pxxxx" where xxxx is composed
   * only of hexadecimal digits.
   * Furthermore, the given address must  be a valid adress
   */
  int l = strlen(s);
  char *p;

  *closure = NULL;
  if (l > 2) {
    if (s[0] == '#' && s[1] == 'p') {
      /* Verify that the rest of the string only contains hexadecimal digits */
      for (p = s + 2; *p; p++)
	if (!isxdigit(*p)) return FALSE;

      sscanf(s+2, "%lx", (unsigned long *) closure);
      if (!STk_valid_address((SCM) *closure)) return FALSE;
    }
  }
  return TRUE;
}

void STk_add_callback(char *key1, char *key2, char *key3, SCM closure)
{
  Tcl_HashEntry *entry;
  Tcl_HashTable *secondary_hash_table;
  int new_entry;
  char key[200]; /* Largely sufficient */

  if (*key2) {
    /* We have two keys. Use a secondary hash table */
    if ((entry=Tcl_FindHashEntry(&Tk_callbacks, key1)))
      /* Key already in hash table */
      secondary_hash_table = (Tcl_HashTable * ) Tcl_GetHashValue(entry);
    else {
      secondary_hash_table = (Tcl_HashTable *) must_malloc(sizeof(Tcl_HashTable));
      Tcl_InitHashTable(secondary_hash_table, TCL_STRING_KEYS);
      entry = Tcl_CreateHashEntry(&Tk_callbacks, (char *) key1, &new_entry);
      Tcl_SetHashValue(entry, secondary_hash_table);
    }
    
    /* Enter a new key (obtained from key2 and key3) in the hash table.
     * Don't worry about old value: since it is no more pointed by the 
     * hash table, it will be garbaged at next GC run
     */
    sprintf(key, "%s#%s", key2, key3);/* Create a new key from key2 and key3 */
    entry = Tcl_CreateHashEntry(secondary_hash_table, key, &new_entry);
    Tcl_SetHashValue(entry, closure); 
  }
  else {
    /* Only one key. No need for a secondary hash table */
    entry =Tcl_CreateHashEntry(&Tk_callbacks, key1, &new_entry);
    Tcl_SetHashValue(entry, closure);
  }
}


void STk_delete_callback(char *key)
{
  /*
   * key is destroyed. We only need to free the entry associated to it in the 
   * Tk_callback hash table (if it exists).
   */
  Tcl_HashEntry *entry;
  Tcl_HashTable *secondary_hash_table;

  if ((entry=Tcl_FindHashEntry(&Tk_callbacks, key))) {
    if (*key != '(' && (*key == 'a' && strncmp(key, "after#", 6) != 0)) {
      /* Delete the secondary hash table associated to this entry */
      secondary_hash_table = (Tcl_HashTable * ) Tcl_GetHashValue(entry);
      Tcl_DeleteHashTable(secondary_hash_table);
      free(secondary_hash_table);
    }
    /* Delete the entry itself */
    Tcl_DeleteHashEntry(entry);
  }
}


void STk_mark_callbacks(void)
{
  Tcl_HashEntry *entry1, *entry2;
  Tcl_HashSearch search1, search2;
  Tcl_HashTable *secondary;
  char *key;

  for (entry1 = Tcl_FirstHashEntry(&Tk_callbacks, &search1);
       entry1;
       entry1 = Tcl_NextHashEntry(&search1)) {
    
    key = Tcl_GetHashKey(&Tk_callbacks, entry1);
    if (*key == '(' || (*key == 'a' && strncmp(key, "after#", 6) == 0)) {
      /* No secondary hash table */
      STk_gc_mark((SCM) Tcl_GetHashValue(entry1));
    }
    else {
      /* We have a secondary hash table. Scan it  */
      secondary = (Tcl_HashTable * ) Tcl_GetHashValue(entry1);
      for (entry2 = Tcl_FirstHashEntry(secondary, &search2);
	   entry2;
	   entry2 = Tcl_NextHashEntry(&search2)) {
	STk_gc_mark((SCM) Tcl_GetHashValue(entry2));
      }
    }
  }
}

/* 
 * Return the parameters associated to the callback contained (as a string)
 * in the value parameter. If an error occurs, this function returns NULL
 *
 */
char *STk_append_callback_parameters(SCM proc)
{
  SCM param, port;

  if (!CLOSUREP(proc)) return NULL;
  param = CLOSURE_PARAMETERS(proc);

  if (NULLP(param) || CONSP(param)) {
    port = STk_open_output_string();
    STk_print(Cons(proc, param) , port, TK_MODE); 
    return CHARS(STk_get_output_string(port));
  }
  return NULL;
}


/******************************************************************************
 *
 * Tcl result manipulation functions
 *
 ******************************************************************************/

void STk_sharp_dot_result(Tcl_Interp *interp, char *value)
{
  /* Transform Tcl result in #.result so that it is evaluated when read */
  int len = strlen(value);
  char *s;

  s = (char *) STk_must_malloc((size_t) len + 3);
  s[0] = '#';
  s[1] = '.';
  strcpy(s+2, value);
  
  Tcl_SetResult(interp, s, TCL_DYNAMIC);
}

void STk_stringify_result(Tcl_Interp *interp, char *value)
{
  /* Transform Tcl result in "result" with " and \ escaped */
  Tcl_SetResult(interp,  STk_stringify(value, 0), TCL_DYNAMIC);
}

SCM STk_last_Tk_as_SCM(void)
{
  return STk_last_Tk_result;
}

SCM STk_get_NIL_value(void)
{
  return NIL;
}

SCM STk_get_widget_value(char *name)
{
  return STk_lookup_variable(name, STk_Tk_module);
}

/******************************************************************************
 *
 * Motif simulation
 *
 * Tk 4.0 uses a field in the Tk_Window structure to tell the library if it
 * must be conform to Motif look. This field is Tcl_LinkVar'ed.
 *
 ******************************************************************************/

static SCM get_Motif(char *s)
{
  TkWindow *p = (TkWindow *) Tk_MainWindow(STk_main_interp);
  return (p->mainPtr->strictMotif) ? Truth: Ntruth;
}

static void set_Motif(char *s, SCM value)
{ 
  TkWindow *p = (TkWindow *) Tk_MainWindow(STk_main_interp);
  p->mainPtr->strictMotif = !(value == Ntruth);
}


void STk_init_glue(void)
{
  /* 
   * Take into account the fact that Tk main window  name (i.e. ``.'') 
   * cannot be used in list since it leads to erroneous evaluation 
   * (e.g. [focus .] would produce an error since read will find a malformed
   * pair).
   *
   */
  STk_root_window = STk_lookup_variable(".", STk_Tk_module);
  STk_gc_protect(&STk_root_window);

  STk_define_variable(ROOT_WINDOW, STk_root_window, STk_Tk_module);

  /* Init the callback table */
  Tcl_InitHashTable(&Tk_callbacks, TCL_STRING_KEYS);
  
  /* Associate a getter and a setter for the global variable *Tk-strict-Motif*  */
  STk_define_C_variable("*tk-strict-motif*", get_Motif, set_Motif);
}

#endif /* USE_TK */
