OC=avr-objcopy
CC=avr-gcc
AD=avrdude

MCU=attiny13
F_CPU=1200000
PROG=bsd

CDEFS=-DF_CPU=$(F_CPU)UL
CFLAGS=-Os -mcall-prologues -Wall -mmcu=$(MCU) -g

dummy:
	@echo "targets include 'build', 'program' and 'clean'"

build: pedometer.ihex

pedometer.ihex: pedometer.c data.h Makefile
	$(CC) $(CFLAGS) $(CDEFS) pedometer.c -o pedometer.out
	$(OC) -j .text -O ihex pedometer.out pedometer.ihex

program: pedometer.ihex
	$(AD) -p $(MCU) -c $(PROG) -e -U flash:w:pedometer.ihex -U lfuse:w:0x6A:m

clean:
	rm -f *.out *.ihex
