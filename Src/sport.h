/*
 * s p o r t . h			-- String ports management
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
 * Last file update:  3-Sep-1999 20:22 (eg)
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


