/*
 *
 * e x t e n d . c			-- All the stuff dealing with 
 *					   extended types
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
 *    Creation date: 15-Mar-1995 11:31
 * Last file update:  3-Sep-1999 20:19 (eg)
 */

#include "stk.h"
#include "stklos.h"
#include "extend.h"

#define EXT_TYPE_DESCR(x)	(xtypes[TYPE(x)- tc_start_extd])

static int extended_type_stamp = tc_start_extd;
static STk_extended_scheme_type *xtypes[tc_stop_extd-tc_start_extd+1];

#ifdef USE_STKLOS
static SCM xclasses[tc_stop_extd-tc_start_extd+1];
#endif

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
  Puts(STk_tkbuffer, port);
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

char *STk_get_extended_name(int i)
{ 
  return ((i>=tc_start_extd) && (i<extended_type_stamp)) ? 
    		(xtypes[i - tc_start_extd])->type_name : 
  		NULL;
}

#ifdef USE_STKLOS
void STk_register_extended_class(SCM c, int type_index)
{
  if (NNULLP(c)) {
    int i = type_index - tc_start_extd;
    xclasses[i] = c;
    STk_gc_protect(xclasses+i);
  }
}

SCM STk_extended_class_of(SCM obj)   /* Warning: obj MUST be an extd type */
{ 
  return  xclasses[TYPE(obj)-tc_start_extd];
}
#endif

/******************************************************************************
 * 
 * C-pointer
 *
 ******************************************************************************/

typedef void (*STk_disp_function)(SCM x, SCM port, int mode);

static int Cpointer_id 		        = ANONYMOUS_DYN_PTR_ID + 1;
static int size       		        = 0;
static STk_disp_function *display_array = NULL;


static void Cpointer_default_display(SCM obj, SCM port, int mode)
{
  sprintf(STk_tkbuffer, "#<C-pointer %d %lx>", EXTID(obj), 
	  				       (unsigned long) EXTDATA(obj));
  Puts(STk_tkbuffer, port);
}


void STk_Cpointer_display(SCM obj, SCM port, int mode)
{
  int id = EXTID(obj);

  if (id <= ANONYMOUS_DYN_PTR_ID) 
    Cpointer_default_display(obj, port, mode);
  else
    (*(display_array[id]))(obj, port, mode);
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

  if ((entry = Tcl_FindHashEntry(&Cvars, var))) {
    struct get_n_set_box *p = (struct get_n_set_box *) Tcl_GetHashValue(entry);
    
    return (*(p->getter))(var);
  }
  else {
    Fprintf(STk_curr_eport, "internal error: %s variable has no getter!!\n", var);
    return UNDEFINED;    
  }
}

void STk_apply_setter_C_variable(char *var, SCM value)
{  
  Tcl_HashEntry *entry;

  if ((entry = Tcl_FindHashEntry(&Cvars, var))) {
    struct get_n_set_box *p = (struct get_n_set_box *) Tcl_GetHashValue(entry);
    
    (*(p->setter))(var, value);
  }
  else
    Fprintf(STk_curr_eport, "internal error: %s variable has no setter!!\n", var);
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

#ifdef USE_STKLOS
  /* Create a new class for this type and register it */
  STk_register_extended_class(STk_make_extended_class(p->type_name),
			      extended_type_stamp);
#endif
  return extended_type_stamp++;
}

void STk_add_new_primitive(char *fct_name, int fct_type, struct obj * (*fct_ptr)())
{
  SCM z;

  NEWCELL(z, fct_type);
  z->storage_as.subr0.name = fct_name;
  z->storage_as.subr0.f    = fct_ptr;
  STk_define_public_var(STk_selected_module, Intern(fct_name), z);
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
    Fprintf(STk_curr_eport, "Attempt to multi-define C variable `%s' !!\n", var);
    return;
  }
  Tcl_SetHashValue(entry, p);

  /* Now enter variable in obarray and set its info field to C variable */
  Intern(var)->cell_info = CELL_INFO_C_VAR;
}
