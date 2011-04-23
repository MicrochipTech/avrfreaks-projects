#! /bin/bash
cd default
clear
make
cd ../
# used http://frank.circleofcurrent.com/fusecalc/fusecalc.php?chip=atmega328p
# to calculate fuses and lockbits.
sudo avrdude -c avrisp2 -p atmega328p -P usb -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U flash:w:default/dmxtopwm4chan.hex
