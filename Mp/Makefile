#
# Makefile for the multiple precision number library
#

include ../config.make

GMP_VERSION=  gmp-1.3.2
FGMP_VERSION= fgmp-1.0b5


what:
	@echo "you must specify the library you want to make"
libgmp.a: 
	rm -f libgmp.a gmp.h
	(cd $(GMP_VERSION); $(MAKE) CC="$(CC)" CFLAGS="$(STKCFLAGS) $(ALLOC)")
	ln -s $(GMP_VERSION)/libgmp.a .
	ln -s $(GMP_VERSION)/gmp.h .

libfgmp.a:
	rm -f libfgmp.a gmp.h
	(cd $(FGMP_VERSION); $(MAKE) CC="$(CC)" CFLAGS="$(STKCFLAGS) $(ALLOC)")
	ln -s $(FGMP_VERSION)/libfgmp.a .
	ln -s $(GMP_VERSION)/gmp.h .

install: 

install.libs: install
	-if [ ! -d $(execdir) ] ; then mkdir -p $(execdir); fi
	-if [ -f libfgmp.a ] ; then \
	   $(CP) libfgmp.a $(execdir); \
	   $(RANLIB) $(execdir)/libfgmp.a; \
	 fi
	-if [ -f libgmp.a ]  ; then \
	   $(CP) libgmp.a  $(execdir); \
	   $(RANLIB) $(execdir)/libgmp.a; \
	fi
	-if [ ! -d $(incdir) ] ; then mkdir -p $(incdir); fi
	$(CP) gmp.h $(incdir)


clean:
	rm -f libgmp.a libfgmp.a gmp.h *~ core 
	(cd $(GMP_VERSION);  $(MAKE) clean)
	(cd $(FGMP_VERSION); $(MAKE) clean)
