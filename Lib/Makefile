#
# STk/Lib Makefile
#
#            Author: Erick Gallesio [eg@unice.fr]
#     Creation date: ???
#  Last file update: 28-Oct-1997 21:23


include ../config.make

SITE_SCHEME=$(stkdir)/site-scheme
STK_IMAGES=$(libdir)/Images
STK_STK=$(libdir)/STk
STK_STK_MATCH=$(STK_STK)/Match
lib: 

install:
	-if [ ! -d $(SITE_SCHEME) ] ; then mkdir -p $(SITE_SCHEME); fi
	-if [ ! -d $(STK_IMAGES) ] ; then mkdir -p $(STK_IMAGES); fi
	-if [ ! -d $(STK_STK) ] ; then mkdir -p $(STK_STK); fi
	-if [ ! -d $(STK_STK_MATCH) ] ; then mkdir -p $(STK_STK_MATCH); fi
	for i in *.stk *.stklos prolog.ps STk.init; \
	do \
	  sed -e 's=/usr/local/lib/stk=$(libdir)=' \
	      -e 's=/usr/local/bin=$(bindir)=' $$i > $(STK_STK)/$$i ;\
	  chmod 0644 $(STK_STK)/$$i;\
	done
	$(CP) Images/* $(STK_IMAGES)
	chmod 0644 $(STK_IMAGES)/*
	$(CP) Match/*.scm $(STK_STK_MATCH)
	chmod 0644 $(STK_STK_MATCH)/*

install.libs:

clean:

