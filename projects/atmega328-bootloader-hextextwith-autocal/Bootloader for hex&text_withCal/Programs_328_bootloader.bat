@echo off
color f0
title AvrDude GUI Command Window
@echo.
@echo. trying to connect to device...
avrdude -p ATMEGA328P -c usbasp -P COM3  -U flash:w:"C:\Users\Mark\Documents\USBasp programmer\win32_executable\Hex_txt_programmer_plusAutoCal.hex":i -F
avrdudeGUI.exe -done
pause