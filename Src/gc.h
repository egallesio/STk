/*
 *
 *  g c . h			-- Mark and Sweep Garbage Collector 
 *
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 17-Feb-1993 12:27
 * Last file update: 10-Feb-1995 19:59
 *
 */

#define GC_MARK			01 /* To mark a cell */

extern SCM *STk_stack_start_ptr;
extern double STk_total_gc_time;

void STk_gc_count_cells(long *allocated, long *used, long *calls);
void STk_mark_stack(SCM *start, SCM *end); 
int  STk_valid_address(SCM p);      /* true if p is a valid address */
void STk_init_gc(void);
