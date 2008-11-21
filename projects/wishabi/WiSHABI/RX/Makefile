# Name: Makefile
# Project: InfraHID; CIR to HID mouse/keyboard converter
# Author: Alex Badea
# Creation Date: 2007-01-14
# Tabsize: 8
# Copyright: (c) 2007 Alex Badea <vamposdecampos@gmail.com>
# License: Proprietary, free under certain conditions. See Documentation.

COMPILE = avr-gcc -Wall -Os -Iusbdrv -Iserial -Imouse_mode -Ikeyboard_mode -Iusb -I. -mmcu=atmega8 #-DDEBUG_LEVEL=1
OBJECTS = usbdrv/usbdrv.o serial/serial.o mouse_mode/mouse_mode.o keyboard_mode/keyboard_mode.o usb/usb.o usbdrv/usbdrvasm.o usbdrv/oddebug.o WiSHABI_RX.o


# symbolic targets:
all:	WiSHABI_RX.hex

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
	rm -f WiSHABI_RX.hex WiSHABI_RX.lst WiSHABI_RX.obj WiSHABI_RX.cof WiSHABI_RX.list WiSHABI_RX.map WiSHABI_RX.eep.hex WiSHABI_RX.bin *.o usbdrv/*.o serial/*.o mouse_mode/*.o keyboard_mode/*.o usb/*.o WiSHABI_RX.s usbdrv/oddebug.s serial/oddebug.s mouse_mode/oddebug.s keyboard_mode/oddebug.s usb/oddebug.s usbdrv/usbdrv.s serial/serial.s usb/usb.s

# file targets:
WiSHABI_RX.bin:	$(OBJECTS)
	$(COMPILE) -o WiSHABI_RX.bin $(OBJECTS)

WiSHABI_RX.hex:	WiSHABI_RX.bin
	rm -f WiSHABI_RX.hex WiSHABI_RX.eep.hex
	avr-objcopy -j .text -j .data -O ihex WiSHABI_RX.bin WiSHABI_RX.hex
#	./checksize WiSHABI_RX.bin 8192 960

disasm:	WiSHABI_RX.bin
	avr-objdump -d WiSHABI_RX.bin

cpp:
	$(COMPILE) -E WiSHABI_RX.c
