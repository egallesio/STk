/*
 *
 * e x t e n d . c			-- All the stuff dealing with 
 *					   extended types
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 15-Mar-1995 11:31
 * Last file update: 10-Jun-1996 00:02
 */

#include "stk.h"
#include "extend.h"

#define EXT_TYPE_DESCR(x)	(xtypes[TYPE(x)- tc_start_extd])

static STk_extended_scheme_type *xtypes[tc_stop_extd-tc_start_extd+1];
static int extended_type_stamp = tc_start_extd;


/******************************************************************************
 * 
 * Extended Types
 *
 ******************************************************************************/

/***
 ***
 *** Default functions
 ***
 ***/ 

static void internal_display(SCM obj, SCM port, int mode)
{
  sprintf(STk_tkbuffer, "#<%s %lx>", (EXT_TYPE_DESCR(obj))->type_name, 
	  			     (unsigned long) obj);
  Puts(STk_tkbuffer, PORT_FILE(port));
}

static SCM internal_apply(SCM obj, SCM args, SCM env)
{
  Err("apply: bad procedure", obj);
  return UNDEFINED; /* to make the compiler happy */
}

static SCM internal_compare(SCM x, SCM y, int equalp)
{
  return Ntruth;
}

/***
 *** 
 *** Utilities
 *** 
 ***/
void STk_extended_mark(SCM x)
{
  STk_extended_scheme_type *p= EXT_TYPE_DESCR(x);
  if (p->gc_mark_fct) (*(p->gc_mark_fct))(x);
}

void STk_extended_sweep(SCM x)
{
  STk_extended_scheme_type *p = EXT_TYPE_DESCR(x);
  if (p->gc_sweep_fct) (*(p->gc_sweep_fct))(x);
}

SCM STk_extended_apply(SCM x, SCM args, SCM env)
{
  return (*(EXT_TYPE_DESCR(x)->apply_fct))(x, args, env);
}

void STk_extended_display(SCM x, SCM port, int mode)
{
  (*(EXT_TYPE_DESCR(x)->display_fct))(x, port, mode);
}

int STk_extended_procedurep(SCM x)
{
  return (EXT_TYPE_DESCR(x)->flags && EXT_ISPROC);
}

int STk_extended_eval_parameters(SCM x)
{
  return (EXT_TYPE_DESCR(x)->flags && EXT_EVALPARAM);
}

SCM STk_extended_compare(SCM x, SCM y, int equalp)
{
  /* One of x or y (at least) is extended. */
  return EXTENDEDP(x) ? (*(EXT_TYPE_DESCR(x)->compare_fct))(x, y, equalp)
		      : (*(EXT_TYPE_DESCR(y)->compare_fct))(x, y, equalp);
}

/******************************************************************************
 * 
 * C-pointer
 *
 ******************************************************************************/

typedef void (*STk_disp_function)(SCM x, SCM port, int mode);

static int Cpointer_id 		        = 0;
static int size       		        = 0;
static STk_disp_function *display_array = NULL;


static void Cpointer_default_display(SCM obj, SCM port, int mode)
{
  sprintf(STk_tkbuffer, "#<C-pointer %d %lx>", EXTID(obj), EXTDATA(obj));
  Puts(STk_tkbuffer, PORT_FILE(port));
}


void STk_Cpointer_display(SCM obj, SCM port, int mode)
{
  (*(display_array[EXTID(obj)]))(obj, port, mode);
}


/******************************************************************************
 *
 * C variable 
 *
 ******************************************************************************/

static Tcl_HashTable Cvars;
static C_hash_table_initialized = 0;

struct get_n_set_box {
  SCM (*getter)();
  void (*setter)();
};

		  
SCM STk_apply_getter_C_variable(char *var)
{
  Tcl_HashEntry *entry;
  
  if (entry = Tcl_FindHashEntry(&Cvars, var)) {
    struct get_n_set_box *p = (struct get_n_set_box *) Tcl_GetHashValue(entry);
    
    return (*(p->getter))(var);
  }
  else {
    fprintf(STk_stderr, "internal error: %s variable has no getter!!\n", var);
    return UNDEFINED;    
  }
}

void STk_apply_setter_C_variable(char *var, SCM value)
{  
  Tcl_HashEntry *entry;
  
  if (entry = Tcl_FindHashEntry(&Cvars, var)) {
    struct get_n_set_box *p = (struct get_n_set_box *) Tcl_GetHashValue(entry);
    
    (*(p->setter))(var, value);
  }
  else
    fprintf(STk_stderr, "internal error: %s variable has no setter!!\n", var);
}

  
/******************************************************************************
 *
 * Extended types and C-pointer User interface
 *
 ******************************************************************************/

int STk_add_new_type(STk_extended_scheme_type *p)		       
{
  if (!p) Err("bad new type description", NIL);
  
  /* Set the apply procedure if not defined */
  if (!p->apply_fct) p->apply_fct = internal_apply;

  /* Replace NULL display function by a default function */
  if (!p->display_fct) p->display_fct = internal_display;

  /* Replace NULL compare function by a default function */
  if (!p->compare_fct) p->compare_fct = internal_compare;

  /* Store the new type descriptor in the xtypes array */
  xtypes[extended_type_stamp - tc_start_extd] = p;

  return extended_type_stamp++;
}

void STk_add_new_primitive(char *fct_name, int fct_type, struct obj * (*fct_ptr)())
{
  SCM z;

  NEWCELL(z, fct_type);
  z->storage_as.subr0.name = fct_name;
  z->storage_as.subr0.f    = fct_ptr;
  VCELL(Intern(fct_name))  = z;
}

SCM STk_eval_C_string(char *s, SCM env)
{
  SCM tmp = STk_internal_eval_string(s, ERR_OK, env);
  return tmp == EVAL_ERROR ? NULL: tmp;
}

			 /*************************/

int STk_new_Cpointer_id(void (*display_func)(SCM x, SCM port, int mode))
{
  if (++Cpointer_id >= size) {
    if (display_array == NULL) {
      display_array = must_malloc(10*sizeof (STk_disp_function));
      size = 10;
    }
    else {
      size += size / 2;
      display_array = must_realloc(display_array, 
				   size * sizeof (STk_disp_function));
    }
  }
  /* store function in array */
  display_array[Cpointer_id]= display_func? display_func : Cpointer_default_display;
  return Cpointer_id;
}

SCM STk_make_Cpointer(int Cpointer_id, void *ptr, int staticp)
{
  register SCM z;

  NEWCELL(z, tc_Cpointer);
  EXTDATA(z)    = ptr;
  EXTID(z)      = Cpointer_id;
  EXTSTATICP(z) = staticp;
  return z;
}

			 /*************************/

void STk_define_C_variable(char *var, SCM (*getter)(), void (*setter)())
{
  Tcl_HashEntry *entry;
  int new;
  struct get_n_set_box *p;

  if (!C_hash_table_initialized) {
    /* First C-var. Create Hash table */
    Tcl_InitHashTable(&Cvars, TCL_STRING_KEYS);
    C_hash_table_initialized = 1;
  }
  
  p         = must_malloc(sizeof(struct get_n_set_box));
  p->getter = getter;
  p->setter = setter;
  entry     = Tcl_CreateHashEntry(&Cvars, var, &new);
  if (!new) {
    fprintf(STk_stderr, "Attempt to multi-define C variable `%s' !!\n", var);
    return;
  }
  Tcl_SetHashValue(entry, p);

  /* Now enter variable in obarray and set its info field to C variable */
  Intern(var)->cell_info = CELL_INFO_C_VAR;
}
