/*
 * v p o r t . c			-- Virtual ports management
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
 *
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 26-Oct-1998 15:55
 * Last file update:  3-Sep-1999 21:02 (eg)
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

  ENTER_PRIMITIVE("open-input-virtual");

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

  ENTER_PRIMITIVE("open-output-virtual");
  
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


