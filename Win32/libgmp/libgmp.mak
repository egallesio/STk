# Microsoft Developer Studio Generated NMAKE File, Based on libgmp.dsp
!IF "$(CFG)" == ""
CFG=libgmp - Win32 Release
!MESSAGE No configuration specified. Defaulting to libgmp - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "libgmp - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libgmp.mak" CFG="libgmp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libgmp - Win32 Release" (based on "Win32 (x86) Static Library")
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

ALL : "..\Win32libs\libgmp.lib" "$(OUTDIR)\libgmp.bsc"

!ELSE 

ALL : "..\Win32libs\libgmp.lib" "$(OUTDIR)\libgmp.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\gmp.obj"
	-@erase "$(INTDIR)\gmp.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\libgmp.bsc"
	-@erase "..\Win32libs\libgmp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /Z7 /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libgmp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\gmp.sbr"

"$(OUTDIR)\libgmp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\Win32libs\libgmp.lib" 
LIB32_OBJS= \
	"$(INTDIR)\gmp.obj"

"..\Win32libs\libgmp.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<


!IF "$(CFG)" == "libgmp - Win32 Release"
SOURCE="..\..\Mp\fgmp-1.0b5\gmp.c"
DEP_CPP_GMP_C=\
	"..\..\Mp\fgmp-1.0b5\gmp.h"\
	

"$(INTDIR)\gmp.obj"	"$(INTDIR)\gmp.sbr" : $(SOURCE) $(DEP_CPP_GMP_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

