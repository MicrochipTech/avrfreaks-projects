@echo off
Rem Pay attention to NOT disable RSTDISBL As You will need HI votage programming to get it back
REM CKSEL1=0 ,CKSEL0=0   
rem 256 ms + 18 CK 64 ms + 18 CK BOD disabled, slowly rising power
REM RSTDISBL=1 
rem When the RSTDISBL Fuse is unprogrammed, this pin serves as External Reset. 
REM When the RSTDISBL Fuse is programmed, this pin is a general input pin or a open-drain output pin. but WE CAN NOT Reprogram !!
rem When the SPIEN Fuse bit is programmed “0”, Low-voltage Serial Program and Data
rem SPIEN = 0
rem Downloading is enabled. Default value is programmed “0”. 
rem Unprogramming this fuse while in the Low-voltage Serial Programming mode will disable future In-System downloading attempts.
rem BODEN = 1
rem BODLEVEL=0 
rem 4.0V (BODLEVEL programmed (0)). 2.7V (BODLEVEL unprogrammed (1) ),
REm Fuse: "BODLEVEL,BODEN,SPIEN,RSTDISBL,0,0,CKSEL1,CKSEL0
REm 0 1 0 1  0 0 0 0

REM CKSEL1..0
REM 01, 10 	Calibrated Internal RC Oscillator 
REM 00 	External Clock 
REM 11 	128 kHz Internal Oscillator 

REM CKSEL1..0 Nominal Frequency
REM  10    9.6 MHz
REM  01    4.8 MHz

REM Start-up Times for the Internal Calibrated RC Oscillator Clock Selection
REM SUT1..0 Start-up Time from Power-down Additional Delay from Reset (VCC = 5.0V) Recommended Usage
REM 00 6 CK 14CK BOD enabled
REM 01 6 CK 14CK + 4 ms Fast rising power
REM 10 6 CK 14CK + 64 ms Slowly rising power
REM 11 Reserved

REM Start-up Times for the External Clock Selection
REM SUT1..0 Start-up Time from Powerdown and Power-save Additional Delay from Reset Recommended Usage
REM 00 6 CK 14CK BOD enabled
REM 01 6 CK 14CK + 4 ms Fast rising power
REM 10 6 CK 14CK + 64 ms Slowly rising power
REM 11 Reserved

REM BODLEVEL [1..0] Fuses Min VBOT Typ VBOT Max VBOT Units
REM 11 BOD Disabled
REM 10 1.8V
REM 01 2.7V
REM 00 4.3V

REM WDT Configuration as a Function of
REM WDTON Safety Level WDT Initial State
REM Unprogrammed 1 Disabled
REM Programmed 2 Enabled

rem WRITE-FUSE 0xff6A 
rem Lock = 0x3f 

rem Memory Lock Bits
rem Protection Type Mode LB1 LB2
rem 1	1 1 No memory lock features enabled.
rem 2	0 1 Further programming of the Flash and EEPROM is disabled.
rem 3	0 0 Same as mode 2, but verify is also disabled.

C:\WinAVR\bin\avrdude.exe -p t13 -P COM1 -c ponyser -u -U calibration:r:calibration.BIN:r -U flash:w:default\ServoControl.hex -U eeprom:w:default\ServoControl.eep -U eeprom:w:0x00:m -U hfuse:w:0xFF:m -U lfuse:w:0x6A:m -U lock:w:0x3f:m 
pause