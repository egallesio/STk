/*
 *
 * t c l - g l u e . c 		- Glue functions between the scheme and Tcl worlds
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  6-Aug-1997 12:48
 * Last file update:  3-Sep-1999 20:59 (eg)
 *
 */

#include "stk.h"
#include "tcl-glue.h"
#include "module.h"
#include "gc.h"
#include "extend.h"

#ifdef USE_TK
extern SCM STk_root_window;
#endif

SCM STk_convert_Tcl_string2list(char *s)
{
  register SCM tmp1, tmp2, z, port;
  SCM result = NIL;
  int eof;
  
#ifdef USE_TK
  SCM module;

  /* Evaluation takes place in the Tk module (for '#.' objects mainly) */
  module              = STk_selected_module;  
  STk_selected_module = STk_Tk_module;
#endif

  if (*s) {
    /* Create a string port to read in the result */
    port   = STk_internal_open_input_string(s);
    result = STk_internal_read_from_string(port, &eof, TRUE);
#ifdef USE_TK
    if (result == Sym_dot) result = STk_root_window;
#endif
    if (!eof) {
      /*  Result was a list of value, build a proper Scheme list */
      tmp1 = result = LIST1(result);
      for ( ; ; ) {
	z = STk_internal_read_from_string(port, &eof, TRUE);
	if (z == EVAL_ERROR || EOFP(z)) break;
#ifdef USE_TK
	if (z == Sym_dot) z = STk_root_window;
#endif
	NEWCELL(tmp2, tc_cons);
	CAR(tmp2) = z; 
	CDR(tmp1) = tmp2;
	tmp1      = tmp2;
      }
      CDR(tmp1) = NIL;
    }
    /* close_string_port(port); */
  }
#ifdef USE_TK
  STk_selected_module = module;
#endif
  return (result == EVAL_ERROR)? UNDEFINED: result;
}  

char *STk_convert_for_Tcl(SCM obj, SCM *res)
{
  switch (TYPE(obj)) {
    case tc_symbol:    *res = obj; return PNAME(obj);
    case tc_integer:
    case tc_bignum:
    case tc_flonum:    *res = STk_number2string(obj, UNBOUND); return CHARS(*res);
    case tc_string:    *res = obj; return CHARS(obj);
#ifdef USE_TK
    case tc_tkcommand: return (obj->storage_as.tk.data)->Id;
#endif
    case tc_keyword:   *res = obj; return obj->storage_as.keyword.data;
    case tc_boolean:   *res = STk_makestring((obj == Truth)? "#t" : "#f");
      		       return CHARS(*res);
    default:           /* Ok, take the big hammer (i.e. use a string port for 
			* type coercion) Here, use write (and not display) 
			* since it handles complex data structures containing
			* eventually special chars which must be escaped
			* Ex: (bind .w "<Enter>" '(display "<Enter>"))
			*     First <Enter> is unquotted and second is not
			*/
		       {
			 SCM port;
			 
			 port = STk_open_output_string();
			 STk_print(obj, port, TK_MODE); 
			 *res = STk_get_output_string(port);
			 return CHARS(*res);
		       }
  }
}

/*
 * STk_stringify permits to transform the string "s" in a valid STk string.
 * Original string is deallocated if free_original is 1 
 */

char *STk_stringify(char *s, int free_original)
{
  char *res, *d;
  
  if (s == NULL) s = "";
  res = d = must_malloc(2 * strlen(s) + 3); /* worst overestimation */
  
  for ( *d++ = '"'; *s; s++, d++) {
    if (*s == '"' || *s == '\\') *d++ = '\\';
    *d = *s;
  }
  *d++ = '"';
  *d   = '\0';
  
  if (free_original) free(s);
  return res;
}

/*******************************************************************************
 *
 * Accessing Scheme variable in a given environment from Tcl implementation code
 *
 ******************************************************************************/

int STk_valid_environment(char *s, void **env)
{
  /* An environment is valid iff it is of the form "#pxxxx" where xxxx is composed
   * only of hexadecimal digits.
   * This procedure is more restrictive than its cousin procedure 
   * STk_valid_procedure because it accepts only environment whereas 
   * the valid_procedure accepts closure AND strings for pre STk3.0 
   * compatibility reasons.
   */
  unsigned long tmp;
  int l = strlen(s);
  char *p;

  if (l == 0 || (l == 2 && s[0] == '#' && s[1] == 'f')) {
#ifdef USE_TK    
    *env = STk_makeenv(MOD_ENV(STk_Tk_module), 0);
#else
    *env = STk_globenv;
#endif
    return TRUE;
  }

  if (l > 2) {
    if (s[0] == '#' && s[1] == 'p') {
      /* Verify that the rest of the string only contains hexadecimal digits */
      for (p = s + 2; *p; p++)
	if (!isxdigit(*p)) return FALSE;
      
      sscanf(s+2, "%lx", &tmp);
      if (STk_valid_address((SCM) tmp) && ENVP((SCM) tmp)) {
	*(SCM *)env = (SCM) tmp;
	return TRUE;
      }
    }
  }
  return FALSE;
}


char *STk_tcl_getvar(char *name, char *env)
{
  SCM dumb, V, e;
  
  if (!STk_valid_environment(env, (void **) &e)) return NULL;

  V = *(STk_varlookup(Intern(name), e->storage_as.env.data, 0));

  if (V == UNBOUND) {
    /* We should probably complain here, but this would break old code */
    STk_tcl_setvar(name, "", 0, env);
    return "";
  }
  else 
    return STk_convert_for_Tcl(V, &dumb);
}


char *STk_tcl_getvar2(char *name1, char *name2, char *env)
{
  if (name2 && *name2) {
    char *res, *s = must_malloc(strlen(name1) + strlen(name2) + 3);

    sprintf(s, "%s{%s}", name1, name2);
    res = STk_tcl_getvar(s, env);
    free(s);
    return res;
  }
  return STk_tcl_getvar(name1,env);
}


char *STk_tcl_setvar(char *name, char *val, int flags, char *env)
{
  register SCM var, value;
  SCM e, *tmp;

  if (!STk_valid_environment(env, (void **) &e)) return NULL;

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

  var = Intern(name);

  tmp = STk_varlookup(var, e->storage_as.env.data, 0);
  if (*tmp == UNBOUND) {
    if (var->cell_info & CELL_INFO_C_VAR) {
      /* This is not an unbound variable but rather a C variable */
	STk_apply_setter_C_variable(PNAME(var), value);
	goto Out;
    }
    /* Unbound variable. Define it in the given environment
     * Use big hammer here, since it occcurs only once (at most)
     * for a given widget
     */
    STk_eval(LIST3(Intern("define"), var, value), e->storage_as.env.data);
    return val;
  }
  *tmp = value;
Out:
  STk_change_value(var, e);
  return val;
}


char *STk_tcl_setvar2(char *name1, char *name2, char *val, int flags, char *env)
{ 
  if (name2 && *name2) {
    char *res, *s = must_malloc(strlen(name1) + strlen(name2) + 3);

    sprintf(s, "%s{%s}", name1, name2);
    res = STk_tcl_setvar(s, val, flags, env);
    free(s);
    return res;
  }
  return STk_tcl_setvar(name1, val, flags, env);
}

/******************************************************************************
 *
 * Tcl_Obj procedures (the one which are not defined in Tcl)
 *
 ******************************************************************************/

Tcl_Obj *STk_create_tcl_object(SCM data)
{
  SCM z;
  
  NEWCELL(z, tc_tclobject);
  TCLOBJDATA(z)   = data;
  return (Tcl_Obj *) z;
}

Tcl_Obj *STk_NewKeywordObj(char *str)
{
  char copy[100];  /* big enough !!! */
  
  strcpy(copy, str);	/* because STk_makekey ill patch first char */
  return STk_create_tcl_object(STk_makekey(copy));
}

Tcl_Obj *STk_NewSymbolObj(char *str)
{
  return STk_create_tcl_object(Intern(str));
}
