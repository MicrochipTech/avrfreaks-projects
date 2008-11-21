MCU = atmega8
F_CPU = 12000000

# Name: Makefile
# Project: InfraHID; CIR to HID mouse/keyboard converter
# Author: Alex Badea
# Creation Date: 2007-01-14
# Tabsize: 8
# Copyright: (c) 2007 Alex Badea <vamposdecampos@gmail.com>
# License: Proprietary, free under certain conditions. See Documentation.

COMPILE = avr-gcc -Wall -Os -Iserial -I. -mmcu=atmega8 #-DDEBUG_LEVEL=1
OBJECTS = WiSHABI_TX.o serial/serial.o


# symbolic targets:
all:	WiSHABI_TX.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

# Fuse high byte:
# 0xc9 = 1 1 0 0   1 0 0 1 <-- BOOTRST (boot reset vector at 0x0000)
#        ^ ^ ^ ^   ^ ^ ^------ BOOTSZ0
#        | | | |   | +-------- BOOTSZ1
#        | | | |   + --------- EESAVE (don't preserve EEPROM over chip erase)
#        | | | +-------------- CKOPT (full output swing)
#        | | +---------------- SPIEN (allow serial programming)
#        | +------------------ WDTON (WDT not always on)
#        +-------------------- RSTDISBL (reset pin is enabled)
# Fuse low byte:
# 0x9f = 1 0 0 1   1 1 1 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (external >8M crystal)
#        | |  +--------------- SUT 1..0 (crystal osc, BOD enabled)
#        | +------------------ BODEN (BrownOut Detector enabled)
#        +-------------------- BODLEVEL (2.7V)

clean:
	rm -f WiSHABI_TX.hex WiSHABI_TX.lst WiSHABI_TX.obj WiSHABI_TX.cof WiSHABI_TX.list WiSHABI_TX.map WiSHABI_TX.eep.hex WiSHABI_TX.bin WiSHABI_TX.s serial/*.o serial/oddebug.s serial/serial.s

# file targets:
WiSHABI_TX.bin:	$(OBJECTS)
	$(COMPILE) -o WiSHABI_TX.bin $(OBJECTS)

WiSHABI_TX.hex:	WiSHABI_TX.bin
	rm -f WiSHABI_TX.hex WiSHABI_TX.eep.hex
	avr-objcopy -j .text -j .data -O ihex WiSHABI_TX.bin WiSHABI_TX.hex
#	./checksize WiSHABI_TX.bin 8192 960

disasm:	WiSHABI_TX.bin
	avr-objdump -d WiSHABI_TX.bin

cpp:
	$(COMPILE) -E WiSHABI_TX.c
