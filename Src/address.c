/*
 *
 *  a d d r e s s . c			-- Addresses management
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
 *            Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 16-Mar-1994 10:34
 * Last file update: 22-Feb-1996 23:05
 *
 *
 */

#include "stk.h"
#include "gc.h"

SCM STk_address2object(char *buffer)
{
  SCM address;

  sscanf(buffer, "%lx", &address);
  if (!STk_valid_address(address)) 
    Err("bad address", STk_makestring(buffer));
  return address;
}


PRIMITIVE STk_address_of(SCM obj)
{
  SCM z;

  NEWCELL(z, tc_address);
  z->storage_as.env.data = obj;

  return z;
}

PRIMITIVE STk_addressp(SCM address)
{
  return TYPEP(address, tc_address)? Truth: Ntruth;
}
