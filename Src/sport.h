/*
 * s p o r t . h			-- String ports management
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 17-Feb-1993 12:27
 * Last file update: 27-Dec-1998 20:58
 *
 */

#define SPORT_SIGNATURE	0xabcdef00	/* Magic number for a string port	*/
#define START_ALLOC	128		/* Initial size of an ouput string port */
#define READING		01
#define WRITING		02
#define STRING_PORTP(f) (((struct str_iob *) f)->signature == SPORT_SIGNATURE)

struct str_iob {
  long int		signature;
       short		flag;
       int		cnt;
       unsigned char 	*ptr;
       unsigned char 	*base;
       int		bufsiz;
};


