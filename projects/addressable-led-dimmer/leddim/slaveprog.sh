#!/bin/bash
clear
# use http://frank.circleofcurrent.com/fusecalc/fusecalc.php?chip=atmega328p
# to calculate fuses and lockbits.
cd slave
make
sudo avrdude -c usbtiny -p atmega328p -P usb -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0x07:m -U flash:w:slave.hex

