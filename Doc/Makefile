#
# Documentation Makefile
# 
# Copyright (C) 1993,1994,1995 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
#    Creation date: 21-Oct-1994 11:25
# Last file update: 20-Aug-1997 14:00

include ../config.make

MAN1_DIR=$(mandir)/man1
MANN_DIR=$(mandir)/mann


DIRS=Extension Reference STklos+Tk Manual Isotas96

install: install.man

all:	dvi ps

dvi:
	for i in $(DIRS) ;do \
	  (cd $$i; $(MAKE) dvi); \
	done

ps:	
	for i in $(DIRS) ;do \
	  (cd $$i; $(MAKE) ps); \
	done

install.man:
	(cd Manual; make install.man)

clean:
	for i in $(DIRS) ;do \
	  (cd $$i; $(MAKE) clean); \
	done

very-clean:
	for i in $(DIRS) ;do \
	  (cd $$i; $(MAKE) very-clean); \
	done

