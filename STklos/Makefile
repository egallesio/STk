#
# Makefile for STklos 
#
# Copyright © 1993-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
# 
# Permission to use, copy, modify, distribute,and license this
# software and its documentation for any purpose is hereby granted,
# provided that existing copyright notices are retained in all
# copies and that this notice is included verbatim in any
# distributions.  No written agreement, license, or royalty fee is
# required for any of the authorized uses.
# This software is provided ``AS IS'' without express or implied
# warranty.
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 29-Jun-1993 13:58
# Last file update:  3-Sep-1999 20:04 (eg)
#

include ../config.make

LIB_STKLOS = $(libdir)/STk

stklos: 
	@echo "**** make STklos completed"

install: 
	-if [ ! -d $(LIB_STKLOS) ] ; then mkdir -p $(LIB_STKLOS); fi
	$(CP) *.stk *.stklos $(LIB_STKLOS)
	$(CP) Tk/*.stklos Tk/Composite/*.stklos $(LIB_STKLOS)
	chmod 0644 $(LIB_STKLOS)/*.stklos $(LIB_STKLOS)/*.stk

install.libs:

clean:
	@/bin/rm -f *~ core stklos.so
