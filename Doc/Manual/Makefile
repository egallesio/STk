###
### Makefile for the man pages
###


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

clean:
	rm -f *~

very-clean: clean
	rm -f *.ps
	