@echo off
REM create destination directory for EXE files
MD \EXTC
GOTO :TLINK

if "%1" == "" goto :help
set LA=
set LB=%1
del %1.exe
:NEXT
if "%1" == "" goto :link
set LA=%LA% %OD%/%1.%OE%
shift
goto :NEXT

:LINK

REM TCC as linker :
REM tcc -e\extc\%LB%.EXE -l..\lq\tc\lq.lib -l=\tc\lib\emu.lib %LA%
goto :END
:HELP

goto :END



:TLINK
REM Check if the main programs name is given
if "%1" == "" goto :ahelp
REM Start creation of linker command file
echo \tc\lib\c0s+ >xx.tmp
set LB=%1

REM Loop: add all other binaries (More than one .OBJ may be given)
:ANEXT
if "%2" == "" goto :T44
echo %OD%\%1+ >>xx.tmp
shift
goto :ANEXT

REM No more object files, all are in linker control file
:T44
REM +\TC\LIB\CS.LIB+\tc\lib\cs.lib 
echo %OD%\%1 >>xx.tmp
REM EXE file name to control file
echo \EX%OD%\%LB%.EXE >>xx.tmp
REM Create map file %1.log
echo %1.log >>xx.tmp
REM add all library files
echo \HOME\CC\LQ\TC\LQ.LIB+ >>xx.tmp
echo \TC\LIB\CS.LIB+ >>xx.tmp
echo \tc\lib\maths.lib+ >>xx.tmp
echo \tc\lib\fp87.lib >>xx.tmp
REM set LIB=C:\TC\LIB
REM
REM the linker control file, xx.tmp is ready
echo linking %LB% by Turbo Cpp linker, invocation through LNK.BAT, xaprompt xx.tmp
tlink @xx.tmp
goto :END
:AHELP
@ECHO LNK.BAT
@ECHO Invocation : LNK name
@ECHO will link \%OD%\name.OBJ with Borlands Turbo C++ for DOS library
goto :END


:end
set LB=
set LA=
