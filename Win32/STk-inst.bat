REM Installation program for STk

@echo off
mkdir \STk
mkdir \STk\STk
mkdir \STk\images
mkdir \STk\demos
mkdir \STk\help


REM ============ STk executable
copy .\stk.exe				\STk

REM ============ The Library directory
copy ..\Lib\*.stk       		\STk\STk
copy ..\Lib\*.stklos    		\STk\STk
copy ..\Lib\prolog.ps			\STk\STk

REM ============= The STklos directory
copy ..\STklos\*.stk         		\STk\STk
copy ..\STklos\*.stklos      		\STk\STk
copy ..\STklos\Tk\*.stklos           	\STk\STk
copy ..\STklos\Tk\Composite\*.stklos 	\STk\STk

REM ============= Images
copy ..\Lib\images\*.*			\STk\images

REM ============== Demos
copy ..\Demos\*.stk			\STk\demos
copy ..\Demos\*.stklos 			\STk\demos
copy ..\Demos\Widget\*.stklos		\STk\demos


REM ============== Help pages
copy ..\Help\*.html			\STk\Help

echo Installation completed.
echo Don't forget to set the STK_LIBRARY variable.
