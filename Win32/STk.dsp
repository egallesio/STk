# Microsoft Developer Studio Project File - Name="STk" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=STk - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "STk.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "STk.mak" CFG="STk - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "STk - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\Mp\fgmp-1.0b5" /I "..\Tcl" /I "..\Src" /I "..\Tk\generic" /I "..\Tk\xlib" /I "..\Tk\win" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_TK" /D "STk_CODE" /D "USE_STKLOS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libtcl.lib libtk.lib libgmp.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"Win32libs"
# Begin Target

# Name "STk - Win32 Release"
# Begin Source File

SOURCE=..\Src\address.c
# End Source File
# Begin Source File

SOURCE=..\Src\argv.c
# End Source File
# Begin Source File

SOURCE=..\Extensions\base64.c
# End Source File
# Begin Source File

SOURCE=..\Src\boolean.c
# End Source File
# Begin Source File

SOURCE=..\Src\char.c
# End Source File
# Begin Source File

SOURCE=..\Src\console.c
# End Source File
# Begin Source File

SOURCE=..\Src\cont.c
# End Source File
# Begin Source File

SOURCE=..\Src\dummy.c
# End Source File
# Begin Source File

SOURCE=..\Src\dump.c
# End Source File
# Begin Source File

SOURCE=..\Src\dynload.c
# End Source File
# Begin Source File

SOURCE=..\Src\env.c
# End Source File
# Begin Source File

SOURCE=..\Src\error.c
# End Source File
# Begin Source File

SOURCE=..\Src\eval.c
# End Source File
# Begin Source File

SOURCE=..\Src\extend.c
# End Source File
# Begin Source File

SOURCE=..\Src\extend.h
# End Source File
# Begin Source File

SOURCE=..\Src\gc.c
# End Source File
# Begin Source File

SOURCE=..\Src\gc.h
# End Source File
# Begin Source File

SOURCE=..\Extensions\hash.c
# End Source File
# Begin Source File

SOURCE=..\Extensions\html.c
# End Source File
# Begin Source File

SOURCE=..\Src\io.c
# End Source File
# Begin Source File

SOURCE=..\Src\keyword.c
# End Source File
# Begin Source File

SOURCE=..\Src\list.c
# End Source File
# Begin Source File

SOURCE=..\Extensions\locale.c
# End Source File
# Begin Source File

SOURCE=..\Src\macros.c
# End Source File
# Begin Source File

SOURCE=..\Src\module.c
# End Source File
# Begin Source File

SOURCE=..\Src\module.h
# End Source File
# Begin Source File

SOURCE=..\Src\number.c
# End Source File
# Begin Source File

SOURCE=..\Extensions\pixmap.c
# End Source File
# Begin Source File

SOURCE=..\Src\port.c
# End Source File
# Begin Source File

SOURCE=..\Src\primitives.c
# End Source File
# Begin Source File

SOURCE=..\Src\print.c
# End Source File
# Begin Source File

SOURCE=..\Src\proc.c
# End Source File
# Begin Source File

SOURCE=..\Src\promise.c
# End Source File
# Begin Source File

SOURCE=..\Src\read.c
# End Source File
# Begin Source File

SOURCE=..\Src\signal.c
# End Source File
# Begin Source File

SOURCE=..\Src\slib.c
# End Source File
# Begin Source File

SOURCE=..\Src\sport.c
# End Source File
# Begin Source File

SOURCE=..\Src\sport.h
# End Source File
# Begin Source File

SOURCE=..\Extensions\sregexp.c
# End Source File
# Begin Source File

SOURCE=..\Src\stk.h
# End Source File
# Begin Source File

SOURCE=..\Tk\win\rc\stk.rc
# End Source File
# Begin Source File

SOURCE=..\Src\stklos.c
# End Source File
# Begin Source File

SOURCE=..\Src\stklos.h
# End Source File
# Begin Source File

SOURCE=..\Src\str.c
# End Source File
# Begin Source File

SOURCE=..\Src\symbol.c
# End Source File
# Begin Source File

SOURCE=..\Src\syntax.c
# End Source File
# Begin Source File

SOURCE="..\Src\tcl-glue.c"
# End Source File
# Begin Source File

SOURCE="..\Src\tcl-glue.h"
# End Source File
# Begin Source File

SOURCE="..\Src\tcl-lib.c"
# End Source File
# Begin Source File

SOURCE="..\Src\tcl-obj.c"
# End Source File
# Begin Source File

SOURCE="..\Src\tcl-util.c"
# End Source File
# Begin Source File

SOURCE="..\Src\tk-glue.c"
# End Source File
# Begin Source File

SOURCE="..\Src\tk-glue.h"
# End Source File
# Begin Source File

SOURCE="..\Src\tk-main.c"
# End Source File
# Begin Source File

SOURCE="..\Src\tk-util.c"
# End Source File
# Begin Source File

SOURCE=..\Src\toplevel.c
# End Source File
# Begin Source File

SOURCE=..\Src\trace.c
# End Source File
# Begin Source File

SOURCE=..\Src\unix.c
# End Source File
# Begin Source File

SOURCE=..\Src\userinit.c
# End Source File
# Begin Source File

SOURCE=..\Src\vector.c
# End Source File
# Begin Source File

SOURCE=..\Src\vport.c
# End Source File
# Begin Source File

SOURCE=..\Src\vport.h
# End Source File
# Begin Source File

SOURCE=..\Src\wstk.c
# End Source File
# End Target
# End Project
