/*
 * v p o r t . h			-- Virtual ports management
 *
 * Copyright © 1998-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *    Creation date: 26-Oct-1998 15:55
 * Last file update:  3-Sep-1999 21:02 (eg)
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


