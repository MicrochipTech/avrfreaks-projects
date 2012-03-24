# ------------------------------------------------
#
#  "Mattress with Motion"
#
#  File    : go.sh
#  Version : February 2012
#
#
#  With avr-gcc 4.3.5 in Linux Mint 11 and avrdude 5.10.
#

avr-gcc -Wall -Os -mmcu=atmega88pa -c mattress.c
avr-gcc -mmcu=atmega88pa -o mattress.elf mattress.o
avr-objcopy -j .text -j .data -O ihex mattress.elf mattress.hex

# The ATmega88PA is not yet in the database of avrdude 5.10
# So use the ATmega88, with the (dangerous) flag '-F'.
# The new avrdude 5.11 does support the atmega88p.

sudo avrdude -c usbasp -p atmega88 -F -U flash:w:mattress.hex

# Show memory usage
avr-size -C --mcu=atmega88pa mattress.elf

