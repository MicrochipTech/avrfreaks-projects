@echo off
color f0
title AvrDude GUI Command Window
@echo.
@echo. trying to connect to device...
avrdude -p ATMEGA328P -c usbasp -P COM3  -U efuse:w:0x5:m	-U hfuse:w:0xD0:m	-U lfuse:w:0xC2:m	-U lock:w:0x2F:m -F
avrdudeGUI.exe -done
pause