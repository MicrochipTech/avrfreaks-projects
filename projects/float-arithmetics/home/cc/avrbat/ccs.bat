@echo ccs compiles %1 >lastavr.log
if not "%OD%" == "AVR" goto :BAD_OD
avr-c++ %1.cpp -g -c -DNPRGPATH=3 -DMCU_CLOCK=4000000L -o avr/%1.o -I../include -I\WinAVR\avr\include -DAVR
avr-c++ %1.cpp -g -S -DNPRGPATH=3 -DMCU_CLOCK=4000000L -o avr/%1.s -I../include -I\WinAVR\avr\include -DAVR
REM mv %1.s
Echo Clock and NPRGPATH settings from home/cc/avrbat/ccs.bat
goto :END


:BAD_OD
echo Bad environment variable OD=, Try to run \HOME\CC\AVR.bat first!

:END

