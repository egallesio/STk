#
# STk/Lib Makefile
#
#            Author: Erick Gallesio [eg@unice.fr]
#     Creation date:  2-Oct-1995 21:17  
#  Last file update: 24-Jul-1996 17:55

include ../config.make

HLP_DIR=$(libdir)/Help

lib: 

install:
	-if [ ! -d $(HLP_DIR) ] ; then mkdir -p $(HLP_DIR); fi
	chmod 0555 $(HLP_DIR)
	$(CP) *.html $(HLP_DIR)
	chmod 0644 $(HLP_DIR)/*.html
	-if [ ! -d $(HLP_DIR)/Img ] ; then mkdir -p $(HLP_DIR)/Img; fi
	chmod 0555 $(HLP_DIR)/Img
	$(CP) Img/*.gif $(HLP_DIR)/Img
	chmod 0644  $(HLP_DIR)/Img/*.gif

install.libs:

clean:
	rm -f index.html

very-clean: clean



