/**
 *	Process.h	- LISP implementation of UNIX processes
 *
 *	Copyright (c) 1994 by Gregory Nickonov.
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
 *            Author: Gregory Nickonov, [gn@jet.msk.su]
 *    Creation date: 15-Jan-1994 11:40
 * Last file update: 11-Jun-1994 22:07
 *
 *            Made working by David Tolpin Dvd@CIM.Msk.SU
 *            Port to Dynaload STk2.1b,
 *            synchronization and registration
 *            mechanism  by  A.Taranov tay@jet.msk.su
 *            
 */
 
#ifndef	__PROCESS_H__
#define	__PROCESS_H__

PRIMITIVE	processp( SCM process );
PRIMITIVE	process_alivep( SCM process );

PRIMITIVE	run_process( SCM command, SCM args, SCM redirection );
PRIMITIVE	run_sync( SCM command, SCM args, SCM redirection );
PRIMITIVE	process_kill( SCM process );

PRIMITIVE	process_input( SCM process );
PRIMITIVE	process_output( SCM process );
PRIMITIVE	process_error( SCM process );

PRIMITIVE	process_input_info( SCM process );
PRIMITIVE	process_output_info( SCM process );
PRIMITIVE	process_error_info( SCM process );

PRIMITIVE	process_command( SCM process );
PRIMITIVE	process_pid( SCM process );

#endif	/* __PROCESS_H__ */
