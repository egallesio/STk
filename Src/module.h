/*
 *
 * m o d u l e . h			-- A simple module sytem
 *
 * Copyright © 1993-1998 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *    Creation date: 13-Mar-1997 20:11
 * Last file update: 14-Sep-1998 13:49
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


