/*
 *
 *  a d d r e s s . c			-- Addresses management
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
 *            Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 16-Mar-1994 10:34
 * Last file update:  3-Sep-1999 20:18 (eg)
 *
 *
 */

#include "stk.h"
#include "gc.h"

SCM STk_address2object(char *buffer)
{
  SCM address;

  sscanf(buffer, "%lx", (unsigned long *) &address);
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
