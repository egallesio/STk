/******************************************************************************
 *
 * s t k . h
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 12-May-1993 10:34
 * Last file update: 25-Sep-1996 14:33
 *
 ******************************************************************************/

#ifndef _STK_H
#define _STK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <setjmp.h>
#include <assert.h>
#include <math.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#ifndef WIN32
#  include <memory.h>
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <stdlib.h>
#include "gmp.h"

/*
 * Header <tcl.h> is always included (even if not USE_TK) for the hash table
 * function prototypes. 
 */
#include <tcl.h>

/* ------------------------------------------------------------------------------ */

#define COMPACT_SMALL_CST			/* compact coding for small const */

#define FALSE			0
#define TRUE			1

#define TKBUFFERN 		1024		/* max size of a token */
#define MAX_CHAR_CODE		255		/* Max code for a char */

#ifdef USE_TK
#  define INITIAL_HEAP_SIZE 	25000		/* size of heap (in cells) */
#else
#  define INITIAL_HEAP_SIZE 	10000		/* size of heap (in cells) */
#endif

#ifdef _POSIX_PATH_MAX
#define MAX_PATH_LENGTH	 _POSIX_PATH_MAX
#else
#define MAX_PATH_LENGTH	 256
#endif

#define GC_VERBOSE	"*gc-verbose*"
#define ARGC		"*argc*"
#define ARGV		"*argv*"
#define PROG_NAME	"*program-name*"
#define DEBUG_MODE	"*debug*"
#define EVAL_HOOK	"*eval-hook*"
#define PRINT_BANNER	"*print-banner*"
#define LOAD_PATH	"*load-path*"
#define LOAD_SUFFIXES	"*load-suffixes*"
#define LOAD_VERBOSE	"*load-verbose*"

#define REPORT_ERROR	"report-error"

#ifdef USE_TK
#    include <tclInt.h>
#    include <tk.h>
#    define ROOT_WINDOW	"*root*"	/* Scheme name of main window */

  struct Tk_command {
    ClientData ptr;		/* pointer associated to the widget command */
    Tcl_CmdProc *fct;	   	/* Tk lib function associated to widget */
    Tcl_CmdDeleteProc *delproc; /* procedure to call when command is destroyed */
    ClientData deldata;	   	/* value to pass to delproc */
    int deleted;		/* 1 if command has already been deleted */
    char Id[1];		   	/* must be last field */
  };
#endif


struct obj {
  unsigned char type;
  unsigned char gc_mark;
  unsigned char cell_info;
  union {struct {struct obj * car; struct obj * cdr;} 		cons;
	 struct {double *data;}					flonum;
	 struct {char *pname; struct obj * vcell;} 		symbol;
	 struct {char *name; struct obj * (*f)(void);} 		subr0;
	 struct {char *name; struct obj * (*f)(void *,...);} 	subr;
	 struct {struct obj *env; struct obj *code;} 		closure;
	 struct {struct obj *code; }				macro;
	 struct {long dim; char *data;} 			string;
	 struct {long dim; struct obj **data;} 			vector;
	 struct {struct port_descr *p;}				port;
	 struct {char *data;} 					keyword;
	 struct {MP_INT *data;}					bignum;
	 struct {short level, position; struct obj *symbol;}	localvar;
	 struct {struct obj *expr; int resultknown; }		promise;
	 struct {void *data; }					cont;
	 struct {struct obj *data;}				env;
	 struct {short id; char staticp; void *data; }		extension;
#ifdef USE_STKLOS
    	 struct {int id; struct stklos_instance *data; }	instance;
#endif
#ifdef USE_TK
	 /* Idea of l_data comes from Alexander Taranov <tay@jet.msk.edu> */
	 struct {struct Tk_command *data; struct obj *l_data;}	tk;
#endif
       } storage_as;
};

typedef struct obj* SCM;
typedef struct obj* PRIMITIVE;


#define tc_nil    	0
#define tc_cons   	1
#define tc_flonum 	2
#define tc_integer	3
#define tc_bignum	4
#define tc_symbol 	5
#define tc_keyword	6
#define tc_subr_0 	7
#define tc_subr_1 	8
#define tc_subr_2 	9
#define tc_subr_3 	10
#define tc_subr_0_or_1  11
#define tc_subr_1_or_2  12
#define tc_subr_2_or_3	13
#define tc_lsubr  	14
#define tc_ssubr	15
#define tc_fsubr  	16
#define tc_syntax  	17
#define tc_closure 	18
#define tc_free_cell 	19
#define tc_char	        20
#define tc_string       21
#define tc_vector	22
#define tc_eof		23
#define tc_undefined	24
#define tc_iport       	25
#define tc_oport	26
#define tc_isport	27
#define tc_osport	28
#define tc_boolean	29
#define tc_macro	30
#define tc_localvar	31
#define tc_globalvar	32
#define tc_cont		33
#define tc_env		34
#define tc_address	35
#define tc_autoload	36
#define tc_Cpointer	37

#ifdef USE_STKLOS
#  define tc_instance	 40
#  define tc_next_method 41
#endif

#ifdef USE_TK
#  define tc_tkcommand	50
#endif

#define tc_quote	61
#define tc_lambda	62
#define tc_if		63
#define tc_setq		64
#define tc_cond		65 
#define tc_and		66
#define tc_or		67
#define tc_let		68
#define tc_letstar	69
#define tc_letrec	70
#define tc_begin	71
#define tc_promise	72
#define tc_apply	73
#define tc_call_cc	74
#define tc_dynwind	75
#define tc_extend_env   76
#define tc_unbound	80
#define tc_start_extd	90	/* Number of first extended type */
#define tc_stop_extd	127	/* Number of last extended type */


#define CAR(x) 		((*x).storage_as.cons.car)
#define CDR(x) 		((*x).storage_as.cons.cdr)
#define PNAME(x) 	((*x).storage_as.symbol.pname)
#define KEYVAL(x)	((*x).storage_as.keyword.data)
#define VCELL(x) 	((*x).storage_as.symbol.vcell)
#define SUBR0(x) 	(*((*x).storage_as.subr0.f))
#define SUBRF(x) 	(*((*x).storage_as.subr.f))
#define FLONM(x) 	(*((*x).storage_as.flonum.data))
#define CHARS(x)	((*x).storage_as.string.data)
#define STRSIZE(x)	((*x).storage_as.string.dim)
#define VECT(x)		((*x).storage_as.vector.data)
#define VECTSIZE(x)	((*x).storage_as.vector.dim)
#define BIGNUM(x)	((*x).storage_as.bignum.data)
#define EXTDATA(x)	((*x).storage_as.extension.data)
#define EXTID(x)	((*x).storage_as.extension.id)
#define EXTSTATICP(x)	((*x).storage_as.extension.staticp)


#ifdef COMPACT_SMALL_CST
#  define MAKE_SMALL_CST(x,type)  (((long) (x) << 8) |((type) << 1) | 1)
#  define SMALL_CST_TYPE(x)	  (((long) (x) >> 1) & 0x7F)
#  define SMALL_CST_VALUE(x)	  ((long)  (x) >> 8)
#  define SMALL_CSTP(x)		  ((long)  (x) & 1)
#  define TYPE(x)		  (SMALL_CSTP(x) ? (int)SMALL_CST_TYPE(x):(x)->type)

#  define INTEGER(x)		  SMALL_CST_VALUE(x)
#  define SET_INTEGER(x, v)	  (x = (SCM) MAKE_SMALL_CST(v, tc_integer))
#  define CHAR(x)		  ((unsigned char) SMALL_CST_VALUE(x))
#  define SET_CHARACTER(x, v)	  (x = (SCM) MAKE_SMALL_CST(v, tc_char))
#else
#  define SMALL_CSTP(x)		  FALSE
#  define TYPE(x)		  ((x)->type)

#  define INTEGER(x)		  ((long) ((x)->storage_as.extension.data))
#  define SET_INTEGER(x, v)	  (INTEGER(x) = (v))
#  define CHAR(x)		  ((unsigned char) ((x)->storage_as.extension.data)
#  define SET_CHARACTER(x, v)	  (CHAR(x) = (v))
#endif


#define EQ(x,y) 	((x) == (y))
#define NEQ(x,y) 	((x) != (y))
#define NULLP(x) 	EQ(x,NIL)
#define NNULLP(x) 	NEQ(x,NIL)

#define TYPEP(x,y) 	(TYPE(x) == (y))
#define NTYPEP(x,y) 	(TYPE(x) != (y))

#define CONSP(x)   	 TYPEP(x,tc_cons)
#define CLOSUREP(x)	 TYPEP(x,tc_closure)
#define FLONUMP(x) 	 TYPEP(x,tc_flonum)
#define SYMBOLP(x) 	 TYPEP(x,tc_symbol)
#define KEYWORDP(x)	 TYPEP(x,tc_keyword)
#define STRINGP(x)	 TYPEP(x,tc_string)
#define EOFP(x)		 TYPEP(x, tc_eof)
#define BOOLEANP(x)	 TYPEP(x, tc_boolean)
#define VECTORP(x)	 TYPEP(x,tc_vector)
#define IPORTP(x)	 TYPEP(x,tc_iport)
#define OPORTP(x)	 TYPEP(x,tc_oport)
#define ISPORTP(x)	 TYPEP(x,tc_isport)
#define OSPORTP(x)	 TYPEP(x,tc_osport)
#define INTEGERP(x)	 TYPEP(x,tc_integer)
#define BIGNUMP(x)	 TYPEP(x,tc_bignum)
#define NUMBERP(x)	 (FLONUMP(x) || INTEGERP(x) || BIGNUMP(x))
#define EXACTP(x)	 (INTEGERP(x) || BIGNUMP(x))
#define CHARP(x)	 TYPEP(x,tc_char)
#define PROMISEP(x)	 TYPEP(x,tc_promise)
#define CONTINUATIONP(x) TYPEP(x,tc_cont)
#define ENVP(x)		 TYPEP(x,tc_env)
#define MACROP(x)	 TYPEP(x,tc_macro)
#define EXTENDEDP(x)	 (tc_start_extd <= TYPE(x))
#define CPOINTERP(x)	 TYPEP(x,tc_Cpointer)

#define NCONSP(x)   	  NTYPEP(x,tc_cons)
#define NCLOSUREP(x)	  NTYPEP(x,tc_closure)
#define NFLONUMP(x) 	  NTYPEP(x,tc_flonum)
#define NSYMBOLP(x) 	  NTYPEP(x,tc_symbol)
#define NKEYWORDP(x)	  NTYPEP(x,tc_keyword)
#define NSTRINGP(x)	  NTYPEP(x,tc_string)
#define NEOFP(x)	  NTYPEP(x, tc_eof)
#define NBOOLEANP(x)	  NTYPEP(x, tc_boolean)
#define NVECTORP(x)	  NTYPEP(x,tc_vector)
#define NIPORTP(x)	  NTYPEP(x,tc_iport)
#define NOPORTP(x)	  NTYPEP(x,tc_oport)
#define NISPORTP(x)	  NTYPEP(x,tc_isport)
#define NOSPORTP(x)	  NTYPEP(x,tc_osport)
#define NINTEGERP(x)	  NTYPEP(x,tc_integer)
#define NBIGNUMP(x)	  NTYPEP(x,tc_bignum)
#define NNUMBERP(x)	  (NFLONUMP(x) && NINTEGERP(x) && NBIGNUMP(x))
#define NEXACTP(x)	  (NINTEGERP(x) && NBIGNUMP(x))
#define NCHARP(x)	  NTYPEP(x,tc_char)
#define NPROMISEP(x)	  NTYPEP(x,tc_promise)
#define NCONTINUATIONP(x) NTYPEP(x,tc_cont)
#define NENVP(x)	  NTYPEP(x,tc_env)
#define NMACROP(x)	  NTYPEP(x,tc_macro)
#define NEXTENDEDP(x)	  (!EXTENDEDP(x))
#define NCPOINTERP(x)	  NTYPEP(x,tc_Cpointer)

#ifdef USE_TK
#  define TKCOMMP(x)	  TYPEP(x,tc_tkcommand)
#  define NTKCOMMP(x)	  NTYPEP(x,tc_tkcommand)
#endif

#define ModifyCode()	NEQ(VCELL(STk_sym_debug), STk_truth)

#define SYNTAX_RETURN(x, need_eval) \
			  { *pform = (x); return (need_eval); }


#ifdef _DEBUG_MALLOC_INC
#define must_malloc(n)		malloc(n)
#define must_realloc(p, n)	realloc(p, n)
#endif


#define CELL_INFO_C_VAR 	01	/* Symbol is a C variable */
#define CELL_INFO_TRACED_VAR	02	/* Symbol is traced */



#define TRACED_VARP(var)	(((var)->cell_info) & CELL_INFO_TRACED_VAR)


#define Debug(message, obj) {fprintf(STk_stderr, "***%s",message); \
			     STk_print(obj, STk_curr_eport, WRT_MODE); \
			     fprintf(STk_stderr, "\n");}
#define TRACE(message)	{printf("In %s (%d):", __FILE__, __LINE__); \
			 printf message; putchar('\n');}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*
  ------------------------------------------------------------------------------
  ----
  ----  A D D R E S S . C
  ----
  ------------------------------------------------------------------------------
*/
SCM 	  STk_address2object(char *buffer);

PRIMITIVE STk_address_of(SCM obj);
PRIMITIVE STk_addressp(SCM address);

/*
  ------------------------------------------------------------------------------
  ----
  ----  A R G V . C
  ----
  ------------------------------------------------------------------------------
*/
#ifdef USE_TK
extern char *STk_arg_Xdisplay;
extern char *STk_arg_geometry;
extern char *STk_arg_name;
extern char *STk_arg_visual;
extern int   STk_arg_colormap;
extern int   STk_arg_sync;
extern int   STk_arg_no_tk;
#endif
extern char *STk_arg_file;
extern char *STk_arg_load;
extern char *STk_arg_cells;
extern char *STk_arg_image;
extern int   STk_arg_interactive;

char** STk_process_argc_argv(int argc, char **argv);
void   STk_save_unix_args_and_environment(int argc, char **argv);
void   STk_restore_unix_args_and_environment(int *argc, char ***argv);
void   STk_initialize_scheme_args(char **argv);


/*
  ------------------------------------------------------------------------------
  ----
  ----  B O O L E A N . C
  ----
  ------------------------------------------------------------------------------
*/
PRIMITIVE STk_not(SCM x);
PRIMITIVE STk_booleanp(SCM x);
PRIMITIVE STk_eqv(SCM x, SCM y);
PRIMITIVE STk_eq(SCM x,SCM y);
PRIMITIVE STk_equal(SCM x, SCM y);


/*
  ------------------------------------------------------------------------------
  ----
  ----  C H A R . C
  ----
  ------------------------------------------------------------------------------
*/
char  STk_string2char(char *s);
char *STk_char2string(char c);
SCM   STk_makechar(char c);

PRIMITIVE STk_charp(SCM obj);

PRIMITIVE STk_chareq   (SCM c1, SCM c2);
PRIMITIVE STk_charless (SCM c1, SCM c2);
PRIMITIVE STk_chargt   (SCM c1, SCM c2);
PRIMITIVE STk_charlesse(SCM c1, SCM c2);
PRIMITIVE STk_chargte  (SCM c1, SCM c2);

PRIMITIVE STk_chareqi   (SCM c1, SCM c2);
PRIMITIVE STk_charlessi (SCM c1, SCM c2);
PRIMITIVE STk_chargti   (SCM c1, SCM c2);
PRIMITIVE STk_charlessei(SCM c1, SCM c2);
PRIMITIVE STk_chargtei  (SCM c1, SCM c2);

PRIMITIVE STk_char_alphap(SCM c);
PRIMITIVE STk_char_numericp(SCM c);
PRIMITIVE STk_char_whitep(SCM c);
PRIMITIVE STk_char_upperp(SCM c);
PRIMITIVE STk_char_lowerp(SCM c);
PRIMITIVE STk_char2integer(SCM c);
PRIMITIVE STk_integer2char(SCM i);
PRIMITIVE STk_char_upper(SCM c);
PRIMITIVE STk_char_lower(SCM c);

/*
  ------------------------------------------------------------------------------
  ----
  ----  C O N T . C
  ----
  ------------------------------------------------------------------------------
*/
SCM 	  STk_mark_continuation(SCM cont);
void 	  STk_throw(SCM fct, SCM val);
SCM 	  STk_do_call_cc(SCM *x);
PRIMITIVE STk_continuationp(SCM obj);

void 	  STk_unwind_all(void);
PRIMITIVE STk_dynamic_wind(SCM thunk1, SCM thunk2, SCM thunk3);

/*
  ------------------------------------------------------------------------------
  ----
  ----  D U M P . C
  ----
  ------------------------------------------------------------------------------
*/
extern int STk_dumped_core;
void STk_restore_image(char *s);
PRIMITIVE STk_dump(SCM s);


/*
  ------------------------------------------------------------------------------
  ----
  ----  D Y N L O A D . C
  ----
  ------------------------------------------------------------------------------
*/
void STk_load_object_file(char *path);


/*
  ------------------------------------------------------------------------------
  ----
  ----  E N V . C
  ----
  ------------------------------------------------------------------------------
*/
#define STk_fast_extend_env(formals, actuals, env) \
	Cons(Cons((formals), (actuals)), (env))

SCM STk_makeenv(SCM l, int create_if_null);
SCM *STk_value_in_env(SCM var, SCM env);
SCM *STk_varlookup(SCM x, SCM env, int err_if_unbound);
SCM STk_localvalue(SCM var, SCM env);
SCM STk_extend_env(SCM formals, SCM actuals, SCM env, SCM who);

PRIMITIVE STk_symbol_boundp(SCM x, SCM env);
PRIMITIVE STk_the_environment(SCM args, SCM env, int len);
PRIMITIVE STk_parent_environment(SCM env);
PRIMITIVE STk_global_environment(void);
PRIMITIVE STk_environment2list(SCM env);
PRIMITIVE STk_environmentp(SCM obj);


/*
  ------------------------------------------------------------------------------
  ----
  ----  E R R O R . C
  ----
  ------------------------------------------------------------------------------
*/

#define EVAL_ERROR		((SCM) 1)

#define ERR_FATAL		000
#define ERR_OK			001
#define ERR_READ_FROM_STRING	002
#define ERR_IGNORED		004
#define ERR_TCL_BACKGROUND	010

#define JMP_INIT		0
#define JMP_ERROR		1
#define JMP_THROW		2
#define JMP_RESTORE		3

extern jmp_buf *STk_top_jmp_buf; /* Jump buffer denoting toplevel context */
extern long STk_error_context;

void STk_err(char *message, SCM x);

#define Err 		STk_err
#define err		STk_err	/* For compatibility: don't use it anymore */
#define Top_jmp_buf	STk_top_jmp_buf
#define Error_context	STk_error_context
/*
  ------------------------------------------------------------------------------
  ----
  ----  E V A L . C
  ----
  ------------------------------------------------------------------------------
*/

/* The eval flag which tells eval that it has something to do before
 * evaluating the form
 */
extern int STk_eval_flag;

/* Eval stack. These are internals of the evaluator. Don't care*/
void STk_show_eval_stack(int depth);
void STk_reset_eval_stack(void);
PRIMITIVE STk_get_eval_stack(void);

/* Eval hook management */
void STk_init_eval_hook(void);
void STk_reset_eval_hook(void);
PRIMITIVE STk_eval_hook(SCM x, SCM env, SCM hook);

/* Environment stack. These are internals of the evaluator. Don't care*/
SCM STk_top_env_stack(void);
PRIMITIVE STk_get_env_stack(void);

SCM STk_eval(SCM x,SCM env);
SCM STk_apply(SCM fct, SCM param);

PRIMITIVE STk_user_eval  (SCM expr, SCM env);
PRIMITIVE STk_eval_string(SCM str, SCM env);

#define Apply		  STk_apply
#define EVAL(x)	  	  (STk_eval((x), env))
#define EVALCAR(x)	  (SYMBOLP(CAR(x)) ? *STk_varlookup((x),env,1):EVAL(CAR(x)))
#define SET_EVAL_FLAG(v)  {STk_eval_flag = (v);}

/*
  ------------------------------------------------------------------------------
  ----
  ----  E X T E N D  . C
  ----
  ------------------------------------------------------------------------------
*/
#define EXT_ISPROC 	01	/* procedure? should answer #t */
#define EXT_EVALPARAM 	02	/* evaluates parameter list when apply */

#define STk_set_symbol_value(name,value)  {VCELL(Intern(name))=(value);}
#define STk_get_symbol_value(name)	  (VCELL(Intern(name)))

typedef struct {
  char *type_name;		/* The external name of this type */
  int  flags;			
  void (*gc_mark_fct)(SCM x);
  void (*gc_sweep_fct)(SCM x);
  SCM  (*apply_fct)(SCM x, SCM args, SCM env);
  void (*display_fct)(SCM x, SCM port, int mode);
  SCM  (*compare_fct)(SCM x, SCM y, int equalp);
  void *Reserved[7];		/* should be sufficient for a while */
} STk_extended_scheme_type;


int  STk_add_new_type(STk_extended_scheme_type *p);
void STk_add_new_primitive(char *fct_name, int fct_type, PRIMITIVE (*fct_ptr)());
SCM  STk_eval_C_string(char *s, SCM env);

int STk_new_Cpointer_id(void (*display_func)(SCM x, SCM port, int mode));
SCM STk_make_Cpointer(int Cpointer_id, void *ptr, int staticp);

void STk_define_C_variable(char *var, SCM (*getter)(), void (*setter)());

/*
  ------------------------------------------------------------------------------
  ----
  ----  G C . C
  ----
  ------------------------------------------------------------------------------
*/
extern SCM  STk_freelist;
extern long STk_alloc_cells;
extern int  STk_gc_requested;

#define NEWCELL(_into,_type)    			\
 { 							\
   if (STk_gc_requested || NULLP(STk_freelist))		\
      STk_gc_for_newcell();				\
   _into 	      = STk_freelist;			\
   STk_freelist       = CDR(STk_freelist);		\
   STk_alloc_cells   += 1;				\
   _into->type        = _type;				\
 }

void STk_gc_for_newcell(void);
void STk_gc_protect(SCM *location);   /* protect against GC this cell */
void STk_gc_unprotect(SCM *location); /* un-protect against GC this cell */
void STk_gc_mark(SCM location);       /* mark (recursively) this location */

PRIMITIVE STk_gc_stats(void);				/* + */
PRIMITIVE STk_gc(void);					/* + */
PRIMITIVE STk_find_cells(SCM type);			/* + */
PRIMITIVE STk_expand_heap(SCM arg);			/* + */

/*
  ------------------------------------------------------------------------------
  ----
  ----  I O . C
  ----
  ------------------------------------------------------------------------------
*/

#ifdef WIN32
   extern FILE *STk_stdin, *STk_stdout, *STk_stderr;
#else
#  define STk_stdin  stdin
#  define STk_stdout stdout
#  define STk_stderr stderr
#endif

void   STk_StdinProc(void);
int    STk_file_data_available(FILE *f);
int    STk_getc(FILE *f);
int    STk_ungetc(int c, FILE *f);
int    STk_putc(int c, FILE *f);
int    STk_puts(char *s, FILE *f);
int    STk_eof(FILE *f);
char * STk_line_bufferize_io(FILE *f);

#ifdef WIN32
void   STk_init_io(void);
#endif

#define Getc(f)         (STk_getc(f))
#define Ungetc(c, f)    (STk_ungetc((c), (f)))
#define Putc(c, f)      (STk_putc((c),   (f)))
#define Puts(s, f)      (STk_puts((s),   (f)))
#define Eof(f)		(STk_eof(f))


/*
  ------------------------------------------------------------------------------
  ----
  ----  K E Y W O R D . C
  ----
  ------------------------------------------------------------------------------
*/
void STk_initialize_keyword_table(void);
void STk_free_keyword(SCM keyword);

SCM STk_makekey(char *token);
PRIMITIVE STk_make_keyword(SCM str);
PRIMITIVE STk_keywordp(SCM obj);
PRIMITIVE STk_keyword2string(SCM obj);
PRIMITIVE STk_get_keyword(SCM key, SCM l, SCM default_value);

/*
  ------------------------------------------------------------------------------
  ----
  ---- L I S T . C
  ----
  ------------------------------------------------------------------------------
*/
int STk_llength(SCM l);	/* length of a list. -1 if not a proper list */

PRIMITIVE STk_pairp(SCM x);
PRIMITIVE STk_cons(SCM x, SCM y);
PRIMITIVE STk_car(SCM x);
PRIMITIVE STk_cdr(SCM x);
PRIMITIVE STk_setcar(SCM cell, SCM value);
PRIMITIVE STk_setcdr(SCM cell, SCM value);
PRIMITIVE STk_caar  (SCM l);
PRIMITIVE STk_cdar  (SCM l);
PRIMITIVE STk_cadr  (SCM l);
PRIMITIVE STk_cddr  (SCM l);
PRIMITIVE STk_caaar (SCM l);
PRIMITIVE STk_cdaar (SCM l);
PRIMITIVE STk_cadar (SCM l);
PRIMITIVE STk_cddar (SCM l);
PRIMITIVE STk_caadr (SCM l);
PRIMITIVE STk_cdadr (SCM l);
PRIMITIVE STk_caddr (SCM l);
PRIMITIVE STk_cdddr (SCM l);
PRIMITIVE STk_caaaar(SCM l);
PRIMITIVE STk_cdaaar(SCM l);
PRIMITIVE STk_cadaar(SCM l);
PRIMITIVE STk_cddaar(SCM l);
PRIMITIVE STk_caadar(SCM l);
PRIMITIVE STk_cdadar(SCM l);
PRIMITIVE STk_caddar(SCM l);
PRIMITIVE STk_cdddar(SCM l);
PRIMITIVE STk_caaadr(SCM l);
PRIMITIVE STk_cdaadr(SCM l);
PRIMITIVE STk_cadadr(SCM l);
PRIMITIVE STk_cddadr(SCM l);
PRIMITIVE STk_caaddr(SCM l);
PRIMITIVE STk_cdaddr(SCM l);
PRIMITIVE STk_cadddr(SCM l);
PRIMITIVE STk_cddddr(SCM l);
PRIMITIVE STk_nullp (SCM x);
PRIMITIVE STk_listp (SCM x);
PRIMITIVE STk_list  (SCM l, int len);
PRIMITIVE STk_list_length(SCM l);
PRIMITIVE STk_append(SCM l, int len);
PRIMITIVE STk_reverse(SCM l);
PRIMITIVE STk_list_tail(SCM list, SCM k);
PRIMITIVE STk_list_ref(SCM list, SCM k);
PRIMITIVE STk_memq  (SCM obj, SCM list);
PRIMITIVE STk_memv  (SCM obj, SCM list);
PRIMITIVE STk_member(SCM obj, SCM list);
PRIMITIVE STk_assq  (SCM obj, SCM alist);
PRIMITIVE STk_assv  (SCM obj, SCM alist);
PRIMITIVE STk_assoc (SCM obj, SCM alist);

PRIMITIVE STk_liststar(SCM l, int len);		/* + */
PRIMITIVE STk_copy_tree(SCM l);			/* + */

#define Cons			 STk_cons
#define Reverse			 STk_reverse
#define LIST1(a)		 Cons((a), NIL)
#define LIST2(a,b) 		 Cons((a), LIST1(b))
#define LIST3(a,b,c)		 Cons((a), LIST2((b),(c)))
#define LIST4(a,b,c,d)		 Cons((a), LIST3((b),(c),(d)))
#define LIST5(a,b,c,d,e)	 Cons((a), LIST4((b),(c),(d),(e)))
#define LIST6(a,b,c,d,e,f)	 Cons((a), LIST5((b),(c),(d),(e),(f)))
#define LIST7(a,b,c,d,e,f,g)	 Cons((a), LIST6((b),(c),(d),(e),(f),(g)))
#define LIST8(a,b,c,d,e,f,g,h)	 Cons((a), LIST7((b),(c),(d),(e),(f),(g),(h)))
#define LIST9(a,b,c,d,e,f,g,h,i) Cons((a), LIST8((b),(c),(d),(e),(f),(g),(h),(i)))

/*
  ------------------------------------------------------------------------------
  ----
  ---- M A C R O . C
  ----
  ------------------------------------------------------------------------------
*/
PRIMITIVE STk_macro(SCM args, SCM env, int len);		/* + */
PRIMITIVE STk_macro_expand(SCM form, SCM env, int len);		/* + */
PRIMITIVE STk_macro_expand_1(SCM form, SCM env, int len);	/* + */
PRIMITIVE STk_macro_body(SCM form);				/* + */
PRIMITIVE STk_macrop(SCM obj);					/* + */

/*
  ------------------------------------------------------------------------------
  ----
  ---- N U M B E R  . C
  ----
  ------------------------------------------------------------------------------
*/
#ifdef COMPACT_SMALL_CST
#  define SMALLINT_MAX	(LONG_MAX>>8)
#else
#  define SMALLINT_MAX  LONG_MAX
#endif
#define SMALLINT_MIN	(-SMALLINT_MAX)

char   *STk_number2Cstr(SCM n, long base, char buffer[]);
SCM	STk_Cstr2number(char *str, long base);
SCM	STk_makenumber(double x);
SCM	STk_makeinteger(long x);
long	STk_integer_value(SCM x); /* Returns LONG_MIN if not representable as int */
long	STk_integer_value_no_overflow(SCM x); /* Returns LONG_MIN if not an int */
int	STk_equal_numbers(SCM number1, SCM number2); /* number1 = number2 */

PRIMITIVE STk_numberp(SCM x);
PRIMITIVE STk_integerp(SCM x);
    
PRIMITIVE STk_exactp(SCM x);
PRIMITIVE STk_inexactp(SCM x);

PRIMITIVE STk_numequal(SCM l, SCM env, int from_eval);
PRIMITIVE STk_lessp(SCM l, SCM env, int from_eval);
PRIMITIVE STk_greaterp(SCM l, SCM env, int from_eval);
PRIMITIVE STk_lessep(SCM l, SCM env, int from_eval);
PRIMITIVE STk_greaterep(SCM l, SCM env, int from_eval);

PRIMITIVE STk_zerop(SCM n);
PRIMITIVE STk_positivep(SCM n);
PRIMITIVE STk_negativep(SCM n);
PRIMITIVE STk_oddp(SCM n);
PRIMITIVE STk_evenp(SCM n);

PRIMITIVE STk_max(SCM l, SCM env, int from_eval);
PRIMITIVE STk_min(SCM l, SCM env, int from_eval);

PRIMITIVE STk_plus(SCM l, SCM env, int from_eval);
PRIMITIVE STk_difference(SCM l, SCM env, int from_eval);
PRIMITIVE STk_times(SCM l, SCM env, int from_eval);
PRIMITIVE STk_division(SCM l, SCM env, int from_eval);

PRIMITIVE STk_absolute(SCM x);
PRIMITIVE STk_quotient(SCM n1, SCM n2);
PRIMITIVE STk_remainder(SCM n1, SCM n2);
PRIMITIVE STk_modulo(SCM n1, SCM n2);

PRIMITIVE STk_gcd(SCM l, SCM env, int from_eval);
PRIMITIVE STk_lcm(SCM l, SCM env, int from_eval);

PRIMITIVE STk_floor(SCM x);
PRIMITIVE STk_ceiling(SCM x);
PRIMITIVE STk_truncate(SCM x);
PRIMITIVE STk_round(SCM x);

PRIMITIVE STk_exp(SCM z);
PRIMITIVE STk_log(SCM z);
PRIMITIVE STk_sin(SCM z);
PRIMITIVE STk_cos(SCM z);
PRIMITIVE STk_tan(SCM z);
PRIMITIVE STk_asin(SCM z);
PRIMITIVE STk_acos(SCM z);
PRIMITIVE STk_atan(SCM y, SCM x);
PRIMITIVE STk_sqrt(SCM z);
PRIMITIVE STk_expt(SCM z1, SCM z2);

PRIMITIVE STk_exact2inexact(SCM z);
PRIMITIVE STk_inexact2exact(SCM z);
PRIMITIVE STk_string2number(SCM str, SCM base);
PRIMITIVE STk_number2string(SCM n, SCM base);

PRIMITIVE  STk_bignump(SCM n);

/*
  ------------------------------------------------------------------------------
  ----
  ---- P O R T . C
  ----
  ------------------------------------------------------------------------------
*/

struct port_descr {		/* Slot order is important (see sport_descr) */
  FILE *file;
  int  flags;
  char *filename;
  SCM  read_event;
  SCM  write_event;
};

#define PORT_FILE(x)	((x)->storage_as.port.p->file)
#define PORT_NAME(x)	((x)->storage_as.port.p->filename)
#define PORT_FLAGS(x)	((x)->storage_as.port.p->flags)
#define PORT_REVENT(x)	((x)->storage_as.port.p->read_event)
#define PORT_WEVENT(x)	((x)->storage_as.port.p->write_event)

#define PORT_CLOSED 	01
#define PIPE_PORT	02

#define OUTP(p) 	(OPORTP(p) || OSPORTP(p))
#define INP(p)  	(IPORTP(p) || ISPORTP(p))
#define F_READ  	01
#define F_WRITE 	02

/* external vars */
extern SCM STk_curr_iport, STk_curr_oport, STk_curr_eport, STk_eof_object;


void 	  STk_freeport(SCM port);
void 	  STk_init_standard_ports(void);
SCM 	  STk_loadfile(char *fname, int err_if_absent);
SCM 	  STk_Cfile2port(char *name, FILE *f, int type, int flags);

PRIMITIVE STk_input_portp(SCM port);
PRIMITIVE STk_output_portp(SCM port);
PRIMITIVE STk_current_input_port(void);
PRIMITIVE STk_current_output_port(void);
PRIMITIVE STk_current_error_port(void);
PRIMITIVE STk_with_input_from_file(SCM string, SCM thunk);
PRIMITIVE STk_with_output_to_file(SCM string, SCM thunk);
PRIMITIVE STk_open_input_file(SCM filename);
PRIMITIVE STk_open_output_file(SCM filename);
PRIMITIVE STk_close_input_port(SCM port);
PRIMITIVE STk_close_output_port(SCM port);
PRIMITIVE STk_read(SCM port);
PRIMITIVE STk_read_char(SCM port);
PRIMITIVE STk_peek_char(SCM port);
PRIMITIVE STk_eof_objectp(SCM obj);
PRIMITIVE STk_char_readyp(SCM port);
PRIMITIVE STk_write(SCM expr, SCM port);
PRIMITIVE STk_display(SCM expr, SCM port);
PRIMITIVE STk_newline(SCM port);
PRIMITIVE STk_write_char(SCM c, SCM port);
PRIMITIVE STk_scheme_load(SCM filename);

/* Non standard functions */
PRIMITIVE STk_format(SCM l, int len);
PRIMITIVE STk_error(SCM l, int len);
PRIMITIVE STk_try_load(SCM filename);
PRIMITIVE STk_open_file(SCM filename, SCM mode);
PRIMITIVE STk_close_port(SCM port);
PRIMITIVE STk_read_line(SCM port);
PRIMITIVE STk_flush(SCM porSTk_t);

void      STk_do_autoload(SCM var);
PRIMITIVE STk_autoload(SCM l, SCM env, int len);
PRIMITIVE STk_autoloadp(SCM l, SCM env, int len);

PRIMITIVE STk_when_port_readable(SCM port, SCM closure);
PRIMITIVE STk_when_port_writable(SCM port, SCM closure);

/*
  ------------------------------------------------------------------------------
  ----
  ---- P R I M I T I V E S . C
  ----
  ------------------------------------------------------------------------------
*/
void STk_init_primitives(void);

/*
  ------------------------------------------------------------------------------
  ----
  ---- P R I N T . C
  ----
  ------------------------------------------------------------------------------
*/
#define DSP_MODE	01
#define WRT_MODE	02
#define TK_MODE		04	/* Always defined even if no Tk support */


SCM STk_print(SCM exp, SCM port, int mode);

/*
  ------------------------------------------------------------------------------
  ----
  ---- P R O C . C
  ----
  ------------------------------------------------------------------------------
*/
#define CLOSURE_PARAMETERS(p)		(CAR((p)->storage_as.closure.code))

int       STk_is_thunk(SCM obj);
PRIMITIVE STk_procedurep(SCM obj);
PRIMITIVE STk_map(SCM l, int len);
PRIMITIVE STk_for_each(SCM l, int len);
PRIMITIVE STk_procedure_body(SCM proc);
PRIMITIVE STk_procedure_environment(SCM proc);

/*
  ------------------------------------------------------------------------------
  ----
  ---- P R O M I S E . C
  ----
  ------------------------------------------------------------------------------
*/
PRIMITIVE STk_force(SCM promise);
PRIMITIVE STk_promisep(SCM promise);


/*
  ------------------------------------------------------------------------------
  ----
  ---- R E A D . C
  ----
  ------------------------------------------------------------------------------
*/
SCM STk_readf(FILE *f, int case_significant);


/*
  ------------------------------------------------------------------------------
  ----
  ---- S I G N A L . C
  ----
  ------------------------------------------------------------------------------
*/
#define MAX_SYSTEM_SIG 		32		    /* True for all systems? */
#define SIGHADGC		MAX_SYSTEM_SIG	    /* End of a GC run */
#define MAX_SIGNAL		(MAX_SYSTEM_SIG+1)  

extern STk_sigint_counter;
extern STk_control_C;

void      STk_handle_signal(int sig);
PRIMITIVE STk_add_signal_handler(SCM sig, SCM proc);
PRIMITIVE STk_set_signal_handler(SCM sig, SCM proc);
PRIMITIVE STk_get_signal_handlers(SCM sig);

void 	  STk_init_signal(void);
void 	  STk_mark_signal_table(void);

#define STk_disallow_sigint() {STk_sigint_counter += 1;}
#define STk_allow_sigint()    {STk_sigint_counter -= 1;}


/*
  ------------------------------------------------------------------------------
  ----
  ---- S L I B . C
  ----
  ------------------------------------------------------------------------------
*/
#ifndef _DEBUG_MALLOC_INC
void *STk_must_malloc(unsigned long size);
void *STk_must_realloc(void *ptr, unsigned long size);
#endif

double   STk_my_time(void);

SCM 	 STk_internal_eval_string(char *s, long err_code, SCM env);

PRIMITIVE STk_catch(SCM expr, SCM env, int len);
PRIMITIVE STk_quit_interpreter(SCM retcode);
PRIMITIVE STk_version(void);
PRIMITIVE STk_machine_type(void);
PRIMITIVE STk_library_location(void);
PRIMITIVE STk_random(SCM n);
PRIMITIVE STk_set_random_seed(SCM n);
PRIMITIVE STk_get_internal_info(void);
PRIMITIVE STk_time(SCM expr, SCM env, int len);
PRIMITIVE STk_uncode(SCM expr);
#ifdef SIGSEGV
PRIMITIVE STk_default_SIGSEGV_handler(void);
#endif
void STk_delete_Tcl_child_Interp(void);
void STk_panic TCL_VARARGS_DEF(char *,arg1);


#define must_malloc  STk_must_malloc
#define must_realloc STk_must_realloc


/*
  ------------------------------------------------------------------------------
  ----
  ---- S P O R T . C
  ----
  ------------------------------------------------------------------------------
*/

struct sport_descr {		/* Slot order is important (see port_descr) */
  FILE *file;
  int  flags;
};


SCM 	  STk_internal_open_input_string(char *s);
void 	  STk_free_string_port(SCM port);
SCM 	  STk_internal_read_from_string(SCM port, int *eof, int case_significant);
PRIMITIVE STk_open_input_string(SCM s);
PRIMITIVE STk_open_output_string();
PRIMITIVE STk_get_output_string(SCM port);
PRIMITIVE STk_input_string_portp(SCM port);
PRIMITIVE STk_output_string_portp(SCM port);
PRIMITIVE STk_with_input_from_string(SCM string, SCM thunk);
PRIMITIVE STk_with_output_to_string(SCM thunk);
PRIMITIVE STk_read_from_string(SCM str);

/*
  ------------------------------------------------------------------------------
  ----
  ---- S T K L O S . C
  ----
  ------------------------------------------------------------------------------
*/
#ifdef USE_STKLOS
#  define STKLOS_VERSION	"2.2b1"
   PRIMITIVE STk_init_STklos(void);
#endif


/*
  ------------------------------------------------------------------------------
  ----
  ---- S T R I N G . C
  ----
  ------------------------------------------------------------------------------
*/
SCM       STk_makestrg(int len, char *init);

PRIMITIVE STk_stringp(SCM obj);
PRIMITIVE STk_make_string(SCM len, SCM init_char);
PRIMITIVE STk_lstring(SCM l, int len);
PRIMITIVE STk_string_length(SCM str);

PRIMITIVE STk_string_ref(SCM str, SCM index);
PRIMITIVE STk_string_set(SCM str, SCM index, SCM value);

PRIMITIVE STk_streq   (SCM s1, SCM s2);
PRIMITIVE STk_strless (SCM s1, SCM s2);
PRIMITIVE STk_strgt   (SCM s1, SCM s2);
PRIMITIVE STk_strlesse(SCM s1, SCM s2);
PRIMITIVE STk_strgte  (SCM s1, SCM s2);

PRIMITIVE STk_streqi   (SCM s1, SCM s2);
PRIMITIVE STk_strlessi (SCM s1, SCM s2);
PRIMITIVE STk_strgti   (SCM s1, SCM s2);
PRIMITIVE STk_strlessei(SCM s1, SCM s2);
PRIMITIVE STk_strgtei  (SCM s1, SCM s2);

PRIMITIVE STk_substring(SCM string, SCM start, SCM end);
PRIMITIVE STk_string_append(SCM l, int len);
PRIMITIVE STk_string2list(SCM str);
PRIMITIVE STk_list2string(SCM l);
PRIMITIVE STk_string_copy(SCM str);
PRIMITIVE STk_string_fill(SCM str, SCM c);

PRIMITIVE STk_string_findp(SCM s1, SCM s2);			/* + */
PRIMITIVE STk_string_index(SCM s1, SCM s2);			/* + */
PRIMITIVE STk_string_lower(SCM s);				/* + */
PRIMITIVE STk_string_upper(SCM s);				/* + */

#define STk_makestring(s) STk_makestrg(strlen(s), (s))


/*
  ------------------------------------------------------------------------------
  ----
  ---- S Y M B O L . C
  ----
  ------------------------------------------------------------------------------
*/
void 	  STk_initialize_symbol_table(void);
void 	  STk_mark_symbol_table(void);
void 	  STk_free_symbol(SCM symbol);
SCM  	  STk_global_env2list(void);
SCM 	  STk_intern(char *name);

PRIMITIVE STk_symbolp(SCM x);
PRIMITIVE STk_symbol2string(SCM symbol);
PRIMITIVE STk_string2symbol(SCM string);

#define Intern	STk_intern

/*
  ------------------------------------------------------------------------------
  ----
  ---- S Y N T A X . C
  ----
  ------------------------------------------------------------------------------
*/
PRIMITIVE STk_syntax_quote     (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_lambda    (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_if        (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_setq      (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_cond      (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_and       (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_or        (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_let       (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_letstar   (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_letrec    (SCM *pform, SCM env, int len);

PRIMITIVE STk_syntax_begin     (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_delay     (SCM *pform, SCM env, int len);
PRIMITIVE STk_syntax_quasiquote(SCM *pform, SCM env, int len);

PRIMITIVE STk_syntax_define    (SCM *pform, SCM env, int len);

PRIMITIVE STk_while(SCM l, SCM env, int len);
PRIMITIVE STk_until(SCM l, SCM env, int len);
PRIMITIVE STk_syntax_extend_env(SCM *pform, SCM env, int len);
/*
  ------------------------------------------------------------------------------
  ----
  ---- T O P L E V E L . C
  ----
  ------------------------------------------------------------------------------
*/
void STk_toplevel(int argc, char **argv);

/*
  ------------------------------------------------------------------------------
  ----
  ---- T R A C E . C
  ----
  ------------------------------------------------------------------------------
*/
void 	  STk_change_value	 (SCM var, SCM env);
void 	  STk_mark_tracevar_table(void);
PRIMITIVE STk_trace_var		 (SCM var, SCM code);
PRIMITIVE STk_untrace_var	 (SCM var);

/*
  ------------------------------------------------------------------------------
  ----
  ---- U N I X . C
  ----
  ------------------------------------------------------------------------------
*/

#ifndef WIN32
#  define ISDIRSEP(ch) 	 ((ch)=='/')
#  define ISABSOLUTE(cp) (ISDIRSEP(*cp))
#  define DIRSEP 	 '/'
#  define SDIRSEP  	 "/"
#  define PATHSEP	 ':'
#else
#  define ISDIRSEP(ch) 	 ((ch)=='\\' || (ch)=='/')
#  define ISABSOLUTE(cp) (ISDIRSEP(*cp) || \
			 (isalpha(*cp)&&*((cp)+1)==':')&&ISDIRSEP(*((cp)+1)))
#  define DIRSEP 	 '\\'
#  define SDIRSEP  	 "\\"
#  define PATHSEP	 ';'
#endif


void 	  STk_whence(char *exec, char *path);
SCM 	  STk_internal_expand_file_name(char *s);
SCM 	  STk_resolve_link(char *path, int count);
int 	  STk_is_directory(const char *path);

PRIMITIVE STk_expand_file_name(SCM s);
PRIMITIVE STk_canonical_path(SCM str);
PRIMITIVE STk_getcwd(void);
PRIMITIVE STk_chdir(SCM s);
PRIMITIVE STk_getpid(void);
PRIMITIVE STk_system(SCM com);
PRIMITIVE STk_getenv(SCM str);
PRIMITIVE STk_setenv(SCM var, SCM value);

PRIMITIVE STk_file_is_directoryp(SCM f);
PRIMITIVE STk_file_is_regularp(SCM f);
PRIMITIVE STk_file_is_readablep(SCM f);
PRIMITIVE STk_file_is_writablep(SCM f);
PRIMITIVE STk_file_is_executablep(SCM f);
PRIMITIVE STk_file_existp(SCM f);
PRIMITIVE STk_file_glob(SCM l, int len);




/*
  ------------------------------------------------------------------------------
  ----
  ---- V E C T O R . C
  ----
  ------------------------------------------------------------------------------
*/
SCM STk_makevect(int len, SCM init);

PRIMITIVE STk_vectorp(SCM obj);
PRIMITIVE STk_make_vector(SCM len, SCM init);
PRIMITIVE STk_vector(SCM l, int len);
PRIMITIVE STk_vector_length(SCM v);
PRIMITIVE STk_vector_ref(SCM v, SCM index);
PRIMITIVE STk_vector_set(SCM v, SCM index, SCM value);
PRIMITIVE STk_vector2list(SCM v);
PRIMITIVE STk_list2vector(SCM l);
PRIMITIVE STk_vector_fill(SCM v, SCM fill);

PRIMITIVE STk_vector_copy(SCM vect);
PRIMITIVE STk_vector_resize(SCM vect, SCM size);
PRIMITIVE STk_sort(SCM obj, SCM test);


/*
  ------------------------------------------------------------------------------
  ----
  ---- U S E R I N I T . C
  ----
  ------------------------------------------------------------------------------
*/
void STk_user_init(void);
void STk_user_cleanup(void);

/*
  ------------------------------------------------------------------------------
  ----
  ---- G L O B A L   V A R I A B L E S 
  ----
  ------------------------------------------------------------------------------
*/

#ifdef STK_MAIN
#   define Extern
#else
#   define Extern extern
#endif

/* Remember if we are running the stk or snow interpreter */
Extern int STk_snow_is_running;

/* Program name (expanded) */
Extern char STk_Argv0[MAX_PATH_LENGTH];

/* Is it an intearctive interpreter? */
Extern int STk_interactivep;

/* Scheme booleans #t and #f */
Extern SCM STk_truth, STk_ntruth;

/* Scheme () and the undefined value */
Extern SCM STk_nil, STk_undefined, STk_unbound;

/* read buffer */
Extern char *STk_tkbuffer;
Extern int STk_line_counter;
Extern SCM STk_current_filename;

/* Special symbols */
Extern SCM STk_sym_lambda, STk_sym_quote,STk_sym_dot, STk_sym_imply, 
			  STk_sym_debug, STk_sym_else, STk_sym_quasiquote, 
           STk_sym_unquote, STk_sym_unquote_splicing, STk_sym_break;;

/* Dynamic-wind */
Extern SCM STk_wind_stack;

/* Global environment */
Extern SCM STk_globenv;

/* Library location */
Extern char *STk_library_path;


#undef  Extern
#define Truth 	  STk_truth
#define Ntruth	  STk_ntruth
#define NIL	  STk_nil
#define UNBOUND   STk_unbound
#define UNDEFINED STk_undefined

#define Sym_lambda 	 STk_sym_lambda
#define Sym_quote 	 STk_sym_quote
#define Sym_dot 	 STk_sym_dot
#define Sym_imply  	 STk_sym_imply
#define Sym_debug 	 STk_sym_debug
#define Sym_else 	 STk_sym_else
#define Sym_quasiquote   STk_sym_quasiquote
#define Sym_unquote 	 STk_sym_unquote
#define Sym_unq_splicing STk_sym_unquote_splicing
#define Sym_break	 STk_sym_break
	
#ifdef USE_TK
/*
  ------------------------------------------------------------------------------
  ----
  ---- T C L - G L U E . C
  ----
  ------------------------------------------------------------------------------
*/
void  STk_init_glue(void);
SCM   STk_execute_Tcl_lib_cmd(SCM cmd, SCM args, SCM env, int eval_args);

/*
 * STk_Stringify permits to transform the string "s" in a valid STk string.
 * Original string is deallocated if free_original is 1 
 */
char *STk_stringify(char *s, int free_original);
#endif

#ifdef USE_TK
/*
  ------------------------------------------------------------------------------
  ----
  ---- T K - U T I L . C
  ----
  ------------------------------------------------------------------------------
*/

PRIMITIVE STk_string2widget(SCM str);
PRIMITIVE STk_widget2string(SCM widget);
PRIMITIVE STk_tk_commandp(SCM obj);
PRIMITIVE STk_widget_name(SCM obj);
PRIMITIVE STk_get_widget_data(SCM widget);
PRIMITIVE STk_set_widget_data(SCM widget, SCM value);
PRIMITIVE STk_widget_environment(SCM widget);

/*
  ------------------------------------------------------------------------------
  ----
  ---- T K - M A I N . C
  ----
  ------------------------------------------------------------------------------
*/
extern Tcl_Interp *STk_main_interp;	/* Interpreter for this application. */
extern int Tk_initialized ;		/* 1 when Tk is fully initialized */

void Tk_main(int synchronize, char *name, char *fileName, char *Xdisplay,
	     char *geometry);

#endif

#ifdef __cplusplus
};
#endif
#endif /* ifndef _STK_H */
