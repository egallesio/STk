/*
 *
 * m o d u l e . c			-- A simple module sytem
 *
 * Copyright © 1997-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 13-Mar-1997 20:11
 * Last file update:  3-Sep-1999 20:21 (eg)
 */

#include "stk.h"
#include "module.h"
#include "extend.h"

/* Public variables */
SCM STk_global_module;		/* Global module -- i.e STk pseudo-module */
SCM STk_scheme_module;		/* The Scheme module */
SCM STk_selected_module;	/* Selected module */

/* Private variables */
static Tcl_HashTable module_table;	/* Table of all modules */


static SCM find_module(SCM name, int fake_module, int create)
{
  SCM z;
  struct Tcl_HashTable *h;
  struct module_info *p;
  Tcl_HashEntry *entry;
  int new;

  entry = Tcl_FindHashEntry(&module_table, PNAME(name));
  if (!entry) {
    /* This is a new module */
    if (!create) return UNBOUND;

    if (!fake_module) {
      h = STk_must_malloc(sizeof(struct Tcl_HashTable));
      Tcl_InitHashTable(h, TCL_ONE_WORD_KEYS);
    }
    else
      h = NULL;
  
    entry = Tcl_CreateHashEntry(&module_table, PNAME(name), &new);

    p = STk_must_malloc(sizeof(struct module_info));
    p->name = name;
    p->exported_symbols = p->imports = p->env = p->ref_module = NIL;
    p->export_all = FALSE;
    
    NEWCELL(z, tc_module);
    MOD_TABLE(z) = h;
    MOD_INFO(z)  = p;

    /* set now p->env to the module itself */
    p->env     = LIST1(z);

    /* Enter this module in the table of modules */
    Tcl_SetHashValue(entry, z);
    return z;
  }
  else {
    /* This module was alreay defined. Return the value stored in table */
    return (SCM) Tcl_GetHashValue(entry);
  }
}

SCM STk_make_module(SCM name)
{
  return find_module(name, FALSE, TRUE);
}


SCM STk_make_parent_module(SCM module)
{
  SCM z, m;
  struct module_info *p;

  if (NULLP(MOD_IMPORTED(module))) return Ntruth;

  /* Build a fake module starting from the infos found in "module" 
   * Fake module is identical to the first imported module, excepting
   * its imported list which is based on the one of "module"
   */
  p = STk_must_malloc(sizeof(struct module_info));
  m = CAR(MOD_IMPORTED(module));

  *p            = *(struct module_info *)MOD_INFO(m); 
  p->imports    = CDR(MOD_IMPORTED(module));
  p->ref_module = m;

  NEWCELL(z, tc_module);
  MOD_TABLE(z) = NULL;
  MOD_INFO(z)  = p;

  /* set now p->env to the module itself */
  p->env     = LIST1(z);
  return z;
}


void STk_mark_module(SCM m)
{ 
  struct Tcl_HashTable *H = MOD_TABLE(m);
  register SCM var, value;

  STk_gc_mark(MOD_NAME(m));
  STk_gc_mark(MOD_EXPORTS(m));
  STk_gc_mark(MOD_IMPORTED(m));
  STk_gc_mark(MOD_REFMOD(m));
  STk_gc_mark(MOD_ENV(m));

  if (H) {    		/* mark the symbols of the table and their value */
    register Tcl_HashEntry *ent;
    Tcl_HashSearch tmp;

    for (ent=Tcl_FirstHashEntry(H, &tmp); ent; ent=Tcl_NextHashEntry(&tmp)){
      var   = (SCM) Tcl_GetHashKey(H, ent);
      value = (SCM) Tcl_GetHashValue(ent);
      if (value == UNBOUND) {
	/* Delete var  from the table */
	Tcl_DeleteHashEntry(ent);
      }
      else {
	STk_gc_mark((SCM) Tcl_GetHashKey(H, ent));
	STk_gc_mark((SCM) Tcl_GetHashValue(ent));
      }
    }
  }
}

void STk_free_module(SCM m)
{
  struct Tcl_HashTable *H = MOD_TABLE(m);
  if (H) {
    Tcl_DeleteHashTable(H);
    free(H);
  }
  free(m->storage_as.module.data);
}

void STk_mark_module_table(void)
{
  Tcl_HashEntry *ent;
  Tcl_HashSearch tmp;
  
  for (ent=Tcl_FirstHashEntry(&module_table, &tmp);ent;ent=Tcl_NextHashEntry(&tmp))
    STk_gc_mark((SCM) Tcl_GetHashValue(ent));
}


void STk_define_public_var(SCM module, SCM var, SCM value)
{
  Tcl_HashEntry *entry;
  int new;

  if (NULLP(module) || module == STk_global_module) {  /* Global variable */
    if (VCELL(var) == UNBOUND && (var->cell_info&CELL_INFO_C_VAR)) {
      /* This is not an unbound variable but rather a C variable */
      STk_apply_setter_C_variable(PNAME(var), value);
    }
    else {
      VCELL(var) = value;
    }
  }
  else {						/* Module variable */	
    entry = Tcl_CreateHashEntry(FIND_TABLE(module), (char *) var, &new);
    Tcl_SetHashValue(entry, value);
  }
}


SCM STk_modulevalue(SCM obj)
{
  return (SCM) Tcl_GetHashValue((Tcl_HashEntry *) CDR(obj));
}


SCM STk_module_env2list(SCM module)
{
  register Tcl_HashEntry *ent;
  struct Tcl_HashTable   *H;
  Tcl_HashSearch 	 tmp;
  SCM l, m, res=NIL;

  if (FAKE_MODULEP(module)) {
    m = (module == STk_global_module) ? MOD_IMPORTED(module) :  
      				    Cons(module, MOD_IMPORTED(module));
  }
  else {
    /* Build the list of all module variables */
    H = MOD_TABLE(module);
    for (ent=Tcl_FirstHashEntry(H,&tmp); ent; ent=Tcl_NextHashEntry(&tmp)) {
      res = Cons(Cons((SCM)Tcl_GetHashKey(H, ent), (SCM)Tcl_GetHashValue(ent)),res);
    }
    m   = MOD_IMPORTED(module);
    res = LIST1(res);
  }

  /* For all imported modules, build the list of its exported variables */
  for ( ; NNULLP(m); m = CDR(m)) {
    H = FIND_TABLE(CAR(m));

    for (l=NIL,ent=Tcl_FirstHashEntry(H,&tmp); ent; ent=Tcl_NextHashEntry(&tmp)) {
      SCM var = (SCM) Tcl_GetHashKey(H, ent);
    
      if (MOD_EXPORTALL(CAR(m)) || STk_memq(var, MOD_EXPORTS(CAR(m))) != Ntruth)
	l = Cons(Cons(var, (SCM) Tcl_GetHashValue(ent)), l);
    }
   res = STk_append2(LIST1(l), res);
  }
  return res;
}


static SCM module_body(SCM module, SCM l)
{
  SCM prev_module  = STk_selected_module;
  SCM env 	   = MOD_ENV(module);
  volatile SCM res = UNDEFINED;

  STk_selected_module = module;

  PUSH_ERROR_HANDLER
    {
      register SCM tmp;

      for (tmp=l; NNULLP(tmp);  tmp = CDR(tmp))
	res = EVALCAR(tmp);
    }
  WHEN_ERROR
    {
      STk_selected_module = prev_module;
      PROPAGATE_ERROR();
    }
  POP_ERROR_HANDLER;
  
  STk_selected_module = prev_module;
  return res;
}


/******************************************************************************
 *
 * d e f i n e - m o d u l e
 *
 ******************************************************************************/
 
PRIMITIVE STk_define_module(SCM l, SCM env, int len)
{
  SCM name, module;

  ENTER_PRIMITIVE("define-module");
  if (!len)		     Serror("bad module definition", NIL);
  if (NSYMBOLP(name=CAR(l))) Serror("bad name",		     name);

  module = find_module(name, FALSE, TRUE);

  if (len > 1) module_body(module, CDR(l));
  STk_last_defined = name
;
  return UNDEFINED;
}


PRIMITIVE STk_modulep(SCM obj)
{
  return MODULEP(obj)? Truth: Ntruth;
}


/******************************************************************************
 *
 * w i t h - m o d u l e
 *
 ******************************************************************************/
PRIMITIVE STk_with_module(SCM l, SCM env, int len)
{
  SCM name, module;

  ENTER_PRIMITIVE("with-module");
  if (len < 2)    	     Serror("no body",  NIL);
  if (NSYMBOLP(name=CAR(l))) Serror("bad name", name);

  module = find_module(name, FALSE, FALSE);
  if (module == UNBOUND)     Serror("module does not exist", name);

  /* Everything is correct. Evaluate the module body in its environment */
  return module_body(module, CDR(l));
}


/******************************************************************************
 *
 * i m p o r t
 *
 ******************************************************************************/
static void import_module(SCM importer, SCM imported)
{
  if (NEQ(importer, imported) &&
      NEQ(imported, STk_global_module) &&
      STk_memv(imported, MOD_IMPORTED(importer)) == Ntruth)
    MOD_IMPORTED(importer) = STk_append2(MOD_IMPORTED(importer), LIST1(imported));
}

PRIMITIVE STk_import(SCM l, SCM env, int len)
{
  SCM current_module = NULLP(env) ? STk_global_module : CAR(env);

  ENTER_PRIMITIVE("import");
  if (NMODULEP(current_module)) Serror("not allowed in a nested environment", NIL);

  if (len) { 
    /* Import into a module */
    for ( ; NNULLP(l); l = CDR(l)) {
      SCM name, module;
      
      if (NSYMBOLP(name=CAR(l))) Serror("bad name", name);
      if ((module=find_module(name, FALSE, FALSE)) == UNBOUND)
	Serror("bad module", name);
      
      import_module(current_module, module);
    }
  }
  return UNDEFINED;
}

/******************************************************************************
 *
 * e x p o r t - s y m b o l
 *
 ******************************************************************************/
PRIMITIVE STk_export_symbol(SCM symbol, SCM module)
{
  ENTER_PRIMITIVE("export-symbol");
  if (NSYMBOLP(symbol)) Serror("bad symbol", symbol);
  if (NMODULEP(module)) Serror("bad module", module);

  if (! MOD_EXPORTALL(module)) {
    if (STk_memv(symbol,  MOD_EXPORTS(module)) == Ntruth) {
      MOD_EXPORTS(module) = Cons(symbol, MOD_EXPORTS(module));
    }
  }
  return UNDEFINED;
}

/******************************************************************************
 *
 * e x p o r t - a l l - s y m b o l s
 *
 ******************************************************************************/
PRIMITIVE STk_export_all_symbols(void)
{
  MOD_EXPORTALL(STk_selected_module) = TRUE;
  MOD_EXPORTS(STk_selected_module)   = NIL; /* let GC continue to do the job */
  return UNDEFINED;
}


/******************************************************************************
 *
 * select-module/current-module
 *
 ******************************************************************************/
PRIMITIVE STk_select_module(SCM l, SCM env, int len)
{
  SCM name, module;

  ENTER_PRIMITIVE("select-module");
  if (NNULLP(env) && NMODULEP(CAR(env)))
    Serror("not allowed in a nested environment", NIL);

  if (len != 1)    	     Serror("no module specified",  NIL);
  if (NSYMBOLP(name=CAR(l))) Serror("bad name", name);

  module = find_module(name, FALSE, FALSE);
  if (module == UNBOUND)     Serror("module does not exist", name);

  STk_selected_module = module;
  return UNDEFINED;
}

static SCM get_module(SCM env)
{
  /* find the last pair */
  for ( ; CONSP(env); env = CDR(env)) {
    if (NULLP(CDR(env)))
      return MODULEP(CAR(env)) ? CAR(env) : STk_global_module;
  }
  
  /* If we are here, env must be NULL, return STk_global_module */
  return STk_global_module;
}


PRIMITIVE STk_get_module(SCM env)			/* subr */
{
  ENTER_PRIMITIVE("%get-module");
  if (NENVP(env)) Serror("bad environment", env);

  return get_module(env->storage_as.env.data);
}

PRIMITIVE STk_current_module(SCM l, SCM env, int len)	/*  fsubr */
{
  /* Find the module from the environment. This is important to 
   * compute the module from the environment (instead of just returning
   * the value of STk_selected_module); otherwise closures would return
   * a value which don't agree with their own environment (see it as a 
   * static current_environment).
   */
  ENTER_PRIMITIVE("current-module");
  if (len) Serror("no parameter required", l);
  return get_module(env);
}


/******************************************************************************
 *
 * Introspective primitives
 *
 ******************************************************************************/

PRIMITIVE STk_find_module(SCM name, SCM default_value)
{
  SCM module;

  ENTER_PRIMITIVE("find-module");
  if (NSYMBOLP(name)) Serror("bad name", name);

  module = find_module(name, FALSE, FALSE);
  if (module == UNBOUND) {
    if (default_value == UNBOUND) Serror("module does not exists", name);
    return default_value;
  }
  return module;
}

PRIMITIVE STk_module_name(SCM module)
{
  ENTER_PRIMITIVE("module-name");
  if (NMODULEP(module)) Serror("bad module", module);

  return MOD_NAME(module);
}

PRIMITIVE STk_module_imports(SCM module)
{
  ENTER_PRIMITIVE("module-imports");
  if (NMODULEP(module)) Serror("bad module", module);

  return MOD_IMPORTED(module);
}

PRIMITIVE STk_module_exports(SCM module)
{
  ENTER_PRIMITIVE("module-exports");
  if (NMODULEP(module)) Serror("bad module", module);

  return MOD_EXPORTALL(module)? STk_module_symbols(module) : MOD_EXPORTS(module);
}

PRIMITIVE STk_module_env(SCM module)
{
  ENTER_PRIMITIVE("module-environment");
  if (NMODULEP(module)) Serror("bad module", module);

  return STk_makeenv(MOD_ENV(module), FALSE);
}

PRIMITIVE STk_module_symbols(SCM module)
{
  register Tcl_HashEntry *ent;
  struct Tcl_HashTable   *H;
  Tcl_HashSearch 	 tmp;
  SCM 			 res=NIL;

  ENTER_PRIMITIVE("module-symbols");
  if (NMODULEP(module)) Serror("bad module", module);

  if (FAKE_MODULEP(module))
    return EQ(module, STk_global_module)? STk_global_symbols(): Ntruth;
  else {
    /* Build the list of all module variables */
    H = MOD_TABLE(module);
    for (ent=Tcl_FirstHashEntry(H,&tmp); ent; ent=Tcl_NextHashEntry(&tmp)) {
      if ((SCM) Tcl_GetHashValue(ent) != UNBOUND)
	res = Cons((SCM)Tcl_GetHashKey(H, ent), res);
    }
    return res;
  }
}

PRIMITIVE STk_get_selected_module(void)
{
  return STk_selected_module;
}

PRIMITIVE STk_all_modules(void)
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch search;
  SCM result = NIL;

  for (entry = Tcl_FirstHashEntry(&module_table, &search);
       entry;
       entry = Tcl_NextHashEntry(&search)) {

    result = Cons((SCM) Tcl_GetHashValue(entry), result);
  }
  return result;
}

void STk_initialize_stk_module(void)
{
  struct Tcl_HashTable *h = FIND_TABLE(STk_scheme_module);
  SCM symbols 		  = STk_module_symbols(STk_scheme_module);
  register SCM var, value, tmp;

  /* Duplicate all the bindings from the Scheme module to the global one
   * So, if the user redefines a standard binding, the old one will be 
   * still available in the Scheme module (except if explicitly destroyed).
   * The copy is only done iff the global variable is unbound
   * (primitives are already in STk module and don't need to be copied 
   * again but it is also useful to let the user redefine standard behaviour
   * in his/her .stkrc file)
   */  
  for (tmp = symbols; NNULLP(tmp); tmp = CDR(tmp)) {
    var   = CAR(tmp);
    value = (SCM) Tcl_GetHashValue(Tcl_FindHashEntry(h, (char*) var));
    if (NTYPEP(value, tc_autoload) && EQ(VCELL(var), UNBOUND))
      VCELL(var) = value;
   }
  /* And now set the default module to the global one */
  STk_selected_module = STk_global_module;
}


/******************************************************************************
 *
 * init
 *
 ******************************************************************************/

void STk_init_modules(void)
{
  SCM stk_module, scheme_module;

  /* Initialize the table of modules */
  Tcl_InitHashTable(&module_table, TCL_STRING_KEYS);

  /* Define the STk and Scheme modules  -- Params: fake, create */  
  stk_module    = find_module(Intern("stk"),    TRUE,  TRUE);
  scheme_module = find_module(Intern("scheme"), FALSE, TRUE);
  
  /* Theses module export all their symbols */
  MOD_EXPORTALL(stk_module) = MOD_EXPORTALL(scheme_module) = TRUE;

  STk_global_module = stk_module;
  STk_scheme_module = STk_selected_module = scheme_module;
}
