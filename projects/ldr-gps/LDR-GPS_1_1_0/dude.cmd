@echo off

echo * Programming thru Butterfly preinstalled bootloder *
echo (tested with with AVRDUDE 5.3.1 win32 as in WinAVR20060122)
echo Select "BOOTLOADER" from the options menu or toggle power or issue a
echo reset via ISP connector. BF must have power-supply (battery or external) 
echo RS232 connection has to be o.k. (can be tested with "UPLOAD NAME").

echo Hit ENTER/Return-key on PC-Keyboard to continue
echo and press joystick ("enter") on BF. 
pause 

avrdude -u -F -D -p atmega16 -c butterfly -P com2 -b 19200 -e -U flash:w:default/bf_gcc.hex
rem avrdude -u -F -D -p atmega169 -c butterfly -P com2 -b 19200 -e -U flash:w:bf_gcc.hex

echo Move joystick "up" now and test if the applications starts. 
echo Depending on the AVRDUDE version you have to reset the device if 
echo "AVR-BUTTERFLY GCC" does not appear on the LCD now. AVRDUDE 
echo Versions below 4.4.0 need "hardware" reset or "power toogle".
pause
