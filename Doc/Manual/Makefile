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
# Last file update: 30-Oct-1996 12:00


include ../../config.make

MAN1_DIR=$(mandir)/man1
MANN_DIR=$(mandir)/mann


all: ps txt

dvi:

ps: stk.ps stk.txt man-pages.ps

txt: stk.txt

#
# Manual pages
#
man-pages.ps: 
	groff -man *.n > man-pages.ps

#
# STk man page 
#
stk.ps: stk.1
	groff -man stk.1 > stk.ps

stk.txt: stk.1
	nroff -man stk.1 > stk.txt

#
# install.man
#

install.man:
	-if [ ! -d $(mandir) ] ; then mkdir -p $(mandir); fi
	rm -f $(stkdir)/man
	(cd $(stkdir) ; ln -s $(VERSION)/man ./man)
	-if [ ! -d $(MAN1_DIR) ] ; then mkdir -p $(MAN1_DIR); fi
	rm -f $(MAN1_DIR)/*.n $(MAN1_DIR)/stk.1 $(MAN1_DIR)/snow.1
	$(CP) STk-man.macros $(MAN1_DIR)
	$(CP) stk.1 $(MAN1_DIR)/stk.1
	ln $(MAN1_DIR)/stk.1 $(MAN1_DIR)/snow.1
	chmod 444 $(MAN1_DIR)/*
	-if [ ! -d $(MANN_DIR) ] ; then mkdir -p $(MANN_DIR); fi
	$(CP) STk-man.macros $(MANN_DIR)
	for  i in *.n ;do \
	  $(CP) $$i $(MANN_DIR)/stk_$$i; \
	done
	../../Src/test-stk -no -file make-link $(MANN_DIR)

clean:
	rm -f *~

very-clean: clean
	rm -f *.ps
	