###############################################################################
# Makefile for the project lcd-bargraph-test
###############################################################################

## General Flags
PROJECT = lcd-bargraph-test
MCU = atmega168
TARGET = lcd-bargraph-test.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -fsigned-char -I../../..
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0


## Objects that must be built in order to link
OBJECTS = lcd-bargraph-test.o lcd-bargraph.o lcd.o analog.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) lcd-bargraph-test.hex lcd-bargraph-test.eep size

## Compile
lcd-bargraph-test.o: ../lcd-bargraph-test.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd-bargraph.o: ../lcd-bargraph.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd.o: ../../../lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

analog.o: ../../../analog.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) lcd-bargraph-test.elf dep/* lcd-bargraph-test.hex lcd-bargraph-test.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

