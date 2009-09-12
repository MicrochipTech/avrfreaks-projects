@ECHO OFF

REM Note: A default Mega168 has lfuse:62, hfuse:df, efuse:f9
REM Note: -B 5 overrides the USBasp's current setting of the "slow" jumper. e.g. GO SLOW

@ECHO ON
avrdude -B 5 -c usbasp -p atmega168 -U lfuse:r:-:h -qq
@ECHO OFF
IF %ERRORLEVEL% NEQ 0 GOTO Failed
ECHO "Read LFuse: OK"

@ECHO ON
avrdude -B 5 -p atmega168 -c usbasp -U hfuse:r:-:h -qq
@ECHO OFF
IF %ERRORLEVEL% NEQ 0 GOTO Failed
ECHO "Read HFuse: OK"

@ECHO ON
avrdude -B 5 -c usbasp -p atmega168 -U efuse:r:-:h -qq
@ECHO OFF
IF %ERRORLEVEL% NEQ 0 GOTO Failed
ECHO "Read EFuse: OK"

@ECHO ON
avrdude -B 5 -p atmega168 -c usbasp -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m -qq
@ECHO OFF
IF %ERRORLEVEL% NEQ 0 GOTO Failed
ECHO "Fuses set to 16Mhz external crystal"

@ECHO ON
avrdude -p atmega168 -c usbasp -U flash:w:TellyMate_M168.hex
@ECHO OFF
IF %ERRORLEVEL% NEQ 0 GOTO Failed
ECHO "Microcontroller Programmed."


:Success
ECHO "ALL OK"
GOTO EndOfScript
:Failed
ECHO FAILED!
:EndOfScript
pause
