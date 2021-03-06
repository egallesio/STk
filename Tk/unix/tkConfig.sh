# tkConfig.sh --
# 
# This shell script (for sh) is generated automatically by Tk's
# configure script.  It will create shell variables for most of
# the configuration options discovered by the configure script.
# This script is intended to be included by the configure scripts
# for Tk extensions so that they don't have to figure this all
# out for themselves.  This file does not duplicate information
# already provided by tclConfig.sh, so you may need to use that
# file in addition to this one.
#
# The information in this file is specific to a single platform.
#
# SCCS: @(#) tkConfig.sh.in 1.8 97/07/07 14:58:48

# Tk's version number.
TK_VERSION='8.0'
TK_MAJOR_VERSION='8'
TK_MINOR_VERSION='0'

# -D flags for use with the C compiler.
TK_DEFS=' -DHAVE_UNISTD_H=1 -DHAVE_LIMITS_H=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TIME_H=1 -DTIME_WITH_SYS_TIME=1 '

# Flag, 1: we built a shared lib, 0 we didn't
TK_SHARED_BUILD=0

# The name of the Tk library (may be either a .a file or a shared library):
TK_LIB_FILE=libtk

# Additional libraries to use when linking Tk.
TK_LIBS='-L/usr/X11R6/lib -lX11   -lm'

# Top-level directory in which Tcl's platform-independent files are
# installed.
TK_PREFIX='/usr/local'

# Top-level directory in which Tcl's platform-specific files (e.g.
# executables) are installed.
TK_EXEC_PREFIX='/usr/local'

# -I switch(es) to use to make all of the X11 include files accessible:
TK_XINCLUDES='-I/usr/X11R6/include'

# Linker switch(es) to use to link with the X11 library archive.
TK_XLIBSW='-L/usr/X11R6/lib -lX11'

# String to pass to linker to pick up the Tk library from its
# build directory.
TK_BUILD_LIB_SPEC='-L/users/eg/STk/STk/Tk/unix -ltk80'

# String to pass to linker to pick up the Tk library from its
# installed directory.
TK_LIB_SPEC='-L/usr/local/lib -ltk80'

# Location of the top-level source directory from which Tk was built.
# This is the directory that contains a README file as well as
# subdirectories such as generic, unix, etc.  If Tk was compiled in a
# different place than the directory containing the source files, this
# points to the location of the sources, not the location where Tk was
# compiled.
TK_SRC_DIR='/users/eg/STk/STk/Tk'
