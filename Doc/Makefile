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
# Last file update: 24-Jul-1996 19:04

include ../config.make

# Don't use $(mandir) for manual. Always put it in $(prefix)/man/man1
# This allows us to avoid the destruction of true Tk4.0 man pages
# which are quite different.
MAN1_DIR=$(mandir)/man1
MANN_DIR=$(mandir)/mann


DIRS=Extension Reference STklos+Tk Manual Isotas96

install: 

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
	-if [ ! -d $(mandir) ] ; then mkdir -p $(mandir); fi
	rm -f $(stkdir)/man
	(cd $(stkdir) ; ln -s $(VERSION)/man ./man)
	-if [ ! -d $(MAN1_DIR) ] ; then mkdir -p $(MAN1_DIR); fi
	rm -f $(MAN1_DIR)/*.n $(MAN1_DIR)/stk.1 $(MAN1_DIR)/snow.1
	$(CP) Manual/STk-man.macros $(MAN1_DIR)
	$(CP) Manual/stk.1 $(MAN1_DIR)/stk.1
	ln $(MAN1_DIR)/stk.1 $(MAN1_DIR)/snow.1
	chmod 444 $(MAN1_DIR)/*
	-if [ ! -d $(MANN_DIR) ] ; then mkdir -p $(MANN_DIR); fi
	$(CP) Manual/STk-man.macros $(MANN_DIR)
	(cd Manual ;for  i in *.n ;do \
			  $(CP) $$i $(MANN_DIR)/stk_$$i; \
		    done)

clean:
	for i in $(DIRS) ;do \
	  (cd $$i; $(MAKE) clean); \
	done

very-clean:
	for i in $(DIRS) ;do \
	  (cd $$i; $(MAKE) very-clean); \
	done

