/*
 * s p o r t . h			-- String ports management
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
 * Last file update:  6-Oct-1996 22:07
 *
 *
 * This is achieved in a (surely very) dependant way. A string port is implemented
 * via a pseudo FILE descriptor malloc'd when open-input-string is called. This 
 * descriptor is released when free-string-port is called.
 */

#define SPORT_SIGNATURE	0xabcdef00	/* Magic number for a string port	*/
#define START_ALLOC	128		/* Initial size of an ouput string port */
#define READING		01
#define WRITING		02

struct str_iob {
  long int		signature;
       short		flag;
       int		cnt;
       unsigned char 	*ptr;
       unsigned char 	*base;
       int		bufsiz;
};


