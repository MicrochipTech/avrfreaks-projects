###############################################################################
# Makefile for the project speedLimiter
###############################################################################

## General Flags
PROJECT = speedLimiter
MCU = atmega88
TARGET = speedLimiter.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -O0
CFLAGS += -Wp,-M,-MP,-MT,$(*F).o,-MF,dep/$(@F).d 

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
OBJECTS = speedLimiter.o displayDrv.o EEPROM.o miscFuncs.o interrupts.o 

## Build
all: $(TARGET) speedLimiter.hex speedLimiter.eep

## Compile
speedLimiter.o: ../speedLimiter.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

displayDrv.o: ../displayDrv.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

EEPROM.o: ../EEPROM.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

miscFuncs.o: ../miscFuncs.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

interrupts.o: ../interrupts.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) speedLimiter.elf dep/ speedLimiter.hex speedLimiter.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

