REM Installation program for STk

@echo off

set dest=%1

if "%dest%" == "" goto error

mkdir %dest%


REM ============ STk executable
mkdir %dest%\MS-Win32
copy Win32\Release\STk.exe		%dest%\MS-Win32

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
xcopy/s Demos				%dest%\Demos

REM ============== Help pages
mkdir %dest%\Help
copy Help\*.html			%dest%\Help

echo *** Installation completed in %dest%

goto end

:error
   echo Syntax: install directory
:end