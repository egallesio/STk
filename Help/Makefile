#
# STk/Lib Makefile
#
#            Author: Erick Gallesio [eg@unice.fr]
#     Creation date:  2-Oct-1995 21:17  
#  Last file update: 30-Oct-1996 12:17

include ../config.make

HLP_DIR=$(libdir)/Help

lib: 

install:
	-if [ ! -d $(HLP_DIR) ] ; then mkdir -p $(HLP_DIR); fi
	chmod 0755 $(HLP_DIR)
	$(CP) *.html $(HLP_DIR)
	chmod 0644 $(HLP_DIR)/*.html
	-if [ ! -d $(HLP_DIR)/Img ] ; then mkdir -p $(HLP_DIR)/Img; fi
	chmod 0755 $(HLP_DIR)/Img
	$(CP) Img/*.gif $(HLP_DIR)/Img
	chmod 0644  $(HLP_DIR)/Img/*.gif
	../Src/test-stk -no -f make-link.stk $(HLP_DIR)

install.libs:

clean:
	rm -f index.html

very-clean: clean



