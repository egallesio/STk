# Generated automatically from Makefile.in by configure.
#
# Makefile for STk
# 
# Copyright © 1993-1996 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
# 
# Permission to use, copy, and/or distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that both the above copyright notice and this permission notice appear in
# all copies and derived works.  Fees for distribution or use of this
# software or derived works may only be charged with express written
# permission of the copyright holder.  
# This software is provided ``as is'' without express or implied warranty.
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: ??-Sep-1993 ??:??
# Last file update:  6-Sep-1996 18:06
#

include ../config.make

CFLAGS		= $(STKCFLAGS) $(VERS-OPT) $(MACHINE) $(DYNLOAD) $(TK) $(DFLGS) \
		  $(OPTS)  -DHAVE_UNISTD_H=1 -DHAVE_LIMITS_H=1 -DHAVE_SIGACTION=1 -DHAVE_SELECT=1 -DSTDC_HEADERS=1  \
		  -I. -I../Tk/generic -I../Tk/$(FLAVOR) -I../Tcl -I../Mp \
		   $(XINCLUDES)
#
# Add here your object ('.o') files
#
USER_OBJ	= 

#
# Scheme interpreter objects
#
SOBJ 		= stk.o slib.o primitives.o boolean.o char.o str.o vector.o\
	  	  gc.o port.o  number.o list.o symbol.o\
		  read.o print.o eval.o env.o macros.o syntax.o \
		  cont.o io.o sport.o promise.o error.o proc.o keyword.o \
		  dump.o address.o dynload.o unix.o toplevel.o argv.o \
		  extend.o signal.o trace.o dummy.o \
		  stklos.o

TCLOBJ		= tcl-lib.o tcl-util.o 
TKOBJ		= tk-main.o tk-glue.o tk-util.o 

TKLIB		= ../Tk/$(FLAVOR)/libtk.a
TCLLIB		= ../Tcl/libtcl.a
EVTCLLIB	= ../Tcl/libevtcl.a
MPLIB		= ../Mp/$(MP).a
LIBS    	=  -ldl -lm $(LIB_DLD) $(LIB_MALLOC)

ALLIBS	 = $(MPLIB) $(TKLIB) $(TCLLIB) $(EVTCLLIB)
SALLLIBS = $(MPLIB) $(TCLLIB) 

default: 
	@echo "You must specify 'STk' or 'Snow'"

STk:
	make stk TK="-DUSE_TK"

Snow:
	make snow TK="-DNO_TK"
	
dev: 
	make dev-bin TK="-DUSE_TK"

tags:
	etags *.[ch] ../Tcl/*.[ch] ../Tk/*.[ch]

stk: 	userinit.o libstk.a $(ALLIBS)
	/bin/rm -f stk
	$(CC) $(STKLDFLAGS) $(CFLAGS) -o stk userinit.o libstk.a $(ALLIBS) \
	      $(XLIBSW) $(LIBS)

snow: 	userinit.o libsnow.a $(SALLIBS)
	/bin/rm -f snow
	$(CC) $(STKLDFLAGS) $(CFLAGS) -o snow userinit.o libsnow.a $(SALLLIBS)\
	       $(LIBS) 

dev-bin: userinit.o $(SOBJ) $(USEROBJ) $(TCLOBJ) $(TKOBJ) $(EOBJ) $(ETKOBJ) $(SALLIBS)
	/bin/rm -f stk
	$(CC) $(STKLDFLAGS) $(CFLAGS) -o stk userinit.o $(SOBJ) $(TCLOBJ) \
	      $(TKOBJ) $(EOBJ) $(ETKOBJ) $(ALLIBS) $(XLIBSW) $(LIBS)

libstk.a: $(SOBJ) $(TCLOBJ) $(TCLOBJ) $(TKOBJ) $(ETKOBJ) $(EOBJ)
	/bin/rm -f libstk.a
	ar rc libstk.a $(SOBJ) $(TKOBJ) $(TCLOBJ) $(ETKOBJ) $(EOBJ)
	$(RANLIB) libstk.a

libsnow.a: $(SOBJ) $(TCLOBJ) $(EOBJ)
	/bin/rm -f libsnow.a
	ar rc libsnow.a $(SOBJ) $(TCLOBJ) $(EOBJ)
	$(RANLIB) libsnow.a

etags:
	etags *.[ch] ../Tcl/*.[ch] ../Tk/generic/*.[ch] ../Tk/unix/*.[ch]

$(MPLIB):
	(cd ../Mp; make $(MP).a)

$(TKLIB): 
	(cd ../Tk; make $(TKLIB))

$(TCLLIB):
	(cd ../Tk; make $(TKLIB))

install:
	@echo "You must specify 'install.stk' or "install.snow'"

install.stk: stk
	-if [ ! -d $(bindir) ] ; then mkdir -p $(bindir); fi
	/bin/rm -f $(bindir)/stk $(bindir)/stk-$(VERSION) 
	ln -s $(execdir)/stk $(bindir)/stk-$(VERSION) 
	ln -s $(execdir)/stk $(bindir)/stk 
	-if [ ! -d $(execdir) ] ; then mkdir -p $(execdir); fi
	$(CP) stk $(execdir)
	$(STRIP) $(execdir)/stk
	chmod 0755 $(bindir)/stk-$(VERSION) $(bindir)/stk $(execdir)/stk

install.stk.libs:
	-if [ ! -d $(execdir) ] ; then mkdir -p $(execdir); fi
	$(CP) libstk.a $(execdir)
	$(RANLIB) $(execdir)/libstk.a
	-if [ ! -d $(incdir) ] ; then mkdir -p $(incdir); fi
	$(CP) stk.h $(incdir)

install.snow: snow
	-if [ ! -d $(bindir) ] ; then mkdir -p $(bindir); fi
	/bin/rm -f $(bindir)/snow-$(VERSION) $(bindir)/snow
	ln -s $(execdir)/snow $(bindir)/snow-$(VERSION) 
	ln -s $(execdir)/snow $(bindir)/snow 
	-if [ ! -d $(execdir) ] ; then mkdir -p $(execdir); fi
	$(CP) snow $(execdir)
	$(STRIP) $(execdir)/snow
	chmod 0755 $(bindir)/snow-$(VERSION) $(bindir)/snow $(execdir)/snow

install.snow.libs:
	-if [ ! -d $(execdir) ] ; then mkdir -p $(execdir); fi
	$(CP) libsnow.a $(execdir)
	$(RANLIB) $(execdir)/libsnow.a	
	-if [ ! -d $(incdir) ] ; then mkdir -p $(incdir); fi
	$(CP) stk.h $(incdir)


clean: 
	@/bin/rm -f  core *.o core config.* \
		     stk  libstk.a  stk  run-stk \
		     snow libsnow.a snow run-snow
