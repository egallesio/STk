/*
 *
 * m o d u l e . h			-- A simple module sytem
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
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 13-Mar-1997 20:11
 * Last file update:  3-Sep-1999 20:21 (eg)
 */

struct module_info {
  SCM name;		/* module name */
  SCM exported_symbols;	/* symbols declared as exported */
  SCM imports;		/* imported modules */
  SCM env;		/* module environment */ 
  SCM ref_module;	/* module associated to a fake one */
  int export_all;	/* #t if all symbols are exported */
};

#define MOD_TABLE(x)		((x)->storage_as.module.t)
#define MOD_INFO(x)		((x)->storage_as.module.data)
#define LMOD_INFO(x)		((struct module_info*)MOD_INFO(x))

#define MOD_NAME(x)		(LMOD_INFO(x)->name)
#define MOD_EXPORTS(x)		(LMOD_INFO(x)->exported_symbols)
#define MOD_IMPORTED(x)		(LMOD_INFO(x)->imports)
#define MOD_ENV(x)		(LMOD_INFO(x)->env)
#define MOD_REFMOD(x)		(LMOD_INFO(x)->ref_module)
#define MOD_EXPORTALL(x)	(LMOD_INFO(x)->export_all)
#ifdef USE_STKLOS
#  define MOD_CLASSES(x)	(LMOD_INFO(x)->classes)
#endif
#define FAKE_MODULEP(x)		(!MOD_TABLE(x))
#define FIND_TABLE(m)		(MOD_TABLE(MOD_TABLE(m) ? m : MOD_REFMOD(m)))


