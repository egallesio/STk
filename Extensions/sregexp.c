/*
 * sregexp.c -- Regular expressions for STk.
 * tromey Fri Jul 22 1994
 *
 */

#include <stk.h>
#include "tclRegexp.h"

/*
 * Regular expression type.  A regular expression is a function that
 * takes one argument.  It returns #f if no match, or a regular
 * expression match object on match.
 */

static void free_regexp (SCM ht);
static SCM apply_regexp (SCM x, SCM args, SCM env);

static int tc_regexp;

static STk_extended_scheme_type regexp_type = {
  "regexp",			/* name */
  EXT_ISPROC | EXT_EVALPARAM,	/* flags */
  NULL,				/* gc_mark_fct */
  free_regexp,			/* gc_sweep_fct */
  apply_regexp,			/* apply_fct */
  NULL,				/* display_fct */
  NULL				/* compare_fct */
};

#define REGEXP(x) 	((struct regexp *) (x)->storage_as.extension.data)
#define REGEXPP(x) 	(TYPEP((x), tc_regexp))

/*
 * GC interface.
 */

static void free_regexp (SCM reg)
{
  free (REGEXP (reg));
}

/*
 * Return #t if object is a regexp, #f otherwise.
 */
static PRIMITIVE regexp_p(SCM obj)
{
  return (REGEXPP (obj) ? Truth : Ntruth);
}

/*
 * Return compiled form of regexp represented by string.  Error if not
 * a string, or if regexp has a syntax error.
 */
static PRIMITIVE string_to_regexp (SCM obj)
{
  struct regexp *r;
  SCM z;

  ENTER_PRIMITIVE("string->regexp");
  
  if (NSTRINGP (obj)) 			     Serror("not a string", obj);
    if ((r=TclRegComp(CHARS (obj))) == NULL) Serror("error compiling regexp", obj);
  
  /* Regexp is Ok. Make a new cell and return it */
  NEWCELL(z, tc_regexp);
  z->storage_as.extension.data = (void *) r;
  return z;
}

/*
 * Try to match string against regular expression.  Returns sub-match
 * object, or #f if no match.
 */
static SCM apply_regexp(SCM regexp, SCM l, SCM env)
{
  SCM string;
  char *the_chars;

  ENTER_SCM("apply-regexp");

  if (STk_llength (l) != 1) Serror("bad number of args", l);
  string = CAR (l);

  if (NSTRINGP (string)) Serror("bad string", string);
  the_chars = CHARS (string);
  
  if (TclRegExec(REGEXP(regexp), the_chars, the_chars)) {
    struct regexp *r = REGEXP(regexp);
    SCM z	     = NIL;
    int i;
    
    /* Find the length of the result */
    for (i=0; r->startp[i]; i++) {/*Nothing*/}

    /* Build result */
    for (--i; i >= 0; i--) {
      z = Cons(LIST2(STk_makeinteger(r->startp[i]-the_chars), 
		     STk_makeinteger(r->endp[i]-the_chars)),
	       z);
    }
    return z;
  }
  return Ntruth;
}

/*
 * Initialization.
 */

PRIMITIVE STk_init_sregexp(void)
{
  tc_regexp = STk_add_new_type (&regexp_type);

  STk_add_new_primitive ("string->regexp", tc_subr_1, string_to_regexp);
  STk_add_new_primitive ("regexp?", tc_subr_1, regexp_p);
  return UNDEFINED;
}
