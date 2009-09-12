REM Many thanks to http://www.engbedded.com/fusecalc
REM
avrdude -p atmega168 -c usbasp -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m
pause

