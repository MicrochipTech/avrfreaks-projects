@echo ccs compiles %1 >lasttc.log
if not "%OD%" == "TC" goto :BAD_OD
tcc -2 -f287 -a -b -O -c -I\home\cc\include -o%OD%\%1.%OE% %1.cpp >lasttc.log
if exist alltc.log copy alltc.log+lasttc.log alltc.log
if not exist alltc.log copy lasttc.log alltc.log
@type lasttc.log
@echo off
goto :END
REM -2 = 286 Code
REM -f287 = Coprocessor present
REM -ff = FastFloating point routines
REM -a = Address Alignment
REM -b = Word Enum
REM -k = Debug Entry / Return
REM -N = Stack-Check
REM -O = Optimize
REM -G = Speed Optimize


:BAD_OD
echo Bad environment variable OD=, Try to run \HOME\CC\tc.bat first!

:END

