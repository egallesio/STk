# Microsoft Developer Studio Project File - Name="libtk" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtk - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtk.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtk.mak" CFG="libtk - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtk - Win32 Release" (based on "Win32 (x86) Static Library")
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
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\..\Mp\fgmp-1.0b5" /I "..\..\Src" /I "..\..\Tcl" /I "..\..\Tk\xlib" /I "..\..\Tk\generic" /I "..\..\Tk\win" /I "..\..\Tk\bitmaps" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STk_CODE" /D "SCM_CODE" /FR /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Win32libs\libtk.lib"
# Begin Target

# Name "libtk - Win32 Release"
# Begin Source File

SOURCE=..\..\Tk\generic\default.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\ks_names.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\stubs.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tk.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tk3d.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tk3d.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkArgv.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkAtom.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkBind.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkBitmap.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkButton.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkButton.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvArc.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvas.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvBmap.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvImg.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvLine.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvPoly.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvPs.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvText.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvUtil.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCanvWind.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkClipboard.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCmds.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkColor.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkColor.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkConfig.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkCursor.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkEntry.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkError.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkEvent.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkFileFilter.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkFileFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkFocus.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkFont.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkFont.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkFrame.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkGC.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkGeometry.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkGet.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkGrab.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkGrid.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkImage.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkImgBmap.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkImgGIF.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkImgPhoto.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkImgPPM.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkImgUtil.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkInitScript.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkInt.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkListbox.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkMacWinMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkMenubutton.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkMenubutton.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkMenuDraw.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkMessage.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkOption.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkPack.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkPlace.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkPointer.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkPort.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkRectOval.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkScale.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkScale.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkScrollbar.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkScrollbar.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkSelect.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkSelect.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkSquare.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkText.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkText.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkTextBTree.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkTextDisp.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkTextImage.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkTextIndex.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkTextMark.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkTextTag.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkTextWind.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkTrig.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\unix\tkUnixMenubu.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\unix\tkUnixScale.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkUtil.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkVisual.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWin.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWin32Dll.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWin3d.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinButton.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinClipboard.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinColor.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinCursor.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinDefault.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinDialog.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\generic\tkWindow.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinDraw.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinEmbed.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinFont.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinImage.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinInit.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinInt.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinKey.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinMenu.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinPixmap.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinPointer.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinPort.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinRegion.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinScrlbr.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinSend.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinWindow.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinWm.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\win\tkWinX.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\xlib\xbytes.h
# End Source File
# Begin Source File

SOURCE=..\..\Tk\xlib\xcolors.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\xlib\xdraw.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\xlib\xgc.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\xlib\ximage.c
# End Source File
# Begin Source File

SOURCE=..\..\Tk\xlib\xutil.c
# End Source File
# End Target
# End Project
