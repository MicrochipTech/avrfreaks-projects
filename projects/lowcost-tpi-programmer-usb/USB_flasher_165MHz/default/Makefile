###############################################################################
# Makefile for the project USB_hid_data
###############################################################################

## General Flags
PROJECT = USB_hid_data
MCU = attiny25
TARGET = USB_hid_data.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99      -ffreestanding  -DF_CPU=16500000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -nodefaultlibs  -nostdlib  -ffreestanding -Wl,-Map=USB_hid_data.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"C:\Dokumente und Einstellungen\Besitzer\Eigene Dateien\USB_flasher_165MHz\." -I"C:\Dokumente und Einstellungen\Besitzer\Eigene Dateien\USB_flasher_165MHz\usbdrv" 

## Objects that must be built in order to link
OBJECTS = main.o oddebug.o usbdrvasm.o gcrt1.o libgcc.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) USB_hid_data.hex USB_hid_data.eep USB_hid_data.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

oddebug.o: ../usbdrv/oddebug.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usbdrvasm.o: ../usbdrv/usbdrvasm.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

gcrt1.o: ../tinystartup/gcrt1.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

libgcc.o: ../tinystartup/libgcc.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) USB_hid_data.elf dep/* USB_hid_data.hex USB_hid_data.eep USB_hid_data.lss USB_hid_data.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

