/*
 * v p o r t . h			-- Virtual ports management
 *
 * Copyright © 1998 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *    Creation date: 26-Oct-1998 15:55
 * Last file update: 29-Oct-1998 14:40
 *
 */


#define VPORT_SIGNATURE	 0xabcdef01	/* Magic number for a virtual port	*/
#define READING		 01
#define WRITING		 02
#define VIRTUAL_PORTP(f) (((struct str_iob *) f)->signature == VPORT_SIGNATURE)

/* There is one structure for input AND ouput vport. This should allow 
 * to esily define RW virtual port in the future
 */

struct virtual_iob {
  long int  signature;
  short	    flag;
  int	    ungetted;
  SCM	    getc;
  SCM	    readyp;
  SCM	    eofp;
  SCM	    close;
  SCM	    putc;
  SCM 	    puts;
  SCM	    flush;
};


