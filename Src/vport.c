/*
 * v p o r t . c			-- Virtual ports management
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
 * Last file update:  5-Nov-1998 16:06
 *
 */

#include "stk.h"
#include "vport.h"


static void verify_param(char *proc_name, SCM  proc, int pos) 
{
  if (proc != Ntruth && STk_procedurep(proc) != Truth) 
    Serror("Uncorrect procedure at position", STk_makeinteger(pos));
}


void STk_mark_virtual_port(SCM port)
{
  struct virtual_iob *p;

  p = (struct virtual_iob *) PORT_FILE(port);
  STk_gc_mark(p->getc);
  STk_gc_mark(p->readyp);
  STk_gc_mark(p->eofp);
  STk_gc_mark(p->close);
  STk_gc_mark(p->putc);
  STk_gc_mark(p->puts);
  STk_gc_mark(p->flush);
}

void STk_free_virtual_port(SCM port)
{
  struct virtual_iob *p;

  p = (struct virtual_iob *) PORT_FILE(port);
  free(p);
  free(port->storage_as.port.p);
}


PRIMITIVE STk_open_input_virtual(SCM l, int len)
{
  struct virtual_iob *p;
  SCM getc, readyp, eofp, close, z;

  ENTER_PRIMITIVE("open-virtual-input");

  if (len != 4) Serror("bad number of arguments", l);
  getc   = CAR(l);
  readyp = CAR(CDR(l));
  eofp   = CAR(CDR(CDR(l)));
  close  = CAR(CDR(CDR(CDR(l))));

  verify_param(proc_name, getc,   1);
  verify_param(proc_name, readyp, 2);
  verify_param(proc_name, eofp,   3);
  verify_param(proc_name, close,  4);

  p = (struct virtual_iob *) must_malloc(sizeof (struct virtual_iob));
  
  p->signature = VPORT_SIGNATURE;
  p->flag      = READING;
  p->ungetted  = EOF;
  p->getc      = getc;
  p->readyp    = readyp;
  p->eofp      = eofp;
  p->close     = close;
  p->putc      = Ntruth;
  p->puts      = Ntruth;
  p->flush     = Ntruth;

  /* Vport_descr is a short version of a port_descr */
  NEWCELL(z, tc_ivport);
  z->storage_as.port.p   = (struct port_descr *) 
				must_malloc(sizeof(struct vport_descr));
  PORT_UNGETC(z)	 = EOF;
  PORT_FILE(z)           = (FILE *) p;
  PORT_FLAGS(z)	 	 = 0;

  return z;
}

PRIMITIVE STk_open_output_virtual(SCM l, int len)
{
  struct virtual_iob *p;
  SCM putc, puts, flush, close, z;

  ENTER_PRIMITIVE("open-virtual-input");
  
  if (len != 4) Serror("bad number of arguments", l);
  putc   = CAR(l);
  puts   = CAR(CDR(l));
  flush  = CAR(CDR(CDR(l)));
  close  = CAR(CDR(CDR(CDR(l))));

  verify_param(proc_name, putc,  1);
  verify_param(proc_name, puts,  2);
  verify_param(proc_name, flush, 3);
  verify_param(proc_name, close, 4);

  p = (struct virtual_iob *) must_malloc(sizeof (struct virtual_iob));
  
  p->signature = VPORT_SIGNATURE;
  p->flag      = WRITING;
  p->ungetted  = EOF;
  p->getc      = Ntruth;
  p->readyp    = Ntruth;
  p->eofp      = Ntruth;
  p->close     = close;
  p->putc      = putc;
  p->puts      = puts;
  p->flush     = flush;

  /* Vport_descr is a short version of a port_descr */
  NEWCELL(z, tc_ovport);
  z->storage_as.port.p   = (struct port_descr *) 
				must_malloc(sizeof(struct vport_descr));
  PORT_UNGETC(z)	 = EOF;
  PORT_FILE(z)           = (FILE *) p;
  PORT_FLAGS(z)	 	 = 0;

  return z;
}


PRIMITIVE STk_input_virtual_portp(SCM port)
{
  return (IVPORTP(port)) ? Truth: Ntruth;
}

PRIMITIVE STk_output_virtual_portp(SCM port)
{
  return (OVPORTP(port)) ? Truth: Ntruth;
}


