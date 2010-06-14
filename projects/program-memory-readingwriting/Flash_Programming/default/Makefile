###############################################################################
# Makefile for the project Flash_Programming
###############################################################################

## General Flags
PROJECT = Flash_Programming
MCU = atmega64
TARGET = Flash_Programming.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=Flash_Programming.map
LDFLAGS += -Wl,-section-start=.bootloader=0xfc00


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"C:\Users\Luke\Documents\AVR Code\Mega64\Flash_Programming\..\..\..\..\..\..\WinAVR-20100110\avr\include\avr" 

## Objects that must be built in order to link
OBJECTS = Flash_Programming.o uart.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Flash_Programming.hex Flash_Programming.eep Flash_Programming.lss size

## Compile
Flash_Programming.o: ../Flash_Programming.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: ../../../../../../../WinAVR-20100110/avr/include/avr/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

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
	-rm -rf $(OBJECTS) Flash_Programming.elf dep/* Flash_Programming.hex Flash_Programming.eep Flash_Programming.lss Flash_Programming.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

