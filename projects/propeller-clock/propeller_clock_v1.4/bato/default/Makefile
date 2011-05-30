###############################################################################
# Makefile for the project mmlml
###############################################################################

## General Flags
PROJECT = mmlml
MCU = atmega64
TARGET = mmlml.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=mmlml.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = spi_com.o analog_clk.o blink.o ext_int.o globals.o i2c_com.o image_process.o propeller.o rs232.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) mmlml.hex mmlml.eep mmlml.lss size

## Compile
spi_com.o: ../../../../../../../../../../../Desktop/propeller/spi_com.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

analog_clk.o: ../../../../../../../../../../../Desktop/propeller/analog_clk.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

blink.o: ../../../../../../../../../../../Desktop/propeller/blink.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ext_int.o: ../../../../../../../../../../../Desktop/propeller/ext_int.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

globals.o: ../../../../../../../../../../../Desktop/propeller/globals.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c_com.o: ../../../../../../../../../../../Desktop/propeller/i2c_com.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

image_process.o: ../../../../../../../../../../../Desktop/propeller/image_process.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

propeller.o: ../../../../../../../../../../../Desktop/propeller/propeller.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rs232.o: ../../../../../../../../../../../Desktop/propeller/rs232.c
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
	-rm -rf $(OBJECTS) mmlml.elf dep/* mmlml.hex mmlml.eep mmlml.lss mmlml.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

