#
# STk/Lib Makefile
#
#            Author: Erick Gallesio [eg@unice.fr]
#     Creation date: ???
#  Last file update: 13-Aug-1996 18:12


include ../config.make

SITE_SCHEME=$(stkdir)/site-scheme
STK_IMAGES=$(libdir)/images
STK_STK=$(libdir)/STk

lib: 

install:
	-if [ ! -d $(SITE_SCHEME) ] ; then mkdir -p $(SITE_SCHEME); fi
	-if [ ! -d $(STK_IMAGES) ] ; then mkdir -p $(STK_IMAGES); fi
	-if [ ! -d $(STK_STK) ] ; then mkdir -p $(STK_STK); fi
	for i in *.stk prolog.ps STk.init; \
	do \
	  sed -e 's=/usr/local/lib/stk=$(libdir)=' \
	      -e 's=/usr/local/bin=$(bindir)=' $$i > $(STK_STK)/$$i ;\
	  chmod 0644 $(STK_STK)/$$i;\
	done
	$(CP) images/* $(STK_IMAGES)
	chmod 0644 $(STK_IMAGES)/*

install.libs:

clean:

