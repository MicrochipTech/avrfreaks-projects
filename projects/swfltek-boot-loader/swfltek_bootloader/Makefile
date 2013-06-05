SHELL = /bin/sh

ifneq (,)
This makefile requires GNU Make.
endif

include ../make.conf

all: flash.hex eeprom.hex

load: flash.hex eeprom.hex
	avrdude -p $(mcu) -c arduino -P $(bootport) -b$(baud) -U flash:w:flash.hex:i
	avrdude -p $(mcu) -c arduino -P $(bootport) -b$(baud) -U eeprom:w:eeprom.hex:i

flash.hex: test.elf
	avr-objcopy -j .text -j .data -O ihex test.elf flash.hex

eeprom.hex: test.elf
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex test.elf eeprom.hex

test.elf: test.o
	avr-gcc -mmcu=$(mcu) -Wl,-Map=test.map -o test.elf test.o
	avr-objdump -h -S test.elf > test.lst

test.o: test.c
	avr-gcc -c -Os -mmcu=$(mcu) -DF_CPU=$(clock) test.c -o test.o

clean:
	rm -f *.hex *.lst *.o *.map *.elf
