/*
 *
 *  g c . h			-- Mark and Sweep Garbage Collector 
 *
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 17-Feb-1993 12:27
 * Last file update:  3-Sep-1999 20:20 (eg)
 *
 * Win32 DLL support by Steve Pruitt <steve@pruitt.net>
 *  	- Modified stack_start_ptr and total_gc_time for IMPORT_DLL_GLOBALS
 *
 */

#define GC_MARK			01 /* To mark a cell */

#if (defined(USE_DYNLOAD) && defined(MSC_VER) && defined(WIN32))
#  ifdef IMPORT_DLL_GLOBALS
    __declspec( dllimport ) SCM *STk_stack_start_ptr;
    __declspec( dllimport ) double STk_total_gc_time;
#  else
     extern SCM *STk_stack_start_ptr;
     extern double STk_total_gc_time;
#  endif
#else
extern SCM *STk_stack_start_ptr;
extern double STk_total_gc_time;
#endif

void STk_gc_count_cells(long *allocated, long *used, long *calls);
void STk_mark_stack(SCM *start, SCM *end); 
int  STk_valid_address(SCM p);      /* true if p is a valid address */
void STk_init_gc(void);
