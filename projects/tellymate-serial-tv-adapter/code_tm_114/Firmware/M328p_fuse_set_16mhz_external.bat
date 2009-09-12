REM Many thanks to http://www.engbedded.com/fusecalc
REM
avrdude -p atmega328p -c usbasp -U lfuse:w:0xff:m -U hfuse:w:0xd9:m -U efuse:w:0x07:m
pause

