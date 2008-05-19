# makefile, written by guido socher
MCU=atmega32
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
#-------------------
all: 8x8leddriver.hex
#-------------------
help: 
	@echo "Usage: make [all]|load|load_pre|rdfuses|wrfuse1mhz|wrfuse4mhz|wrfuse8mhz|wrfusecrystal"
	@echo "Warning: you will not be able to undo wrfusecrystal unless you connect an"
	@echo "         external crystal! uC is dead after wrfusecrystal if you do not"
	@echo "         have an external crystal."
#-------------------
8x8leddriver.hex : 8x8leddriver.out 
	$(OBJCOPY) -R .eeprom -O ihex 8x8leddriver.out 8x8leddriver.hex 
8x8leddriver.out : 8x8leddriver.o 
	$(CC) $(CFLAGS) -o 8x8leddriver.out -Wl,-Map,8x8leddriver.map 8x8leddriver.o 
8x8leddriver.o : 8x8leddriver.c 
	$(CC) $(CFLAGS) -Os -c 8x8leddriver.c
# you need to erase first before loading the program.
# load (program) the software into the eeprom:
load: 8x8leddriver.hex
	avrdude -p m32 -c avrusb500 -P /dev/ttyUSB0 -U flash:w:8x8leddriver.hex
# here is a pre-compiled version in case you have trouble with
# your development environment
load_pre: 8x8leddriver_pre.hex
	uisp -dlpt=/dev/ttyUSB0 --erase  -dprog=avrusb500
	uisp -dlpt=/dev/ttyUSB0 --upload if=8x8leddriver_pre.hex -dprog=avrusb500 -dno-poll -v=3 --hash=32
#-------------------
# fuse byte settings:
#  Atmel AVR ATmega8 
#  Fuse Low Byte      = 0xe1 (1MHz internal), 0xe3 (4MHz internal), 0xe4 (8MHz internal)
#  Fuse High Byte     = 0xd9 
#  Factory default is 0xe1 for low byte and 0xd9 for high byte
# Check this with make rdfuses
rdfuses:
	uisp -dlpt=/dev/ttyUSB0 -dprog=avrusb500 --rd_fuses
# use internal RC oscillator 1 Mhz
wrfuse1mhz:
	uisp -dlpt=/dev/ttyUSB0 -dprog=avrusb500 --wr_fuse_l=0xe1
# use internal RC oscillator 4 Mhz
wrfuse4mhz:
	uisp -dlpt=/dev/ttyUSB0 -dprog=avrusb500 --wr_fuse_l=0xe3
# use internal RC oscillator 8 Mhz
wrfuse8mhz:
	uisp -dlpt=/dev/ttyUSB0 -dprog=avrusb500 --wr_fuse_l=0xe4
# use external 3-8 Mhz crystal
# Warning: you can not reset this to intenal unless you connect a crystal!!
wrfusecrystal:
	uisp -dlpt=/dev/ttyUSB0 -dprog=avrusb500 --wr_fuse_l=0xee
#-------------------
clean:
	rm -f *.o *.map *.out *t.hex
#-------------------

