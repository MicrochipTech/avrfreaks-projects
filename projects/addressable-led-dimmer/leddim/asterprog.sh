#!/bin/bash
clear
# use http://frank.circleofcurrent.com/fusecalc/fusecalc.php?chip=atmega328p
# to calculate fuses and lockbits.
cd master
make
sudo avrdude -c usbtiny -p atmega1284p -P usb -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0xff:m -U flash:w:master.hex

