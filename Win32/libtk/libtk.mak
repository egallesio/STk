# Microsoft Developer Studio Generated NMAKE File, Based on libtk.dsp
!IF "$(CFG)" == ""
CFG=libtk - Win32 Release
!MESSAGE No configuration specified. Defaulting to libtk - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "libtk - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtk.mak" CFG="libtk - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtk - Win32 Release" (based on "Win32 (x86) Static Library")
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

ALL : "..\Win32libs\libtk.lib" "$(OUTDIR)\libtk.bsc"

!ELSE 

ALL : "..\Win32libs\libtk.lib" "$(OUTDIR)\libtk.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\stubs.obj"
	-@erase "$(INTDIR)\stubs.sbr"
	-@erase "$(INTDIR)\tk3d.obj"
	-@erase "$(INTDIR)\tk3d.sbr"
	-@erase "$(INTDIR)\tkArgv.obj"
	-@erase "$(INTDIR)\tkArgv.sbr"
	-@erase "$(INTDIR)\tkAtom.obj"
	-@erase "$(INTDIR)\tkAtom.sbr"
	-@erase "$(INTDIR)\tkBind.obj"
	-@erase "$(INTDIR)\tkBind.sbr"
	-@erase "$(INTDIR)\tkBitmap.obj"
	-@erase "$(INTDIR)\tkBitmap.sbr"
	-@erase "$(INTDIR)\tkButton.obj"
	-@erase "$(INTDIR)\tkButton.sbr"
	-@erase "$(INTDIR)\tkCanvArc.obj"
	-@erase "$(INTDIR)\tkCanvArc.sbr"
	-@erase "$(INTDIR)\tkCanvas.obj"
	-@erase "$(INTDIR)\tkCanvas.sbr"
	-@erase "$(INTDIR)\tkCanvBmap.obj"
	-@erase "$(INTDIR)\tkCanvBmap.sbr"
	-@erase "$(INTDIR)\tkCanvImg.obj"
	-@erase "$(INTDIR)\tkCanvImg.sbr"
	-@erase "$(INTDIR)\tkCanvLine.obj"
	-@erase "$(INTDIR)\tkCanvLine.sbr"
	-@erase "$(INTDIR)\tkCanvPoly.obj"
	-@erase "$(INTDIR)\tkCanvPoly.sbr"
	-@erase "$(INTDIR)\tkCanvPs.obj"
	-@erase "$(INTDIR)\tkCanvPs.sbr"
	-@erase "$(INTDIR)\tkCanvText.obj"
	-@erase "$(INTDIR)\tkCanvText.sbr"
	-@erase "$(INTDIR)\tkCanvUtil.obj"
	-@erase "$(INTDIR)\tkCanvUtil.sbr"
	-@erase "$(INTDIR)\tkCanvWind.obj"
	-@erase "$(INTDIR)\tkCanvWind.sbr"
	-@erase "$(INTDIR)\tkClipboard.obj"
	-@erase "$(INTDIR)\tkClipboard.sbr"
	-@erase "$(INTDIR)\tkCmds.obj"
	-@erase "$(INTDIR)\tkCmds.sbr"
	-@erase "$(INTDIR)\tkColor.obj"
	-@erase "$(INTDIR)\tkColor.sbr"
	-@erase "$(INTDIR)\tkConfig.obj"
	-@erase "$(INTDIR)\tkConfig.sbr"
	-@erase "$(INTDIR)\tkCursor.obj"
	-@erase "$(INTDIR)\tkCursor.sbr"
	-@erase "$(INTDIR)\tkEntry.obj"
	-@erase "$(INTDIR)\tkEntry.sbr"
	-@erase "$(INTDIR)\tkError.obj"
	-@erase "$(INTDIR)\tkError.sbr"
	-@erase "$(INTDIR)\tkEvent.obj"
	-@erase "$(INTDIR)\tkEvent.sbr"
	-@erase "$(INTDIR)\tkFileFilter.obj"
	-@erase "$(INTDIR)\tkFileFilter.sbr"
	-@erase "$(INTDIR)\tkFocus.obj"
	-@erase "$(INTDIR)\tkFocus.sbr"
	-@erase "$(INTDIR)\tkFont.obj"
	-@erase "$(INTDIR)\tkFont.sbr"
	-@erase "$(INTDIR)\tkFrame.obj"
	-@erase "$(INTDIR)\tkFrame.sbr"
	-@erase "$(INTDIR)\tkGC.obj"
	-@erase "$(INTDIR)\tkGC.sbr"
	-@erase "$(INTDIR)\tkGeometry.obj"
	-@erase "$(INTDIR)\tkGeometry.sbr"
	-@erase "$(INTDIR)\tkGet.obj"
	-@erase "$(INTDIR)\tkGet.sbr"
	-@erase "$(INTDIR)\tkGrab.obj"
	-@erase "$(INTDIR)\tkGrab.sbr"
	-@erase "$(INTDIR)\tkGrid.obj"
	-@erase "$(INTDIR)\tkGrid.sbr"
	-@erase "$(INTDIR)\tkImage.obj"
	-@erase "$(INTDIR)\tkImage.sbr"
	-@erase "$(INTDIR)\tkImgBmap.obj"
	-@erase "$(INTDIR)\tkImgBmap.sbr"
	-@erase "$(INTDIR)\tkImgGIF.obj"
	-@erase "$(INTDIR)\tkImgGIF.sbr"
	-@erase "$(INTDIR)\tkImgPhoto.obj"
	-@erase "$(INTDIR)\tkImgPhoto.sbr"
	-@erase "$(INTDIR)\tkImgPPM.obj"
	-@erase "$(INTDIR)\tkImgPPM.sbr"
	-@erase "$(INTDIR)\tkImgUtil.obj"
	-@erase "$(INTDIR)\tkImgUtil.sbr"
	-@erase "$(INTDIR)\tkListbox.obj"
	-@erase "$(INTDIR)\tkListbox.sbr"
	-@erase "$(INTDIR)\tkMacWinMenu.obj"
	-@erase "$(INTDIR)\tkMacWinMenu.sbr"
	-@erase "$(INTDIR)\tkMenu.obj"
	-@erase "$(INTDIR)\tkMenu.sbr"
	-@erase "$(INTDIR)\tkMenubutton.obj"
	-@erase "$(INTDIR)\tkMenubutton.sbr"
	-@erase "$(INTDIR)\tkMenuDraw.obj"
	-@erase "$(INTDIR)\tkMenuDraw.sbr"
	-@erase "$(INTDIR)\tkMessage.obj"
	-@erase "$(INTDIR)\tkMessage.sbr"
	-@erase "$(INTDIR)\tkOption.obj"
	-@erase "$(INTDIR)\tkOption.sbr"
	-@erase "$(INTDIR)\tkPack.obj"
	-@erase "$(INTDIR)\tkPack.sbr"
	-@erase "$(INTDIR)\tkPlace.obj"
	-@erase "$(INTDIR)\tkPlace.sbr"
	-@erase "$(INTDIR)\tkPointer.obj"
	-@erase "$(INTDIR)\tkPointer.sbr"
	-@erase "$(INTDIR)\tkRectOval.obj"
	-@erase "$(INTDIR)\tkRectOval.sbr"
	-@erase "$(INTDIR)\tkScale.obj"
	-@erase "$(INTDIR)\tkScale.sbr"
	-@erase "$(INTDIR)\tkScrollbar.obj"
	-@erase "$(INTDIR)\tkScrollbar.sbr"
	-@erase "$(INTDIR)\tkSelect.obj"
	-@erase "$(INTDIR)\tkSelect.sbr"
	-@erase "$(INTDIR)\tkSquare.obj"
	-@erase "$(INTDIR)\tkSquare.sbr"
	-@erase "$(INTDIR)\tkText.obj"
	-@erase "$(INTDIR)\tkText.sbr"
	-@erase "$(INTDIR)\tkTextBTree.obj"
	-@erase "$(INTDIR)\tkTextBTree.sbr"
	-@erase "$(INTDIR)\tkTextDisp.obj"
	-@erase "$(INTDIR)\tkTextDisp.sbr"
	-@erase "$(INTDIR)\tkTextImage.obj"
	-@erase "$(INTDIR)\tkTextImage.sbr"
	-@erase "$(INTDIR)\tkTextIndex.obj"
	-@erase "$(INTDIR)\tkTextIndex.sbr"
	-@erase "$(INTDIR)\tkTextMark.obj"
	-@erase "$(INTDIR)\tkTextMark.sbr"
	-@erase "$(INTDIR)\tkTextTag.obj"
	-@erase "$(INTDIR)\tkTextTag.sbr"
	-@erase "$(INTDIR)\tkTextWind.obj"
	-@erase "$(INTDIR)\tkTextWind.sbr"
	-@erase "$(INTDIR)\tkTrig.obj"
	-@erase "$(INTDIR)\tkTrig.sbr"
	-@erase "$(INTDIR)\tkUnixMenubu.obj"
	-@erase "$(INTDIR)\tkUnixMenubu.sbr"
	-@erase "$(INTDIR)\tkUnixScale.obj"
	-@erase "$(INTDIR)\tkUnixScale.sbr"
	-@erase "$(INTDIR)\tkUtil.obj"
	-@erase "$(INTDIR)\tkUtil.sbr"
	-@erase "$(INTDIR)\tkVisual.obj"
	-@erase "$(INTDIR)\tkVisual.sbr"
	-@erase "$(INTDIR)\tkWin32Dll.obj"
	-@erase "$(INTDIR)\tkWin32Dll.sbr"
	-@erase "$(INTDIR)\tkWin3d.obj"
	-@erase "$(INTDIR)\tkWin3d.sbr"
	-@erase "$(INTDIR)\tkWinButton.obj"
	-@erase "$(INTDIR)\tkWinButton.sbr"
	-@erase "$(INTDIR)\tkWinClipboard.obj"
	-@erase "$(INTDIR)\tkWinClipboard.sbr"
	-@erase "$(INTDIR)\tkWinColor.obj"
	-@erase "$(INTDIR)\tkWinColor.sbr"
	-@erase "$(INTDIR)\tkWinCursor.obj"
	-@erase "$(INTDIR)\tkWinCursor.sbr"
	-@erase "$(INTDIR)\tkWinDialog.obj"
	-@erase "$(INTDIR)\tkWinDialog.sbr"
	-@erase "$(INTDIR)\tkWindow.obj"
	-@erase "$(INTDIR)\tkWindow.sbr"
	-@erase "$(INTDIR)\tkWinDraw.obj"
	-@erase "$(INTDIR)\tkWinDraw.sbr"
	-@erase "$(INTDIR)\tkWinEmbed.obj"
	-@erase "$(INTDIR)\tkWinEmbed.sbr"
	-@erase "$(INTDIR)\tkWinFont.obj"
	-@erase "$(INTDIR)\tkWinFont.sbr"
	-@erase "$(INTDIR)\tkWinImage.obj"
	-@erase "$(INTDIR)\tkWinImage.sbr"
	-@erase "$(INTDIR)\tkWinInit.obj"
	-@erase "$(INTDIR)\tkWinInit.sbr"
	-@erase "$(INTDIR)\tkWinKey.obj"
	-@erase "$(INTDIR)\tkWinKey.sbr"
	-@erase "$(INTDIR)\tkWinMenu.obj"
	-@erase "$(INTDIR)\tkWinMenu.sbr"
	-@erase "$(INTDIR)\tkWinPixmap.obj"
	-@erase "$(INTDIR)\tkWinPixmap.sbr"
	-@erase "$(INTDIR)\tkWinPointer.obj"
	-@erase "$(INTDIR)\tkWinPointer.sbr"
	-@erase "$(INTDIR)\tkWinRegion.obj"
	-@erase "$(INTDIR)\tkWinRegion.sbr"
	-@erase "$(INTDIR)\tkWinScrlbr.obj"
	-@erase "$(INTDIR)\tkWinScrlbr.sbr"
	-@erase "$(INTDIR)\tkWinSend.obj"
	-@erase "$(INTDIR)\tkWinSend.sbr"
	-@erase "$(INTDIR)\tkWinWindow.obj"
	-@erase "$(INTDIR)\tkWinWindow.sbr"
	-@erase "$(INTDIR)\tkWinWm.obj"
	-@erase "$(INTDIR)\tkWinWm.sbr"
	-@erase "$(INTDIR)\tkWinX.obj"
	-@erase "$(INTDIR)\tkWinX.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\xcolors.obj"
	-@erase "$(INTDIR)\xcolors.sbr"
	-@erase "$(INTDIR)\xdraw.obj"
	-@erase "$(INTDIR)\xdraw.sbr"
	-@erase "$(INTDIR)\xgc.obj"
	-@erase "$(INTDIR)\xgc.sbr"
	-@erase "$(INTDIR)\ximage.obj"
	-@erase "$(INTDIR)\ximage.sbr"
	-@erase "$(INTDIR)\xutil.obj"
	-@erase "$(INTDIR)\xutil.sbr"
	-@erase "$(OUTDIR)\libtk.bsc"
	-@erase "..\Win32libs\libtk.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /Z7 /Od /I "..\..\Mp\fgmp-1.0b5" /I "..\..\Src" /I\
 "..\..\Tcl" /I "..\..\Tk\xlib" /I "..\..\Tk\generic" /I "..\..\Tk\win" /I\
 "..\..\Tk\bitmaps" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STk_CODE" /D\
 "SCM_CODE" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libtk.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\stubs.sbr" \
	"$(INTDIR)\tk3d.sbr" \
	"$(INTDIR)\tkArgv.sbr" \
	"$(INTDIR)\tkAtom.sbr" \
	"$(INTDIR)\tkBind.sbr" \
	"$(INTDIR)\tkBitmap.sbr" \
	"$(INTDIR)\tkButton.sbr" \
	"$(INTDIR)\tkCanvArc.sbr" \
	"$(INTDIR)\tkCanvas.sbr" \
	"$(INTDIR)\tkCanvBmap.sbr" \
	"$(INTDIR)\tkCanvImg.sbr" \
	"$(INTDIR)\tkCanvLine.sbr" \
	"$(INTDIR)\tkCanvPoly.sbr" \
	"$(INTDIR)\tkCanvPs.sbr" \
	"$(INTDIR)\tkCanvText.sbr" \
	"$(INTDIR)\tkCanvUtil.sbr" \
	"$(INTDIR)\tkCanvWind.sbr" \
	"$(INTDIR)\tkClipboard.sbr" \
	"$(INTDIR)\tkCmds.sbr" \
	"$(INTDIR)\tkColor.sbr" \
	"$(INTDIR)\tkConfig.sbr" \
	"$(INTDIR)\tkCursor.sbr" \
	"$(INTDIR)\tkEntry.sbr" \
	"$(INTDIR)\tkError.sbr" \
	"$(INTDIR)\tkEvent.sbr" \
	"$(INTDIR)\tkFileFilter.sbr" \
	"$(INTDIR)\tkFocus.sbr" \
	"$(INTDIR)\tkFont.sbr" \
	"$(INTDIR)\tkFrame.sbr" \
	"$(INTDIR)\tkGC.sbr" \
	"$(INTDIR)\tkGeometry.sbr" \
	"$(INTDIR)\tkGet.sbr" \
	"$(INTDIR)\tkGrab.sbr" \
	"$(INTDIR)\tkGrid.sbr" \
	"$(INTDIR)\tkImage.sbr" \
	"$(INTDIR)\tkImgBmap.sbr" \
	"$(INTDIR)\tkImgGIF.sbr" \
	"$(INTDIR)\tkImgPhoto.sbr" \
	"$(INTDIR)\tkImgPPM.sbr" \
	"$(INTDIR)\tkImgUtil.sbr" \
	"$(INTDIR)\tkListbox.sbr" \
	"$(INTDIR)\tkMacWinMenu.sbr" \
	"$(INTDIR)\tkMenu.sbr" \
	"$(INTDIR)\tkMenubutton.sbr" \
	"$(INTDIR)\tkMenuDraw.sbr" \
	"$(INTDIR)\tkMessage.sbr" \
	"$(INTDIR)\tkOption.sbr" \
	"$(INTDIR)\tkPack.sbr" \
	"$(INTDIR)\tkPlace.sbr" \
	"$(INTDIR)\tkPointer.sbr" \
	"$(INTDIR)\tkRectOval.sbr" \
	"$(INTDIR)\tkScale.sbr" \
	"$(INTDIR)\tkScrollbar.sbr" \
	"$(INTDIR)\tkSelect.sbr" \
	"$(INTDIR)\tkSquare.sbr" \
	"$(INTDIR)\tkText.sbr" \
	"$(INTDIR)\tkTextBTree.sbr" \
	"$(INTDIR)\tkTextDisp.sbr" \
	"$(INTDIR)\tkTextImage.sbr" \
	"$(INTDIR)\tkTextIndex.sbr" \
	"$(INTDIR)\tkTextMark.sbr" \
	"$(INTDIR)\tkTextTag.sbr" \
	"$(INTDIR)\tkTextWind.sbr" \
	"$(INTDIR)\tkTrig.sbr" \
	"$(INTDIR)\tkUnixMenubu.sbr" \
	"$(INTDIR)\tkUnixScale.sbr" \
	"$(INTDIR)\tkUtil.sbr" \
	"$(INTDIR)\tkVisual.sbr" \
	"$(INTDIR)\tkWin32Dll.sbr" \
	"$(INTDIR)\tkWin3d.sbr" \
	"$(INTDIR)\tkWinButton.sbr" \
	"$(INTDIR)\tkWinClipboard.sbr" \
	"$(INTDIR)\tkWinColor.sbr" \
	"$(INTDIR)\tkWinCursor.sbr" \
	"$(INTDIR)\tkWinDialog.sbr" \
	"$(INTDIR)\tkWindow.sbr" \
	"$(INTDIR)\tkWinDraw.sbr" \
	"$(INTDIR)\tkWinEmbed.sbr" \
	"$(INTDIR)\tkWinFont.sbr" \
	"$(INTDIR)\tkWinImage.sbr" \
	"$(INTDIR)\tkWinInit.sbr" \
	"$(INTDIR)\tkWinKey.sbr" \
	"$(INTDIR)\tkWinMenu.sbr" \
	"$(INTDIR)\tkWinPixmap.sbr" \
	"$(INTDIR)\tkWinPointer.sbr" \
	"$(INTDIR)\tkWinRegion.sbr" \
	"$(INTDIR)\tkWinScrlbr.sbr" \
	"$(INTDIR)\tkWinSend.sbr" \
	"$(INTDIR)\tkWinWindow.sbr" \
	"$(INTDIR)\tkWinWm.sbr" \
	"$(INTDIR)\tkWinX.sbr" \
	"$(INTDIR)\xcolors.sbr" \
	"$(INTDIR)\xdraw.sbr" \
	"$(INTDIR)\xgc.sbr" \
	"$(INTDIR)\ximage.sbr" \
	"$(INTDIR)\xutil.sbr"

"$(OUTDIR)\libtk.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\Win32libs\libtk.lib" 
LIB32_OBJS= \
	"$(INTDIR)\stubs.obj" \
	"$(INTDIR)\tk3d.obj" \
	"$(INTDIR)\tkArgv.obj" \
	"$(INTDIR)\tkAtom.obj" \
	"$(INTDIR)\tkBind.obj" \
	"$(INTDIR)\tkBitmap.obj" \
	"$(INTDIR)\tkButton.obj" \
	"$(INTDIR)\tkCanvArc.obj" \
	"$(INTDIR)\tkCanvas.obj" \
	"$(INTDIR)\tkCanvBmap.obj" \
	"$(INTDIR)\tkCanvImg.obj" \
	"$(INTDIR)\tkCanvLine.obj" \
	"$(INTDIR)\tkCanvPoly.obj" \
	"$(INTDIR)\tkCanvPs.obj" \
	"$(INTDIR)\tkCanvText.obj" \
	"$(INTDIR)\tkCanvUtil.obj" \
	"$(INTDIR)\tkCanvWind.obj" \
	"$(INTDIR)\tkClipboard.obj" \
	"$(INTDIR)\tkCmds.obj" \
	"$(INTDIR)\tkColor.obj" \
	"$(INTDIR)\tkConfig.obj" \
	"$(INTDIR)\tkCursor.obj" \
	"$(INTDIR)\tkEntry.obj" \
	"$(INTDIR)\tkError.obj" \
	"$(INTDIR)\tkEvent.obj" \
	"$(INTDIR)\tkFileFilter.obj" \
	"$(INTDIR)\tkFocus.obj" \
	"$(INTDIR)\tkFont.obj" \
	"$(INTDIR)\tkFrame.obj" \
	"$(INTDIR)\tkGC.obj" \
	"$(INTDIR)\tkGeometry.obj" \
	"$(INTDIR)\tkGet.obj" \
	"$(INTDIR)\tkGrab.obj" \
	"$(INTDIR)\tkGrid.obj" \
	"$(INTDIR)\tkImage.obj" \
	"$(INTDIR)\tkImgBmap.obj" \
	"$(INTDIR)\tkImgGIF.obj" \
	"$(INTDIR)\tkImgPhoto.obj" \
	"$(INTDIR)\tkImgPPM.obj" \
	"$(INTDIR)\tkImgUtil.obj" \
	"$(INTDIR)\tkListbox.obj" \
	"$(INTDIR)\tkMacWinMenu.obj" \
	"$(INTDIR)\tkMenu.obj" \
	"$(INTDIR)\tkMenubutton.obj" \
	"$(INTDIR)\tkMenuDraw.obj" \
	"$(INTDIR)\tkMessage.obj" \
	"$(INTDIR)\tkOption.obj" \
	"$(INTDIR)\tkPack.obj" \
	"$(INTDIR)\tkPlace.obj" \
	"$(INTDIR)\tkPointer.obj" \
	"$(INTDIR)\tkRectOval.obj" \
	"$(INTDIR)\tkScale.obj" \
	"$(INTDIR)\tkScrollbar.obj" \
	"$(INTDIR)\tkSelect.obj" \
	"$(INTDIR)\tkSquare.obj" \
	"$(INTDIR)\tkText.obj" \
	"$(INTDIR)\tkTextBTree.obj" \
	"$(INTDIR)\tkTextDisp.obj" \
	"$(INTDIR)\tkTextImage.obj" \
	"$(INTDIR)\tkTextIndex.obj" \
	"$(INTDIR)\tkTextMark.obj" \
	"$(INTDIR)\tkTextTag.obj" \
	"$(INTDIR)\tkTextWind.obj" \
	"$(INTDIR)\tkTrig.obj" \
	"$(INTDIR)\tkUnixMenubu.obj" \
	"$(INTDIR)\tkUnixScale.obj" \
	"$(INTDIR)\tkUtil.obj" \
	"$(INTDIR)\tkVisual.obj" \
	"$(INTDIR)\tkWin32Dll.obj" \
	"$(INTDIR)\tkWin3d.obj" \
	"$(INTDIR)\tkWinButton.obj" \
	"$(INTDIR)\tkWinClipboard.obj" \
	"$(INTDIR)\tkWinColor.obj" \
	"$(INTDIR)\tkWinCursor.obj" \
	"$(INTDIR)\tkWinDialog.obj" \
	"$(INTDIR)\tkWindow.obj" \
	"$(INTDIR)\tkWinDraw.obj" \
	"$(INTDIR)\tkWinEmbed.obj" \
	"$(INTDIR)\tkWinFont.obj" \
	"$(INTDIR)\tkWinImage.obj" \
	"$(INTDIR)\tkWinInit.obj" \
	"$(INTDIR)\tkWinKey.obj" \
	"$(INTDIR)\tkWinMenu.obj" \
	"$(INTDIR)\tkWinPixmap.obj" \
	"$(INTDIR)\tkWinPointer.obj" \
	"$(INTDIR)\tkWinRegion.obj" \
	"$(INTDIR)\tkWinScrlbr.obj" \
	"$(INTDIR)\tkWinSend.obj" \
	"$(INTDIR)\tkWinWindow.obj" \
	"$(INTDIR)\tkWinWm.obj" \
	"$(INTDIR)\tkWinX.obj" \
	"$(INTDIR)\xcolors.obj" \
	"$(INTDIR)\xdraw.obj" \
	"$(INTDIR)\xgc.obj" \
	"$(INTDIR)\ximage.obj" \
	"$(INTDIR)\xutil.obj"

"..\Win32libs\libtk.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<


!IF "$(CFG)" == "libtk - Win32 Release"
SOURCE=..\..\Tk\win\stubs.c
DEP_CPP_STUBS=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\stubs.obj"	"$(INTDIR)\stubs.sbr" : $(SOURCE) $(DEP_CPP_STUBS)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tk3d.c
DEP_CPP_TK3D_=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tk3d.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tk3d.obj"	"$(INTDIR)\tk3d.sbr" : $(SOURCE) $(DEP_CPP_TK3D_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkArgv.c
DEP_CPP_TKARG=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkArgv.obj"	"$(INTDIR)\tkArgv.sbr" : $(SOURCE) $(DEP_CPP_TKARG)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkAtom.c
DEP_CPP_TKATO=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkAtom.obj"	"$(INTDIR)\tkAtom.sbr" : $(SOURCE) $(DEP_CPP_TKATO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkBind.c
DEP_CPP_TKBIN=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\ks_names.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkBind.obj"	"$(INTDIR)\tkBind.sbr" : $(SOURCE) $(DEP_CPP_TKBIN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkBitmap.c
DEP_CPP_TKBIT=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\tk\bitmaps\error.bmp"\
	"..\..\tk\bitmaps\gray12.bmp"\
	"..\..\tk\bitmaps\gray25.bmp"\
	"..\..\tk\bitmaps\gray50.bmp"\
	"..\..\tk\bitmaps\gray75.bmp"\
	"..\..\tk\bitmaps\hourglass.bmp"\
	"..\..\tk\bitmaps\info.bmp"\
	"..\..\tk\bitmaps\questhead.bmp"\
	"..\..\tk\bitmaps\question.bmp"\
	"..\..\tk\bitmaps\warning.bmp"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkBitmap.obj"	"$(INTDIR)\tkBitmap.sbr" : $(SOURCE) $(DEP_CPP_TKBIT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkButton.c
DEP_CPP_TKBUT=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkButton.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkButton.obj"	"$(INTDIR)\tkButton.sbr" : $(SOURCE) $(DEP_CPP_TKBUT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvArc.c
DEP_CPP_TKCAN=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvArc.obj"	"$(INTDIR)\tkCanvArc.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCAN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvas.c
DEP_CPP_TKCANV=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkCanvas.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvas.obj"	"$(INTDIR)\tkCanvas.sbr" : $(SOURCE) $(DEP_CPP_TKCANV)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvBmap.c
DEP_CPP_TKCANVB=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkCanvas.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvBmap.obj"	"$(INTDIR)\tkCanvBmap.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCANVB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvImg.c
DEP_CPP_TKCANVI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkCanvas.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvImg.obj"	"$(INTDIR)\tkCanvImg.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCANVI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvLine.c
DEP_CPP_TKCANVL=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvLine.obj"	"$(INTDIR)\tkCanvLine.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCANVL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvPoly.c
DEP_CPP_TKCANVP=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvPoly.obj"	"$(INTDIR)\tkCanvPoly.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCANVP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvPs.c
DEP_CPP_TKCANVPS=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkCanvas.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvPs.obj"	"$(INTDIR)\tkCanvPs.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCANVPS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvText.c
DEP_CPP_TKCANVT=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkCanvas.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvText.obj"	"$(INTDIR)\tkCanvText.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCANVT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvUtil.c
DEP_CPP_TKCANVU=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkCanvas.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvUtil.obj"	"$(INTDIR)\tkCanvUtil.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCANVU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCanvWind.c
DEP_CPP_TKCANVW=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkCanvas.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCanvWind.obj"	"$(INTDIR)\tkCanvWind.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCANVW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkClipboard.c
DEP_CPP_TKCLI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkSelect.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkClipboard.obj"	"$(INTDIR)\tkClipboard.sbr" : $(SOURCE)\
 $(DEP_CPP_TKCLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCmds.c
DEP_CPP_TKCMD=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCmds.obj"	"$(INTDIR)\tkCmds.sbr" : $(SOURCE) $(DEP_CPP_TKCMD)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkColor.c
DEP_CPP_TKCOL=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkColor.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkColor.obj"	"$(INTDIR)\tkColor.sbr" : $(SOURCE) $(DEP_CPP_TKCOL)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkConfig.c
DEP_CPP_TKCON=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkConfig.obj"	"$(INTDIR)\tkConfig.sbr" : $(SOURCE) $(DEP_CPP_TKCON)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkCursor.c
DEP_CPP_TKCUR=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkCursor.obj"	"$(INTDIR)\tkCursor.sbr" : $(SOURCE) $(DEP_CPP_TKCUR)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkEntry.c
DEP_CPP_TKENT=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkEntry.obj"	"$(INTDIR)\tkEntry.sbr" : $(SOURCE) $(DEP_CPP_TKENT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkError.c
DEP_CPP_TKERR=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkError.obj"	"$(INTDIR)\tkError.sbr" : $(SOURCE) $(DEP_CPP_TKERR)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkEvent.c
DEP_CPP_TKEVE=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkEvent.obj"	"$(INTDIR)\tkEvent.sbr" : $(SOURCE) $(DEP_CPP_TKEVE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkFileFilter.c
DEP_CPP_TKFIL=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkFileFilter.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkFileFilter.obj"	"$(INTDIR)\tkFileFilter.sbr" : $(SOURCE)\
 $(DEP_CPP_TKFIL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkFocus.c
DEP_CPP_TKFOC=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkFocus.obj"	"$(INTDIR)\tkFocus.sbr" : $(SOURCE) $(DEP_CPP_TKFOC)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkFont.c
DEP_CPP_TKFON=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkFont.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkFont.obj"	"$(INTDIR)\tkFont.sbr" : $(SOURCE) $(DEP_CPP_TKFON)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkFrame.c
DEP_CPP_TKFRA=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkFrame.obj"	"$(INTDIR)\tkFrame.sbr" : $(SOURCE) $(DEP_CPP_TKFRA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkGC.c
DEP_CPP_TKGC_=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkGC.obj"	"$(INTDIR)\tkGC.sbr" : $(SOURCE) $(DEP_CPP_TKGC_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkGeometry.c
DEP_CPP_TKGEO=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkGeometry.obj"	"$(INTDIR)\tkGeometry.sbr" : $(SOURCE)\
 $(DEP_CPP_TKGEO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkGet.c
DEP_CPP_TKGET=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkGet.obj"	"$(INTDIR)\tkGet.sbr" : $(SOURCE) $(DEP_CPP_TKGET)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkGrab.c
DEP_CPP_TKGRA=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkGrab.obj"	"$(INTDIR)\tkGrab.sbr" : $(SOURCE) $(DEP_CPP_TKGRA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkGrid.c
DEP_CPP_TKGRI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkGrid.obj"	"$(INTDIR)\tkGrid.sbr" : $(SOURCE) $(DEP_CPP_TKGRI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkImage.c
DEP_CPP_TKIMA=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkImage.obj"	"$(INTDIR)\tkImage.sbr" : $(SOURCE) $(DEP_CPP_TKIMA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkImgBmap.c
DEP_CPP_TKIMG=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkImgBmap.obj"	"$(INTDIR)\tkImgBmap.sbr" : $(SOURCE)\
 $(DEP_CPP_TKIMG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkImgGIF.c
DEP_CPP_TKIMGG=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkImgGIF.obj"	"$(INTDIR)\tkImgGIF.sbr" : $(SOURCE) $(DEP_CPP_TKIMGG)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkImgPhoto.c
DEP_CPP_TKIMGP=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkImgPhoto.obj"	"$(INTDIR)\tkImgPhoto.sbr" : $(SOURCE)\
 $(DEP_CPP_TKIMGP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkImgPPM.c
DEP_CPP_TKIMGPP=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkImgPPM.obj"	"$(INTDIR)\tkImgPPM.sbr" : $(SOURCE)\
 $(DEP_CPP_TKIMGPP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkImgUtil.c
DEP_CPP_TKIMGU=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	"..\..\Tk\xlib\xbytes.h"\
	

"$(INTDIR)\tkImgUtil.obj"	"$(INTDIR)\tkImgUtil.sbr" : $(SOURCE)\
 $(DEP_CPP_TKIMGU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkListbox.c
DEP_CPP_TKLIS=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkListbox.obj"	"$(INTDIR)\tkListbox.sbr" : $(SOURCE)\
 $(DEP_CPP_TKLIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkMacWinMenu.c
DEP_CPP_TKMAC=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkMenu.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkMacWinMenu.obj"	"$(INTDIR)\tkMacWinMenu.sbr" : $(SOURCE)\
 $(DEP_CPP_TKMAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkMenu.c
DEP_CPP_TKMEN=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkMenu.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkMenu.obj"	"$(INTDIR)\tkMenu.sbr" : $(SOURCE) $(DEP_CPP_TKMEN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkMenubutton.c
DEP_CPP_TKMENU=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkMenubutton.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkMenubutton.obj"	"$(INTDIR)\tkMenubutton.sbr" : $(SOURCE)\
 $(DEP_CPP_TKMENU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkMenuDraw.c
DEP_CPP_TKMENUD=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkMenu.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkMenuDraw.obj"	"$(INTDIR)\tkMenuDraw.sbr" : $(SOURCE)\
 $(DEP_CPP_TKMENUD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkMessage.c
DEP_CPP_TKMES=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkMessage.obj"	"$(INTDIR)\tkMessage.sbr" : $(SOURCE)\
 $(DEP_CPP_TKMES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkOption.c
DEP_CPP_TKOPT=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkOption.obj"	"$(INTDIR)\tkOption.sbr" : $(SOURCE) $(DEP_CPP_TKOPT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkPack.c
DEP_CPP_TKPAC=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkPack.obj"	"$(INTDIR)\tkPack.sbr" : $(SOURCE) $(DEP_CPP_TKPAC)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkPlace.c
DEP_CPP_TKPLA=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkPlace.obj"	"$(INTDIR)\tkPlace.sbr" : $(SOURCE) $(DEP_CPP_TKPLA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkPointer.c
DEP_CPP_TKPOI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkPointer.obj"	"$(INTDIR)\tkPointer.sbr" : $(SOURCE)\
 $(DEP_CPP_TKPOI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkRectOval.c
DEP_CPP_TKREC=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkRectOval.obj"	"$(INTDIR)\tkRectOval.sbr" : $(SOURCE)\
 $(DEP_CPP_TKREC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkScale.c
DEP_CPP_TKSCA=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkScale.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkScale.obj"	"$(INTDIR)\tkScale.sbr" : $(SOURCE) $(DEP_CPP_TKSCA)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkScrollbar.c
DEP_CPP_TKSCR=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkScrollbar.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkScrollbar.obj"	"$(INTDIR)\tkScrollbar.sbr" : $(SOURCE)\
 $(DEP_CPP_TKSCR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkSelect.c
DEP_CPP_TKSEL=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkSelect.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkSelect.obj"	"$(INTDIR)\tkSelect.sbr" : $(SOURCE) $(DEP_CPP_TKSEL)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkSquare.c
DEP_CPP_TKSQU=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkSquare.obj"	"$(INTDIR)\tkSquare.sbr" : $(SOURCE) $(DEP_CPP_TKSQU)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkText.c
DEP_CPP_TKTEX=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkText.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkText.obj"	"$(INTDIR)\tkText.sbr" : $(SOURCE) $(DEP_CPP_TKTEX)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkTextBTree.c
DEP_CPP_TKTEXT=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkText.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkTextBTree.obj"	"$(INTDIR)\tkTextBTree.sbr" : $(SOURCE)\
 $(DEP_CPP_TKTEXT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkTextDisp.c
DEP_CPP_TKTEXTD=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkText.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkTextDisp.obj"	"$(INTDIR)\tkTextDisp.sbr" : $(SOURCE)\
 $(DEP_CPP_TKTEXTD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkTextImage.c
DEP_CPP_TKTEXTI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkText.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkTextImage.obj"	"$(INTDIR)\tkTextImage.sbr" : $(SOURCE)\
 $(DEP_CPP_TKTEXTI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkTextIndex.c
DEP_CPP_TKTEXTIN=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkText.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkTextIndex.obj"	"$(INTDIR)\tkTextIndex.sbr" : $(SOURCE)\
 $(DEP_CPP_TKTEXTIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkTextMark.c
DEP_CPP_TKTEXTM=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkText.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkTextMark.obj"	"$(INTDIR)\tkTextMark.sbr" : $(SOURCE)\
 $(DEP_CPP_TKTEXTM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkTextTag.c
DEP_CPP_TKTEXTT=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkText.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkTextTag.obj"	"$(INTDIR)\tkTextTag.sbr" : $(SOURCE)\
 $(DEP_CPP_TKTEXTT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkTextWind.c
DEP_CPP_TKTEXTW=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkText.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkTextWind.obj"	"$(INTDIR)\tkTextWind.sbr" : $(SOURCE)\
 $(DEP_CPP_TKTEXTW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkTrig.c
DEP_CPP_TKTRI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkCanvas.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkTrig.obj"	"$(INTDIR)\tkTrig.sbr" : $(SOURCE) $(DEP_CPP_TKTRI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\unix\tkUnixMenubu.c
DEP_CPP_TKUNI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkMenubutton.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkUnixMenubu.obj"	"$(INTDIR)\tkUnixMenubu.sbr" : $(SOURCE)\
 $(DEP_CPP_TKUNI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\unix\tkUnixScale.c
DEP_CPP_TKUNIX=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkScale.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkUnixScale.obj"	"$(INTDIR)\tkUnixScale.sbr" : $(SOURCE)\
 $(DEP_CPP_TKUNIX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkUtil.c
DEP_CPP_TKUTI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkUtil.obj"	"$(INTDIR)\tkUtil.sbr" : $(SOURCE) $(DEP_CPP_TKUTI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkVisual.c
DEP_CPP_TKVIS=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkVisual.obj"	"$(INTDIR)\tkVisual.sbr" : $(SOURCE) $(DEP_CPP_TKVIS)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWin32Dll.c
DEP_CPP_TKWIN=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWin32Dll.obj"	"$(INTDIR)\tkWin32Dll.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWin3d.c
DEP_CPP_TKWIN3=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tk3d.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWin3d.obj"	"$(INTDIR)\tkWin3d.sbr" : $(SOURCE) $(DEP_CPP_TKWIN3)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinButton.c
DEP_CPP_TKWINB=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkButton.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinButton.obj"	"$(INTDIR)\tkWinButton.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinClipboard.c
DEP_CPP_TKWINC=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkSelect.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinClipboard.obj"	"$(INTDIR)\tkWinClipboard.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinColor.c
DEP_CPP_TKWINCO=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkColor.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinColor.obj"	"$(INTDIR)\tkWinColor.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINCO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinCursor.c
DEP_CPP_TKWINCU=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinCursor.obj"	"$(INTDIR)\tkWinCursor.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINCU) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinDialog.c
DEP_CPP_TKWIND=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkFileFilter.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinDialog.obj"	"$(INTDIR)\tkWinDialog.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWIND) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\generic\tkWindow.c
DEP_CPP_TKWINDO=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWindow.obj"	"$(INTDIR)\tkWindow.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINDO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinDraw.c
DEP_CPP_TKWINDR=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinDraw.obj"	"$(INTDIR)\tkWinDraw.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINDR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinEmbed.c
DEP_CPP_TKWINE=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinEmbed.obj"	"$(INTDIR)\tkWinEmbed.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinFont.c
DEP_CPP_TKWINF=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkFont.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinFont.obj"	"$(INTDIR)\tkWinFont.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinImage.c
DEP_CPP_TKWINI=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinImage.obj"	"$(INTDIR)\tkWinImage.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinInit.c
DEP_CPP_TKWININ=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinInit.obj"	"$(INTDIR)\tkWinInit.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWININ) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinKey.c
DEP_CPP_TKWINK=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinKey.obj"	"$(INTDIR)\tkWinKey.sbr" : $(SOURCE) $(DEP_CPP_TKWINK)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinMenu.c
DEP_CPP_TKWINM=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\default.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkMenu.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinDefault.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinMenu.obj"	"$(INTDIR)\tkWinMenu.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinPixmap.c
DEP_CPP_TKWINP=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinPixmap.obj"	"$(INTDIR)\tkWinPixmap.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinPointer.c
DEP_CPP_TKWINPO=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinPointer.obj"	"$(INTDIR)\tkWinPointer.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINPO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinRegion.c
DEP_CPP_TKWINR=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinRegion.obj"	"$(INTDIR)\tkWinRegion.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinScrlbr.c
DEP_CPP_TKWINS=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\generic\tkScrollbar.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinScrlbr.obj"	"$(INTDIR)\tkWinScrlbr.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinSend.c
DEP_CPP_TKWINSE=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinSend.obj"	"$(INTDIR)\tkWinSend.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINSE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinWindow.c
DEP_CPP_TKWINW=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinWindow.obj"	"$(INTDIR)\tkWinWindow.sbr" : $(SOURCE)\
 $(DEP_CPP_TKWINW) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinWm.c
DEP_CPP_TKWINWM=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinWm.obj"	"$(INTDIR)\tkWinWm.sbr" : $(SOURCE) $(DEP_CPP_TKWINWM)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\win\tkWinX.c
DEP_CPP_TKWINX=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWin.h"\
	"..\..\Tk\win\tkWinInt.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\tkWinX.obj"	"$(INTDIR)\tkWinX.sbr" : $(SOURCE) $(DEP_CPP_TKWINX)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\xlib\xcolors.c
DEP_CPP_XCOLO=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\xcolors.obj"	"$(INTDIR)\xcolors.sbr" : $(SOURCE) $(DEP_CPP_XCOLO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\xlib\xdraw.c
DEP_CPP_XDRAW=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	

"$(INTDIR)\xdraw.obj"	"$(INTDIR)\xdraw.sbr" : $(SOURCE) $(DEP_CPP_XDRAW)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\xlib\xgc.c
DEP_CPP_XGC_C=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\xgc.obj"	"$(INTDIR)\xgc.sbr" : $(SOURCE) $(DEP_CPP_XGC_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\xlib\ximage.c
DEP_CPP_XIMAG=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\Tk\generic\tkInt.h"\
	"..\..\Tk\generic\tkPort.h"\
	"..\..\Tk\win\tkWinPort.h"\
	"..\..\tk\xlib\x11\cursorfont.h"\
	"..\..\tk\xlib\x11\keysym.h"\
	"..\..\tk\xlib\x11\keysymdef.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\ximage.obj"	"$(INTDIR)\ximage.sbr" : $(SOURCE) $(DEP_CPP_XIMAG)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Tk\xlib\xutil.c
DEP_CPP_XUTIL=\
	"..\..\Src\tcl-glue.h"\
	"..\..\Src\tk-glue.h"\
	"..\..\Tcl\tcl.h"\
	"..\..\Tk\generic\tk.h"\
	"..\..\tk\xlib\x11\x.h"\
	"..\..\tk\xlib\x11\xatom.h"\
	"..\..\tk\xlib\x11\xfuncproto.h"\
	"..\..\tk\xlib\x11\xlib.h"\
	"..\..\tk\xlib\x11\xutil.h"\
	

"$(INTDIR)\xutil.obj"	"$(INTDIR)\xutil.sbr" : $(SOURCE) $(DEP_CPP_XUTIL)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

