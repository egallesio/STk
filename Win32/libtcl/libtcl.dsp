# Microsoft Developer Studio Project File - Name="libtcl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtcl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtcl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtcl.mak" CFG="libtcl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtcl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\..\Mp\fgmp-1.0b5" /I "..\..\Src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STk_CODE" /FR /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Win32libs\libtcl.lib"
# Begin Target

# Name "libtcl - Win32 Release"
# Begin Source File

SOURCE=..\..\Tcl\panic.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\regexp.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tcl.h
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclAsync.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclEvent.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclGet.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclHash.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclInt.h
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclNotify.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclPort.h
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclPreserve.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclRegexp.h
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclTimer.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclUtil.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclWinNotify.c
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclWinPort.h
# End Source File
# Begin Source File

SOURCE=..\..\Tcl\tclWinTime.c
# End Source File
# End Target
# End Project
