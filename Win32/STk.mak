# Microsoft Developer Studio Generated NMAKE File, Based on STk.dsp
!IF "$(CFG)" == ""
CFG=STk - Win32 Release
!MESSAGE No configuration specified. Defaulting to STk - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "STk - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "STk.mak" CFG="STk - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "STk - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\STk.exe" "$(OUTDIR)\STk.bsc"

!ELSE 

ALL : "libtk - Win32 Release" "libtcl - Win32 Release" "libgmp - Win32 Release"\
 "$(OUTDIR)\STk.exe" "$(OUTDIR)\STk.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libgmp - Win32 ReleaseCLEAN" "libtcl - Win32 ReleaseCLEAN"\
 "libtk - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\address.obj"
	-@erase "$(INTDIR)\address.sbr"
	-@erase "$(INTDIR)\argv.obj"
	-@erase "$(INTDIR)\argv.sbr"
	-@erase "$(INTDIR)\base64.obj"
	-@erase "$(INTDIR)\base64.sbr"
	-@erase "$(INTDIR)\boolean.obj"
	-@erase "$(INTDIR)\boolean.sbr"
	-@erase "$(INTDIR)\char.obj"
	-@erase "$(INTDIR)\char.sbr"
	-@erase "$(INTDIR)\console.obj"
	-@erase "$(INTDIR)\console.sbr"
	-@erase "$(INTDIR)\cont.obj"
	-@erase "$(INTDIR)\cont.sbr"
	-@erase "$(INTDIR)\dummy.obj"
	-@erase "$(INTDIR)\dummy.sbr"
	-@erase "$(INTDIR)\dump.obj"
	-@erase "$(INTDIR)\dump.sbr"
	-@erase "$(INTDIR)\dynload.obj"
	-@erase "$(INTDIR)\dynload.sbr"
	-@erase "$(INTDIR)\env.obj"
	-@erase "$(INTDIR)\env.sbr"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\error.sbr"
	-@erase "$(INTDIR)\eval.obj"
	-@erase "$(INTDIR)\eval.sbr"
	-@erase "$(INTDIR)\extend.obj"
	-@erase "$(INTDIR)\extend.sbr"
	-@erase "$(INTDIR)\gc.obj"
	-@erase "$(INTDIR)\gc.sbr"
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\hash.sbr"
	-@erase "$(INTDIR)\html.obj"
	-@erase "$(INTDIR)\html.sbr"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\io.sbr"
	-@erase "$(INTDIR)\keyword.obj"
	-@erase "$(INTDIR)\keyword.sbr"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\list.sbr"
	-@erase "$(INTDIR)\locale.obj"
	-@erase "$(INTDIR)\locale.sbr"
	-@erase "$(INTDIR)\macros.obj"
	-@erase "$(INTDIR)\macros.sbr"
	-@erase "$(INTDIR)\module.obj"
	-@erase "$(INTDIR)\module.sbr"
	-@erase "$(INTDIR)\number.obj"
	-@erase "$(INTDIR)\number.sbr"
	-@erase "$(INTDIR)\pixmap.obj"
	-@erase "$(INTDIR)\pixmap.sbr"
	-@erase "$(INTDIR)\port.obj"
	-@erase "$(INTDIR)\port.sbr"
	-@erase "$(INTDIR)\primitives.obj"
	-@erase "$(INTDIR)\primitives.sbr"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\print.sbr"
	-@erase "$(INTDIR)\proc.obj"
	-@erase "$(INTDIR)\proc.sbr"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\process.sbr"
	-@erase "$(INTDIR)\promise.obj"
	-@erase "$(INTDIR)\promise.sbr"
	-@erase "$(INTDIR)\read.obj"
	-@erase "$(INTDIR)\read.sbr"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\signal.sbr"
	-@erase "$(INTDIR)\slib.obj"
	-@erase "$(INTDIR)\slib.sbr"
	-@erase "$(INTDIR)\socket.obj"
	-@erase "$(INTDIR)\socket.sbr"
	-@erase "$(INTDIR)\sport.obj"
	-@erase "$(INTDIR)\sport.sbr"
	-@erase "$(INTDIR)\sregexp.obj"
	-@erase "$(INTDIR)\sregexp.sbr"
	-@erase "$(INTDIR)\stk.res"
	-@erase "$(INTDIR)\stklos.obj"
	-@erase "$(INTDIR)\stklos.sbr"
	-@erase "$(INTDIR)\str.obj"
	-@erase "$(INTDIR)\str.sbr"
	-@erase "$(INTDIR)\symbol.obj"
	-@erase "$(INTDIR)\symbol.sbr"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\syntax.sbr"
	-@erase "$(INTDIR)\tcl-glue.obj"
	-@erase "$(INTDIR)\tcl-glue.sbr"
	-@erase "$(INTDIR)\tcl-lib.obj"
	-@erase "$(INTDIR)\tcl-lib.sbr"
	-@erase "$(INTDIR)\tcl-obj.obj"
	-@erase "$(INTDIR)\tcl-obj.sbr"
	-@erase "$(INTDIR)\tcl-util.obj"
	-@erase "$(INTDIR)\tcl-util.sbr"
	-@erase "$(INTDIR)\tk-glue.obj"
	-@erase "$(INTDIR)\tk-glue.sbr"
	-@erase "$(INTDIR)\tk-main.obj"
	-@erase "$(INTDIR)\tk-main.sbr"
	-@erase "$(INTDIR)\tk-util.obj"
	-@erase "$(INTDIR)\tk-util.sbr"
	-@erase "$(INTDIR)\toplevel.obj"
	-@erase "$(INTDIR)\toplevel.sbr"
	-@erase "$(INTDIR)\trace.obj"
	-@erase "$(INTDIR)\trace.sbr"
	-@erase "$(INTDIR)\unix.obj"
	-@erase "$(INTDIR)\unix.sbr"
	-@erase "$(INTDIR)\userinit.obj"
	-@erase "$(INTDIR)\userinit.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vector.obj"
	-@erase "$(INTDIR)\vector.sbr"
	-@erase "$(INTDIR)\vport.obj"
	-@erase "$(INTDIR)\vport.sbr"
	-@erase "$(INTDIR)\wstk.obj"
	-@erase "$(INTDIR)\wstk.sbr"
	-@erase "$(OUTDIR)\STk.bsc"
	-@erase "$(OUTDIR)\STk.exe"
	-@erase "$(OUTDIR)\STk.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /Z7 /Od /I "..\Mp\fgmp-1.0b5" /I "..\Tcl" /I\
 "..\Src" /I "..\Tk\generic" /I "..\Tk\xlib" /I "..\Tk\win" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /D "USE_TK" /D "STk_CODE" /D "SCM_CODE" /D "USE_STKLOS"\
 /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\stk.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STk.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\address.sbr" \
	"$(INTDIR)\argv.sbr" \
	"$(INTDIR)\base64.sbr" \
	"$(INTDIR)\boolean.sbr" \
	"$(INTDIR)\char.sbr" \
	"$(INTDIR)\console.sbr" \
	"$(INTDIR)\cont.sbr" \
	"$(INTDIR)\dummy.sbr" \
	"$(INTDIR)\dump.sbr" \
	"$(INTDIR)\dynload.sbr" \
	"$(INTDIR)\env.sbr" \
	"$(INTDIR)\error.sbr" \
	"$(INTDIR)\eval.sbr" \
	"$(INTDIR)\extend.sbr" \
	"$(INTDIR)\gc.sbr" \
	"$(INTDIR)\hash.sbr" \
	"$(INTDIR)\html.sbr" \
	"$(INTDIR)\io.sbr" \
	"$(INTDIR)\keyword.sbr" \
	"$(INTDIR)\list.sbr" \
	"$(INTDIR)\locale.sbr" \
	"$(INTDIR)\macros.sbr" \
	"$(INTDIR)\module.sbr" \
	"$(INTDIR)\number.sbr" \
	"$(INTDIR)\pixmap.sbr" \
	"$(INTDIR)\port.sbr" \
	"$(INTDIR)\primitives.sbr" \
	"$(INTDIR)\print.sbr" \
	"$(INTDIR)\proc.sbr" \
	"$(INTDIR)\process.sbr" \
	"$(INTDIR)\promise.sbr" \
	"$(INTDIR)\read.sbr" \
	"$(INTDIR)\signal.sbr" \
	"$(INTDIR)\slib.sbr" \
	"$(INTDIR)\socket.sbr" \
	"$(INTDIR)\sport.sbr" \
	"$(INTDIR)\sregexp.sbr" \
	"$(INTDIR)\stklos.sbr" \
	"$(INTDIR)\str.sbr" \
	"$(INTDIR)\symbol.sbr" \
	"$(INTDIR)\syntax.sbr" \
	"$(INTDIR)\tcl-glue.sbr" \
	"$(INTDIR)\tcl-lib.sbr" \
	"$(INTDIR)\tcl-obj.sbr" \
	"$(INTDIR)\tcl-util.sbr" \
	"$(INTDIR)\tk-glue.sbr" \
	"$(INTDIR)\tk-main.sbr" \
	"$(INTDIR)\tk-util.sbr" \
	"$(INTDIR)\toplevel.sbr" \
	"$(INTDIR)\trace.sbr" \
	"$(INTDIR)\unix.sbr" \
	"$(INTDIR)\userinit.sbr" \
	"$(INTDIR)\vector.sbr" \
	"$(INTDIR)\vport.sbr" \
	"$(INTDIR)\wstk.sbr"

"$(OUTDIR)\STk.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib Wsock32.lib libtcl.lib libtk.lib libgmp.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\STk.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)\STk.exe" /libpath:"Win32libs" 
LINK32_OBJS= \
	"$(INTDIR)\address.obj" \
	"$(INTDIR)\argv.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\boolean.obj" \
	"$(INTDIR)\char.obj" \
	"$(INTDIR)\console.obj" \
	"$(INTDIR)\cont.obj" \
	"$(INTDIR)\dummy.obj" \
	"$(INTDIR)\dump.obj" \
	"$(INTDIR)\dynload.obj" \
	"$(INTDIR)\env.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\eval.obj" \
	"$(INTDIR)\extend.obj" \
	"$(INTDIR)\gc.obj" \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\html.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\keyword.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\locale.obj" \
	"$(INTDIR)\macros.obj" \
	"$(INTDIR)\module.obj" \
	"$(INTDIR)\number.obj" \
	"$(INTDIR)\pixmap.obj" \
	"$(INTDIR)\port.obj" \
	"$(INTDIR)\primitives.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\proc.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\promise.obj" \
	"$(INTDIR)\read.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\slib.obj" \
	"$(INTDIR)\socket.obj" \
	"$(INTDIR)\sport.obj" \
	"$(INTDIR)\sregexp.obj" \
	"$(INTDIR)\stk.res" \
	"$(INTDIR)\stklos.obj" \
	"$(INTDIR)\str.obj" \
	"$(INTDIR)\symbol.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tcl-glue.obj" \
	"$(INTDIR)\tcl-lib.obj" \
	"$(INTDIR)\tcl-obj.obj" \
	"$(INTDIR)\tcl-util.obj" \
	"$(INTDIR)\tk-glue.obj" \
	"$(INTDIR)\tk-main.obj" \
	"$(INTDIR)\tk-util.obj" \
	"$(INTDIR)\toplevel.obj" \
	"$(INTDIR)\trace.obj" \
	"$(INTDIR)\unix.obj" \
	"$(INTDIR)\userinit.obj" \
	"$(INTDIR)\vector.obj" \
	"$(INTDIR)\vport.obj" \
	"$(INTDIR)\wstk.obj" \
	".\Win32libs\libgmp.lib" \
	".\Win32libs\libtcl.lib" \
	".\Win32libs\libtk.lib"

"$(OUTDIR)\STk.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<


!IF "$(CFG)" == "STk - Win32 Release"

!IF  "$(CFG)" == "STk - Win32 Release"

"libgmp - Win32 Release" : 
   cd ".\libgmp"
   $(MAKE) /$(MAKEFLAGS) /F ".\libgmp.mak" CFG="libgmp - Win32 Release" 
   cd ".."

"libgmp - Win32 ReleaseCLEAN" : 
   cd ".\libgmp"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F ".\libgmp.mak" CFG="libgmp - Win32 Release"\
 RECURSE=1 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "STk - Win32 Release"

"libtcl - Win32 Release" : 
   cd ".\libtcl"
   $(MAKE) /$(MAKEFLAGS) /F ".\libtcl.mak" CFG="libtcl - Win32 Release" 
   cd ".."

"libtcl - Win32 ReleaseCLEAN" : 
   cd ".\libtcl"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F ".\libtcl.mak" CFG="libtcl - Win32 Release"\
 RECURSE=1 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "STk - Win32 Release"

"libtk - Win32 Release" : 
   cd ".\libtk"
   $(MAKE) /$(MAKEFLAGS) /F ".\libtk.mak" CFG="libtk - Win32 Release" 
   cd ".."

"libtk - Win32 ReleaseCLEAN" : 
   cd ".\libtk"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F ".\libtk.mak" CFG="libtk - Win32 Release"\
 RECURSE=1 
   cd ".."

!ENDIF 

SOURCE=..\Src\address.c
DEP_CPP_ADDRE=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\gc.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\address.obj"	"$(INTDIR)\address.sbr" : $(SOURCE) $(DEP_CPP_ADDRE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\argv.c
DEP_CPP_ARGV_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\argv.obj"	"$(INTDIR)\argv.sbr" : $(SOURCE) $(DEP_CPP_ARGV_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Extensions\base64.c
DEP_CPP_BASE6=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\base64.obj"	"$(INTDIR)\base64.sbr" : $(SOURCE) $(DEP_CPP_BASE6)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\boolean.c
DEP_CPP_BOOLE=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\stk.h"\
	"..\Src\stklos.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\boolean.obj"	"$(INTDIR)\boolean.sbr" : $(SOURCE) $(DEP_CPP_BOOLE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\char.c
DEP_CPP_CHAR_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\char.obj"	"$(INTDIR)\char.sbr" : $(SOURCE) $(DEP_CPP_CHAR_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\console.c
DEP_CPP_CONSO=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\console.obj"	"$(INTDIR)\console.sbr" : $(SOURCE) $(DEP_CPP_CONSO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\cont.c
DEP_CPP_CONT_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\gc.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\cont.obj"	"$(INTDIR)\cont.sbr" : $(SOURCE) $(DEP_CPP_CONT_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\dummy.c
DEP_CPP_DUMMY=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\dummy.obj"	"$(INTDIR)\dummy.sbr" : $(SOURCE) $(DEP_CPP_DUMMY)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\dump.c
DEP_CPP_DUMP_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\dump.obj"	"$(INTDIR)\dump.sbr" : $(SOURCE) $(DEP_CPP_DUMP_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\dynload.c
DEP_CPP_DYNLO=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\dynload.obj"	"$(INTDIR)\dynload.sbr" : $(SOURCE) $(DEP_CPP_DYNLO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\env.c
DEP_CPP_ENV_C=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\env.obj"	"$(INTDIR)\env.sbr" : $(SOURCE) $(DEP_CPP_ENV_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\error.c
DEP_CPP_ERROR=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\error.obj"	"$(INTDIR)\error.sbr" : $(SOURCE) $(DEP_CPP_ERROR)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\eval.c
DEP_CPP_EVAL_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\stk.h"\
	"..\Src\stklos.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\eval.obj"	"$(INTDIR)\eval.sbr" : $(SOURCE) $(DEP_CPP_EVAL_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\extend.c
DEP_CPP_EXTEN=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\stk.h"\
	"..\Src\stklos.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\extend.obj"	"$(INTDIR)\extend.sbr" : $(SOURCE) $(DEP_CPP_EXTEN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\gc.c
DEP_CPP_GC_C1c=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\gc.h"\
	"..\Src\stk.h"\
	"..\Src\stklos.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\gc.obj"	"$(INTDIR)\gc.sbr" : $(SOURCE) $(DEP_CPP_GC_C1c) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Extensions\hash.c
DEP_CPP_HASH_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\hash.obj"	"$(INTDIR)\hash.sbr" : $(SOURCE) $(DEP_CPP_HASH_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Extensions\html.c
DEP_CPP_HTML_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\html.obj"	"$(INTDIR)\html.sbr" : $(SOURCE) $(DEP_CPP_HTML_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\io.c
DEP_CPP_IO_C22=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\sport.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Src\vport.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\io.obj"	"$(INTDIR)\io.sbr" : $(SOURCE) $(DEP_CPP_IO_C22) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\keyword.c
DEP_CPP_KEYWO=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\keyword.obj"	"$(INTDIR)\keyword.sbr" : $(SOURCE) $(DEP_CPP_KEYWO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\list.c
DEP_CPP_LIST_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\list.obj"	"$(INTDIR)\list.sbr" : $(SOURCE) $(DEP_CPP_LIST_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Extensions\locale.c
DEP_CPP_LOCAL=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\locale.obj"	"$(INTDIR)\locale.sbr" : $(SOURCE) $(DEP_CPP_LOCAL)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\macros.c
DEP_CPP_MACRO=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\macros.obj"	"$(INTDIR)\macros.sbr" : $(SOURCE) $(DEP_CPP_MACRO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\module.c
DEP_CPP_MODUL=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\module.obj"	"$(INTDIR)\module.sbr" : $(SOURCE) $(DEP_CPP_MODUL)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\number.c
DEP_CPP_NUMBE=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\number.obj"	"$(INTDIR)\number.sbr" : $(SOURCE) $(DEP_CPP_NUMBE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Extensions\pixmap.c
DEP_CPP_PIXMA=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\Tk\generic\tkInt.h"\
	"..\Tk\generic\tkPort.h"\
	"..\Tk\win\tkWinPort.h"\
	"..\tk\xlib\x11\cursorfont.h"\
	"..\tk\xlib\x11\keysym.h"\
	"..\tk\xlib\x11\keysymdef.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xatom.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	"..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\pixmap.obj"	"$(INTDIR)\pixmap.sbr" : $(SOURCE) $(DEP_CPP_PIXMA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\port.c
DEP_CPP_PORT_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\port.obj"	"$(INTDIR)\port.sbr" : $(SOURCE) $(DEP_CPP_PORT_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\primitives.c
DEP_CPP_PRIMI=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\primitives.obj"	"$(INTDIR)\primitives.sbr" : $(SOURCE)\
 $(DEP_CPP_PRIMI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\print.c
DEP_CPP_PRINT=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\Src\stklos.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\print.obj"	"$(INTDIR)\print.sbr" : $(SOURCE) $(DEP_CPP_PRINT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\proc.c
DEP_CPP_PROC_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\stk.h"\
	"..\Src\stklos.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\proc.obj"	"$(INTDIR)\proc.sbr" : $(SOURCE) $(DEP_CPP_PROC_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Extensions\process.c
DEP_CPP_PROCE=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\process.obj"	"$(INTDIR)\process.sbr" : $(SOURCE) $(DEP_CPP_PROCE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\promise.c
DEP_CPP_PROMI=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\promise.obj"	"$(INTDIR)\promise.sbr" : $(SOURCE) $(DEP_CPP_PROMI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\read.c
DEP_CPP_READ_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\read.obj"	"$(INTDIR)\read.sbr" : $(SOURCE) $(DEP_CPP_READ_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\signal.c
DEP_CPP_SIGNA=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\signal.obj"	"$(INTDIR)\signal.sbr" : $(SOURCE) $(DEP_CPP_SIGNA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\slib.c
DEP_CPP_SLIB_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\gc.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tcl\tclWinPort.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\slib.obj"	"$(INTDIR)\slib.sbr" : $(SOURCE) $(DEP_CPP_SLIB_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Extensions\socket.c
DEP_CPP_SOCKE=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\socket.obj"	"$(INTDIR)\socket.sbr" : $(SOURCE) $(DEP_CPP_SOCKE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\sport.c
DEP_CPP_SPORT=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\sport.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\sport.obj"	"$(INTDIR)\sport.sbr" : $(SOURCE) $(DEP_CPP_SPORT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Extensions\sregexp.c
DEP_CPP_SREGE=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\sregexp.obj"	"$(INTDIR)\sregexp.sbr" : $(SOURCE) $(DEP_CPP_SREGE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Tk\win\rc\stk.rc
DEP_RSC_STK_R=\
	"..\Tk\generic\tk.h"\
	"..\Tk\win\rc\buttons.bmp"\
	"..\Tk\win\rc\cursor00.cur"\
	"..\Tk\win\rc\cursor02.cur"\
	"..\Tk\win\rc\cursor04.cur"\
	"..\Tk\win\rc\cursor06.cur"\
	"..\Tk\win\rc\cursor08.cur"\
	"..\Tk\win\rc\cursor0a.cur"\
	"..\Tk\win\rc\cursor0c.cur"\
	"..\Tk\win\rc\cursor0e.cur"\
	"..\Tk\win\rc\cursor10.cur"\
	"..\Tk\win\rc\cursor12.cur"\
	"..\Tk\win\rc\cursor14.cur"\
	"..\Tk\win\rc\cursor16.cur"\
	"..\Tk\win\rc\cursor18.cur"\
	"..\Tk\win\rc\cursor1a.cur"\
	"..\Tk\win\rc\cursor1c.cur"\
	"..\Tk\win\rc\cursor1e.cur"\
	"..\Tk\win\rc\cursor20.cur"\
	"..\Tk\win\rc\cursor22.cur"\
	"..\Tk\win\rc\cursor24.cur"\
	"..\Tk\win\rc\cursor26.cur"\
	"..\Tk\win\rc\cursor28.cur"\
	"..\Tk\win\rc\cursor2a.cur"\
	"..\Tk\win\rc\cursor2c.cur"\
	"..\Tk\win\rc\cursor2e.cur"\
	"..\Tk\win\rc\cursor30.cur"\
	"..\Tk\win\rc\cursor32.cur"\
	"..\Tk\win\rc\cursor34.cur"\
	"..\Tk\win\rc\cursor36.cur"\
	"..\Tk\win\rc\cursor38.cur"\
	"..\Tk\win\rc\cursor3a.cur"\
	"..\Tk\win\rc\cursor3c.cur"\
	"..\Tk\win\rc\cursor3e.cur"\
	"..\Tk\win\rc\cursor42.cur"\
	"..\Tk\win\rc\cursor44.cur"\
	"..\Tk\win\rc\cursor46.cur"\
	"..\Tk\win\rc\cursor48.cur"\
	"..\Tk\win\rc\cursor4a.cur"\
	"..\Tk\win\rc\cursor4c.cur"\
	"..\Tk\win\rc\cursor4e.cur"\
	"..\Tk\win\rc\cursor50.cur"\
	"..\Tk\win\rc\cursor52.cur"\
	"..\Tk\win\rc\cursor54.cur"\
	"..\Tk\win\rc\cursor56.cur"\
	"..\Tk\win\rc\cursor58.cur"\
	"..\Tk\win\rc\cursor5a.cur"\
	"..\Tk\win\rc\cursor5c.cur"\
	"..\Tk\win\rc\cursor5e.cur"\
	"..\Tk\win\rc\cursor60.cur"\
	"..\Tk\win\rc\cursor62.cur"\
	"..\Tk\win\rc\cursor64.cur"\
	"..\Tk\win\rc\cursor66.cur"\
	"..\Tk\win\rc\cursor68.cur"\
	"..\Tk\win\rc\cursor6a.cur"\
	"..\Tk\win\rc\cursor6c.cur"\
	"..\Tk\win\rc\cursor6e.cur"\
	"..\Tk\win\rc\cursor70.cur"\
	"..\Tk\win\rc\cursor72.cur"\
	"..\Tk\win\rc\cursor74.cur"\
	"..\Tk\win\rc\cursor76.cur"\
	"..\Tk\win\rc\cursor78.cur"\
	"..\Tk\win\rc\cursor7a.cur"\
	"..\Tk\win\rc\cursor7c.cur"\
	"..\Tk\win\rc\cursor7e.cur"\
	"..\Tk\win\rc\cursor80.cur"\
	"..\Tk\win\rc\cursor82.cur"\
	"..\Tk\win\rc\cursor84.cur"\
	"..\Tk\win\rc\cursor86.cur"\
	"..\Tk\win\rc\cursor88.cur"\
	"..\Tk\win\rc\cursor8a.cur"\
	"..\Tk\win\rc\cursor8c.cur"\
	"..\Tk\win\rc\cursor8e.cur"\
	"..\Tk\win\rc\cursor90.cur"\
	"..\Tk\win\rc\cursor92.cur"\
	"..\Tk\win\rc\cursor94.cur"\
	"..\Tk\win\rc\cursor96.cur"\
	"..\Tk\win\rc\cursor98.cur"\
	"..\Tk\win\rc\STk.ico"\
	

"$(INTDIR)\stk.res" : $(SOURCE) $(DEP_RSC_STK_R) "$(INTDIR)"
	$(RSC) /l 0x40c /fo"$(INTDIR)\stk.res" /i "\Export\STk-4.0.0\Tk\win\rc" /d\
 "NDEBUG" $(SOURCE)


SOURCE=..\Src\stklos.c
DEP_CPP_STKLO=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\Src\stklos.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\stklos.obj"	"$(INTDIR)\stklos.sbr" : $(SOURCE) $(DEP_CPP_STKLO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\str.c
DEP_CPP_STR_C=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\str.obj"	"$(INTDIR)\str.sbr" : $(SOURCE) $(DEP_CPP_STR_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\symbol.c
DEP_CPP_SYMBO=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\symbol.obj"	"$(INTDIR)\symbol.sbr" : $(SOURCE) $(DEP_CPP_SYMBO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\syntax.c
DEP_CPP_SYNTA=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\syntax.obj"	"$(INTDIR)\syntax.sbr" : $(SOURCE) $(DEP_CPP_SYNTA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\Src\tcl-glue.c"
DEP_CPP_TCL_G=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\extend.h"\
	"..\Src\gc.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\tcl-glue.obj"	"$(INTDIR)\tcl-glue.sbr" : $(SOURCE) $(DEP_CPP_TCL_G)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\Src\tcl-lib.c"
DEP_CPP_TCL_L=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\gc.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\tcl-lib.obj"	"$(INTDIR)\tcl-lib.sbr" : $(SOURCE) $(DEP_CPP_TCL_L)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\Src\tcl-obj.c"
DEP_CPP_TCL_O=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\tcl-obj.obj"	"$(INTDIR)\tcl-obj.sbr" : $(SOURCE) $(DEP_CPP_TCL_O)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\Src\tcl-util.c"
DEP_CPP_TCL_U=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\tcl-util.obj"	"$(INTDIR)\tcl-util.sbr" : $(SOURCE) $(DEP_CPP_TCL_U)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\Src\tk-glue.c"
DEP_CPP_TK_GL=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\gc.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\Tk\generic\tkInt.h"\
	"..\Tk\generic\tkPort.h"\
	"..\Tk\win\tkWinPort.h"\
	"..\tk\xlib\x11\cursorfont.h"\
	"..\tk\xlib\x11\keysym.h"\
	"..\tk\xlib\x11\keysymdef.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xatom.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	"..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tk-glue.obj"	"$(INTDIR)\tk-glue.sbr" : $(SOURCE) $(DEP_CPP_TK_GL)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\Src\tk-main.c"
DEP_CPP_TK_MA=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\Tk\generic\tkInt.h"\
	"..\Tk\generic\tkPort.h"\
	"..\Tk\win\tkWinPort.h"\
	"..\tk\xlib\x11\cursorfont.h"\
	"..\tk\xlib\x11\keysym.h"\
	"..\tk\xlib\x11\keysymdef.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xatom.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	"..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tk-main.obj"	"$(INTDIR)\tk-main.sbr" : $(SOURCE) $(DEP_CPP_TK_MA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\Src\tk-util.c"
DEP_CPP_TK_UT=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\tk-util.obj"	"$(INTDIR)\tk-util.sbr" : $(SOURCE) $(DEP_CPP_TK_UT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\toplevel.c
DEP_CPP_TOPLE=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\gc.h"\
	"..\Src\module.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\toplevel.obj"	"$(INTDIR)\toplevel.sbr" : $(SOURCE) $(DEP_CPP_TOPLE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\trace.c
DEP_CPP_TRACE=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\trace.obj"	"$(INTDIR)\trace.sbr" : $(SOURCE) $(DEP_CPP_TRACE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\unix.c
DEP_CPP_UNIX_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\unix.obj"	"$(INTDIR)\unix.sbr" : $(SOURCE) $(DEP_CPP_UNIX_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\userinit.c
DEP_CPP_USERI=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\userinit.obj"	"$(INTDIR)\userinit.sbr" : $(SOURCE) $(DEP_CPP_USERI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\vector.c
DEP_CPP_VECTO=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\vector.obj"	"$(INTDIR)\vector.sbr" : $(SOURCE) $(DEP_CPP_VECTO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\vport.c
DEP_CPP_VPORT=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Src\vport.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\vport.obj"	"$(INTDIR)\vport.sbr" : $(SOURCE) $(DEP_CPP_VPORT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Src\wstk.c
DEP_CPP_WSTK_=\
	"..\Mp\fgmp-1.0b5\gmp.h"\
	"..\Src\stk.h"\
	"..\src\stkvers.h"\
	"..\Src\tcl-glue.h"\
	"..\Src\tk-glue.h"\
	"..\Tcl\tcl.h"\
	"..\Tcl\tclInt.h"\
	"..\Tcl\tclRegexp.h"\
	"..\Tk\generic\tk.h"\
	"..\Tk\generic\tkInt.h"\
	"..\Tk\generic\tkPort.h"\
	"..\Tk\win\tkWin.h"\
	"..\Tk\win\tkWinInt.h"\
	"..\Tk\win\tkWinPort.h"\
	"..\tk\xlib\x11\cursorfont.h"\
	"..\tk\xlib\x11\keysym.h"\
	"..\tk\xlib\x11\keysymdef.h"\
	"..\tk\xlib\x11\x.h"\
	"..\tk\xlib\x11\xatom.h"\
	"..\tk\xlib\x11\xfuncproto.h"\
	"..\tk\xlib\x11\xlib.h"\
	"..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\wstk.obj"	"$(INTDIR)\wstk.sbr" : $(SOURCE) $(DEP_CPP_WSTK_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

