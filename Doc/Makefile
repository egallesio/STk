#
# Documentation Makefile
# 
# Copyright © 1994-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
#    Creation date: 21-Oct-1994 11:25
# Last file update:  3-Sep-1999 21:22 (eg)

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

