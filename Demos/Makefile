#
# Makefile for the Demos directory
#
#  Copyright © 1993-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
# Last file update:  3-Sep-1999 19:20 (eg)
#

include ../config.make

DEMODIR=$(root)$(libdir)/Demos

what:
	@echo "Type make install to install demos"
install:
	-if [ ! -d $(DEMODIR) ] ; then mkdir -p $(DEMODIR); fi
	chmod 0755 $(DEMODIR)
	for i in *.stk *.stklos Widget/*.stklos *.html; \
	do \
	  j=`basename $$i`; \
	  sed -e 's=/usr/local/lib/stk=$(libdir)=' \
	      -e 's=/usr/local/bin=$(bindir)=' $$i > $(DEMODIR)/$$j;\
	done
	for i in *.stk *.stklos *.html; \
	do \
	  chmod 0755 $(DEMODIR)/`basename $$i`; \
	done
	mkdir $(DEMODIR)/Html-Demos; chmod 0755 $(DEMODIR)/Html-Demos 
	cp Html-Demos/*.html $(DEMODIR)/Html-Demos
	chmod 0644  $(DEMODIR)/Html-Demos/*.html
	mkdir $(DEMODIR)/Html-Demos/Images; chmod 0755 $(DEMODIR)/Html-Demos/Images
	cp Html-Demos/Images/*.gif  $(DEMODIR)/Html-Demos/Images
	chmod 0644  $(DEMODIR)/Html-Demos/Images/*
	(cd $(DEMODIR)/Html-Demos; ln -s ../amib.stklos ../stklos-widgets .)

install.libs:

