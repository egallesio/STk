# tclConfig.sh --
# 
# This shell script (for sh) is generated automatically by Tcl's
# configure script.  It will create shell variables for most of
# the configuration options discovered by the configure script.
# This script is intended to be included by the configure scripts
# for Tcl extensions so that they don't have to figure this all
# out for themselves.
#
# The information in this file is specific to a single platform.
#
# SCCS: @(#) tclConfig.sh.in 1.13 96/04/11 09:50:14

# Tcl's version number.
TCL_VERSION='7.5'
TCL_MAJOR_VERSION='7'
TCL_MINOR_VERSION='5'

# C compiler to use for compilation.
TCL_CC='gcc'

# -D flags for use with the C compiler.
TCL_DEFS=' -DHAVE_UNISTD_H=1 -DHAVE_SYS_TIME_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_TZNAME=1 -DHAVE_TIMEZONE_VAR=1 -DSTDC_HEADERS=1 -Dvfork=fork '

# The name of the Tcl library (may be either a .a file or a shared library):
TCL_LIB_FILE=libtcl7.5.a

# Additional libraries to use when linking Tcl.
TCL_LIBS='-ldl  -lieee -lm'

# Top-level directory in which Tcl's platform-independent files are
# installed.
TCL_PREFIX='/usr/local'

# Top-level directory in which Tcl's platform-specific files (e.g.
# executables) are installed.
TCL_EXEC_PREFIX='/usr/local'

# Flags to pass to cc when compiling the components of a shared library:
TCL_SHLIB_CFLAGS='-fPIC'

# Base command to use for combining object files into a shared library:
TCL_SHLIB_LD='gcc -shared'

# Either '$LIBS' (if dependent libraries should be included when linking
# shared libraries) or an empty string.  See Tcl's configure.in for more
# explanation.
TCL_SHLIB_LD_LIBS=''

# Suffix to use for the name of a shared library.
TCL_SHLIB_SUFFIX='.so'

# Library file(s) to include in tclsh and other base applications
# in order to provide facilities needed by DLOBJ above.
TCL_DL_LIBS='-ldl'

# Flags to pass to the compiler when linking object files into
# an executable tclsh or tcltest binary.
TCL_LD_FLAGS='-rdynamic'

# Flags to pass to ld, such as "-R /usr/local/tcl/lib", that tell the
# run-time dynamic linker where to look for shared libraries such as
# libtcl.so.  Used when linking applications.  Only works if there
# is a variable "LIB_INSTALL_DIR" defined in the Makefile.
TCL_LD_SEARCH_FLAGS=''

# Additional object files linked with Tcl to provide compatibility
# with standard facilities from ANSI C or POSIX.
TCL_COMPAT_OBJS=''

# Name of the ranlib program to use.
TCL_RANLIB='ranlib'

# String to pass to linker to pick up the Tcl library from its
# build directory.
TCL_LIB_SPEC='-L/usr/local/lib -ltcl7.5'

# Indicates whether a version numbers should be used in -l switches
# (if 1, use switches like -ltcl7.5;  if 0, use switches like -ltcl).
# 0 is required under SunOS 4.1, for example.
TCL_LIB_VERSIONS_OK='ok'

# String that can be evaluated to generate the part of a shared library
# name that comes after the "libxxx" (includes version number, if any,
# extension, and anything else needed).  Depends on the variables
# VERSION and SHLIB_SUFFIX.  On most UNIX systems this is
# ${VERSION}.${SHLIB_SUFFIX}.
TCL_SHARED_LIB_SUFFIX='${VERSION}${SHLIB_SUFFIX}'

# String that can be evaluated to generate the part of an unshared library
# name that comes after the "libxxx" (includes version number, if any,
# extension, and anything else needed).  Depends on the variables
# VERSION.  On most UNIX systems this is ${VERSION}.a.
TCL_UNSHARED_LIB_SUFFIX='${VERSION}.a'
