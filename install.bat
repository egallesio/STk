REM Installation program for STk

@echo off

set dest=%1

if "%dest%" == "" goto error

mkdir %dest%

REM ============ The install documentation file for binary release
copy Win32\Doc\install.doc 		%dest%

REM ============ STk executable & libraries
mkdir %dest%\MS-Win32
copy Win32\Release\STk.exe		%dest%\MS-Win32
copy Win32\Win32libs\*.lib		%dest%\MS-Win32

REM ============ Header files
mkdir %dest%\Include
copy Tcl\tcl.h 				%dest%\Include
copy Tcl\tclInt.h			%dest%\Include
copy Tcl\tclRegexp.h			%dest%\Include

copy Tk\tk.h				%dest%\Include
copy Tk\tkInt.h				%dest%\Include

copy Mp\fgmp-1.05b\gmp.h		%dest%\Include

copy Src\stk.h				%dest%\Include
copy Src\tcl-glue.h			%dest%\Include
copy Src\tk-glue.h			%dest%\Include

REM ============ The Library directory
mkdir %dest%\STk
mkdir %dest%\STk\Match

copy Lib\*.stk       			%dest%\STk
copy Lib\*.stklos	    		%dest%\STk
copy Lib\Match\*.scm			%dest%\STk\Match

REM ============= The STklos directory
copy STklos\*.stk         		%dest%\STk
copy STklos\*.stklos      		%dest%\STk
copy STklos\Tk\*.stklos           	%dest%\STk
copy STklos\Tk\Composite\*.stklos 	%dest%\STk

REM ============= Images
mkdir %dest%\Images
copy Lib\Images\*.*			%dest%\Images
copy Lib\Images\*			%dest%\Images

REM ============== Demos
mkdir %dest%\Demos
copy Demos\*.stk			%dest%\Demos
copy Demos\*.stklos			%dest%\Demos
copy Demos\Widget\*.stklos		%dest%\Demos
mkdir %dest%\Demos\Html-Demos
mkdir %dest%\Demos\Html-Demos\Images
copy Demos\Html-Demos\*.*		%dest%\Demos\Html-Demos
copy Demos\Html-Demos\Images\*.*	%dest%\Demos\Html-Demos\Images

REM ============== Help pages
mkdir %dest%\Help
mkdir %dest%\Help\Img
copy Help\*.html			%dest%\Help
copy Help\Img\*.*			%dest%\Help\Img

echo *** Installation completed in %dest%

goto end

:error
   echo Syntax: install directory
:end
