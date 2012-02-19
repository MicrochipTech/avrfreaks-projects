REM avrdude -p m1284p -c usbasp -U flash:w:flash_pa.hex
avrdude -p m32 -c stk200 -U flash:w:optiboot_32_8mhz.hex

avrdude -p m32 -c stk200 -U lock:w:0x0f:m
pause
