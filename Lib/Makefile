#
# STk/Lib Makefile
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
#            Author: Erick Gallesio [eg@unice.fr]
#     Creation date: ???
#  Last file update:  3-Sep-1999 20:00 (eg)


include ../config.make

SITE_SCHEME=$(root)$(stkdir)/site-scheme
STK_IMAGES=$(root)$(libdir)/Images
STK_STK=$(root)$(libdir)/STk
STK_STK_MATCH=$(STK_STK)/Match

lib: 

install:
	-if [ ! -d $(SITE_SCHEME) ] ; then mkdir -p $(SITE_SCHEME); fi
	-if [ ! -d $(STK_IMAGES) ] ; then mkdir -p $(STK_IMAGES); fi
	-if [ ! -d $(STK_STK) ] ; then mkdir -p $(STK_STK); fi
	-if [ ! -d $(STK_STK_MATCH) ] ; then mkdir -p $(STK_STK_MATCH); fi
	for i in *.stk *.stklos STk.init; \
	do \
	  sed -e 's=/usr/local/lib/stk=$(libdir)=' \
	      -e 's=/usr/local/bin=$(bindir)=' $$i > $(STK_STK)/$$i ;\
	  chmod 0644 $(STK_STK)/$$i;\
	done
	( cd Images; $(CP) * $(STK_IMAGES) )
	chmod 0644 $(STK_IMAGES)/*
	$(CP) Match/*.scm $(STK_STK_MATCH)
	chmod 0644 $(STK_STK_MATCH)/*
	chmod 0755 $(SITE_SCHEME) $(STK_IMAGES) $(STK_STK) $(STK_STK_MATCH)
install.libs:

clean:

