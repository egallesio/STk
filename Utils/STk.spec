%define		release 1
Summary: 	Scheme Interpreter with access to the Tk toolkit
Name:    	STk
Version: 	4.0.0
Release: 	%{release}
Copyright: 	distributable
Source: 	STk-4.0.0.tar.gz
Group:		Development/Languages
Packager:	Erick Gallesio <eg@unice.fr>
BuildRoot:	/tmp/STk

%package devel
Summary: 	Header files and libraries for STk
Group: 		Development/Libraries
Requires:	STk = 4.0.0

%description
STk is a R4RS Scheme interpreter which can access the Tk graphical
package.  Concretely it can be seen as the John Ousterhout's Tk
package where the Tcl language has been replaced by Scheme.

STk also provides an efficient object oriented system called STklos.
STklos is an extensive OO system with multiple inheritance, generic
functions, multi-methods and a Meta Object Protocol).

This package integrates the 8.0.3 version of the Tk toolkit.


%description devel
This package contains the files necessary for extending, in C, the 
STk interpeter. If you don't need to write some C code for extending
the interpreter, you don't need to install this package.


%ifarch i386
%define machine Linux-2.X-ix86
%else
%ifarch ppc
%define machine Linux-2.X-ppc
%else
%define machine Linux-2.X-ix86
%endif
%endif


%prep

%setup

%build
CFLAGS=-O2 sh configure --prefix=/usr/local
make

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}

make install root=%{buildroot}
make install.libs root=%{buildroot}

%clean

 

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
/usr/local/lib/stk/4.0.0/Demos
/usr/local/lib/stk/4.0.0/Help
/usr/local/lib/stk/4.0.0/Images
/usr/local/lib/stk/4.0.0/%{machine}/stk
/usr/local/lib/stk/4.0.0/%{machine}/snow
/usr/local/lib/stk/4.0.0/%{machine}/*.so


/usr/local/lib/stk/4.0.0/STk
/usr/local/lib/stk/4.0.0/include
/usr/local/lib/stk/4.0.0/man
/usr/local/bin/stk-4.0.0
/usr/local/bin/snow-4.0.0
/usr/local/bin/stk
/usr/local/bin/snow



%files devel
/usr/local/lib/stk/4.0.0/%{machine}/Config
/usr/local/lib/stk/4.0.0/%{machine}/Libs
