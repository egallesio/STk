#
# Makefile for STklos 
#
# Copyright © 1993-1996 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
# 
# Permission to use, copy, and/or distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that both the above copyright notice and this permission notice appear in
# all copies and derived works.  Fees for distribution or use of this
# software or derived works may only be charged with express written
# permission of the copyright holder.  
# This software is provided ``as is'' without express or implied warranty.
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 29-Jun-1993 13:58
# Last file update: 24-Jul-1996 16:35
#

include ../config.make

LIB_STKLOS = $(libdir)/STk

stklos: 
	@echo "**** make STklos completed"

install: 
	-if [ ! -d $(LIB_STKLOS) ] ; then mkdir -p $(LIB_STKLOS); fi
	$(CP) stklos.stk describe.stk $(LIB_STKLOS)
	$(CP) Tk/*.stklos Tk/Composite/*.stklos $(LIB_STKLOS)

install.libs:

clean:
	@/bin/rm -f *~ core stklos.so
