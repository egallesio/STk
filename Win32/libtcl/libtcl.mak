# Microsoft Developer Studio Generated NMAKE File, Based on libtcl.dsp
!IF "$(CFG)" == ""
CFG=libtcl - Win32 Release
!MESSAGE No configuration specified. Defaulting to libtcl - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "libtcl - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtcl.mak" CFG="libtcl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtcl - Win32 Release" (based on "Win32 (x86) Static Library")
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

ALL : "..\Win32libs\libtcl.lib" "$(OUTDIR)\libtcl.bsc"

!ELSE 

ALL : "..\Win32libs\libtcl.lib" "$(OUTDIR)\libtcl.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\panic.obj"
	-@erase "$(INTDIR)\panic.sbr"
	-@erase "$(INTDIR)\regexp.obj"
	-@erase "$(INTDIR)\regexp.sbr"
	-@erase "$(INTDIR)\tclAsync.obj"
	-@erase "$(INTDIR)\tclAsync.sbr"
	-@erase "$(INTDIR)\tclEvent.obj"
	-@erase "$(INTDIR)\tclEvent.sbr"
	-@erase "$(INTDIR)\tclGet.obj"
	-@erase "$(INTDIR)\tclGet.sbr"
	-@erase "$(INTDIR)\tclHash.obj"
	-@erase "$(INTDIR)\tclHash.sbr"
	-@erase "$(INTDIR)\tclNotify.obj"
	-@erase "$(INTDIR)\tclNotify.sbr"
	-@erase "$(INTDIR)\tclPreserve.obj"
	-@erase "$(INTDIR)\tclPreserve.sbr"
	-@erase "$(INTDIR)\tclTimer.obj"
	-@erase "$(INTDIR)\tclTimer.sbr"
	-@erase "$(INTDIR)\tclUtil.obj"
	-@erase "$(INTDIR)\tclUtil.sbr"
	-@erase "$(INTDIR)\tclWinNotify.obj"
	-@erase "$(INTDIR)\tclWinNotify.sbr"
	-@erase "$(INTDIR)\tclWinTime.obj"
	-@erase "$(INTDIR)\tclWinTime.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\libtcl.bsc"
	-@erase "..\Win32libs\libtcl.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /Z7 /Od /I "..\..\Mp\fgmp-1.0b5" /I "..\..\Src" /I\
 "..\..\Tcl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STk_CODE" /D "SCM_CODE"\
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

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libtcl.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\panic.sbr" \
	"$(INTDIR)\regexp.sbr" \
	"$(INTDIR)\tclAsync.sbr" \
	"$(INTDIR)\tclEvent.sbr" \
	"$(INTDIR)\tclGet.sbr" \
	"$(INTDIR)\tclHash.sbr" \
	"$(INTDIR)\tclNotify.sbr" \
	"$(INTDIR)\tclPreserve.sbr" \
	"$(INTDIR)\tclTimer.sbr" \
	"$(INTDIR)\tclUtil.sbr" \
	"$(INTDIR)\tclWinNotify.sbr" \
	"$(INTDIR)\tclWinTime.sbr"

"$(OUTDIR)\libtcl.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\Win32libs\libtcl.lib" 
LIB32_OBJS= \
	"$(INTDIR)\panic.obj" \
	"$(INTDIR)\regexp.obj" \
	"$(INTDIR)\tclAsync.obj" \
	"$(INTDIR)\tclEvent.obj" \
	"$(INTDIR)\tclGet.obj" \
	"$(INTDIR)\tclHash.obj" \
	"$(INTDIR)\tclNotify.obj" \
	"$(INTDIR)\tclPreserve.obj" \
	"$(INTDIR)\tclTimer.obj" \
	"$(INTDIR)\tclUtil.obj" \
	"$(INTDIR)\tclWinNotify.obj" \
	"$(INTDIR)\tclWinTime.obj"

"..\Win32libs\libtcl.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<


!IF "$(CFG)" == "libtcl - Win32 Release"
SOURCE=..\..\Tcl\panic.c
DEP_CPP_PANIC=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	

"$(INTDIR)\panic.obj"	"$(INTDIR)\panic.sbr" : $(SOURCE) $(DEP_CPP_PANIC)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\regexp.c
DEP_CPP_REGEX=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclPort.h"\
	"..\..\Tcl\tclRegexp.h"\
	"..\..\Tcl\tclWinPort.h"\
	

"$(INTDIR)\regexp.obj"	"$(INTDIR)\regexp.sbr" : $(SOURCE) $(DEP_CPP_REGEX)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclAsync.c
DEP_CPP_TCLAS=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclRegexp.h"\
	

"$(INTDIR)\tclAsync.obj"	"$(INTDIR)\tclAsync.sbr" : $(SOURCE) $(DEP_CPP_TCLAS)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclEvent.c
DEP_CPP_TCLEV=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclPort.h"\
	"..\..\Tcl\tclRegexp.h"\
	"..\..\Tcl\tclWinPort.h"\
	

"$(INTDIR)\tclEvent.obj"	"$(INTDIR)\tclEvent.sbr" : $(SOURCE) $(DEP_CPP_TCLEV)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclGet.c
DEP_CPP_TCLGE=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclPort.h"\
	"..\..\Tcl\tclRegexp.h"\
	"..\..\Tcl\tclWinPort.h"\
	

"$(INTDIR)\tclGet.obj"	"$(INTDIR)\tclGet.sbr" : $(SOURCE) $(DEP_CPP_TCLGE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclHash.c
DEP_CPP_TCLHA=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclRegexp.h"\
	

"$(INTDIR)\tclHash.obj"	"$(INTDIR)\tclHash.sbr" : $(SOURCE) $(DEP_CPP_TCLHA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclNotify.c
DEP_CPP_TCLNO=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclPort.h"\
	"..\..\Tcl\tclRegexp.h"\
	"..\..\Tcl\tclWinPort.h"\
	

"$(INTDIR)\tclNotify.obj"	"$(INTDIR)\tclNotify.sbr" : $(SOURCE)\
 $(DEP_CPP_TCLNO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclPreserve.c
DEP_CPP_TCLPR=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclRegexp.h"\
	

"$(INTDIR)\tclPreserve.obj"	"$(INTDIR)\tclPreserve.sbr" : $(SOURCE)\
 $(DEP_CPP_TCLPR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclTimer.c
DEP_CPP_TCLTI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclPort.h"\
	"..\..\Tcl\tclRegexp.h"\
	"..\..\Tcl\tclWinPort.h"\
	

"$(INTDIR)\tclTimer.obj"	"$(INTDIR)\tclTimer.sbr" : $(SOURCE) $(DEP_CPP_TCLTI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclUtil.c
DEP_CPP_TCLUT=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclPort.h"\
	"..\..\Tcl\tclRegexp.h"\
	"..\..\Tcl\tclWinPort.h"\
	

"$(INTDIR)\tclUtil.obj"	"$(INTDIR)\tclUtil.sbr" : $(SOURCE) $(DEP_CPP_TCLUT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclWinNotify.c
DEP_CPP_TCLWI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclPort.h"\
	"..\..\Tcl\tclRegexp.h"\
	"..\..\Tcl\tclWinPort.h"\
	

"$(INTDIR)\tclWinNotify.obj"	"$(INTDIR)\tclWinNotify.sbr" : $(SOURCE)\
 $(DEP_CPP_TCLWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tcl\tclWinTime.c
DEP_CPP_TCLWIN=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tcl\tclInt.h"\
	"..\..\Tcl\tclPort.h"\
	"..\..\Tcl\tclRegexp.h"\
	"..\..\Tcl\tclWinPort.h"\
	

"$(INTDIR)\tclWinTime.obj"	"$(INTDIR)\tclWinTime.sbr" : $(SOURCE)\
 $(DEP_CPP_TCLWIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

