#
# Makefile for the Demos directory
#

include ../config.make

DEMODIR=$(libdir)/demos

what:
	@echo "Type make install to install demos"
install:
	-if [ ! -d $(DEMODIR) ] ; then mkdir -p $(DEMODIR); fi
	for i in *.stk *.stklos Widget/*.stklos ; \
	do \
	  j=`basename $$i`; \
	  sed -e 's=/usr/local/lib/stk=$(libdir)=' \
	      -e 's=/usr/local/bin=$(bindir)=' $$i > $(DEMODIR)/$$j;\
	done
	for i in *.stk *.stklos ; \
	do \
	  chmod 0755 $(DEMODIR)/`basename $$i`; \
	done

install.libs:

