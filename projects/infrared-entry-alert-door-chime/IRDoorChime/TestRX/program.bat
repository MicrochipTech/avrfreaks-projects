avrdude -p t2313 -b 115200 -c avr910 -C avrdude.conf -P com4 -e -U flash:w:RX_IR.hex -U lfuse:w:0xfd:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
pause