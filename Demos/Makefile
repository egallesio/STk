#
# Makefile for the Demos directory
#

include ../config.make

DEMODIR=$(libdir)/Demos

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
	cp Html-Demos/Images/*  $(DEMODIR)/Html-Demos/Images
	chmod 0644  $(DEMODIR)/Html-Demos/Images/*
	(cd $(DEMODIR)/Html-Demos; ln -s ../amib.stklos ../stklos-widgets .)

install.libs:

