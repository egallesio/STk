%define		release 1
Summary: 	Scheme Interpreter with access to the Tk toolkit
Name:    	STk
Version: 	3.99.2
Release: 	%{release}
Copyright: 	distributable
Source: 	STk-3.99.2.tar.gz
Group:		Development/Languages
Packager:	Erick Gallesio <eg@unice.fr>

%package devel
Summary: 	Header files and libraries for STk
Group: 		Development/Libraries
Requires:	STk = 3.99.2

%description
STk is a R4RS Scheme interpreter which can access the Tk graphical
package.  Concretely it can be seen as the John Ousterhout's Tk
package where the Tcl language has been replaced by Scheme.

STk also provides an efficient object oriented system called STklos.
STklos is an extensive OO system with multiple inheritance, generic
functions, multi-methods and a Meta Object Protocol).

This package uses the 8.0 version of the Tk package for the graphical part.


%description devel
This package contains the files necessary for extending, in C, the 
STk interpeter. If you don't need to write some C code for extending
the interpreter, you don't need to install this package.



%prep
%setup

%build
CFLAGS=-O2 sh configure --prefix=/usr/local
make

%install 
make install
make install.libs

%post
if [ -d /usr/local/lib/stk/site-scheme ] 
then 
  mkdir -p /usr/local/lib/stk/site-scheme 
fi

%postun
rm -f /usr/local/lib/stk/man
rmdir /usr/local/lib/stk/site-scheme || true
rmdir /usr/local/lib/stk || true

%postun devel
rm -f /usr/local/lib/stk/include


%files
%doc README INSTALL CHANGES ChangeLog
/usr/local/lib/stk/3.99.2/Demos
/usr/local/lib/stk/3.99.2/Help
/usr/local/lib/stk/3.99.2/Images
/usr/local/lib/stk/3.99.2/Linux-2.X-ix86/stk
/usr/local/lib/stk/3.99.2/Linux-2.X-ix86/snow
/usr/local/lib/stk/3.99.2/Linux-2.X-ix86/*.so
/usr/local/lib/stk/3.99.2/STk
/usr/local/lib/stk/3.99.2/include
/usr/local/lib/stk/3.99.2/man
/usr/local/bin/stk-3.99.2
/usr/local/bin/snow-3.99.2
/usr/local/bin/stk
/usr/local/bin/snow



%files devel
/usr/local/lib/stk/3.99.2/Linux-2.X-ix86/Config
/usr/local/lib/stk/3.99.2/Linux-2.X-ix86/Libs
