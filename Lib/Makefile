#
# STk/Lib Makefile
#
#            Author: Erick Gallesio [eg@unice.fr]
#     Creation date: ???
#  Last file update: 27-Sep-1998 17:37


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
	for i in *.stk *.stklos STk.init; \
	do \
	  sed -e 's=/usr/local/lib/stk=$(libdir)=' \
	      -e 's=/usr/local/bin=$(bindir)=' $$i > $(STK_STK)/$$i ;\
	  chmod 0644 $(STK_STK)/$$i;\
	done
	$(CP) Images/* $(STK_IMAGES)
	chmod 0644 $(STK_IMAGES)/*
	$(CP) Match/*.scm $(STK_STK_MATCH)
	chmod 0644 $(STK_STK_MATCH)/*
	chmod 0755 $(SITE_SCHEME) $(STK_IMAGES) $(STK_STK) $(STK_STK_MATCH)
install.libs:

clean:

