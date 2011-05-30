###############################################################################
# Makefile for the project lito
###############################################################################

## General Flags
PROJECT = lito
MCU = atmega64
TARGET = lito.elf
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
LDFLAGS +=  -Wl,-Map=lito.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = analog_clk.o blink.o ext_int.o globals.o i2c_com.o image_process.o lito.o propeller.o rs232.o spi_com.o digital_clk.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) lito.hex lito.eep lito.lss size

## Compile
analog_clk.o: ../analog_clk.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

blink.o: ../blink.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ext_int.o: ../ext_int.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

globals.o: ../globals.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c_com.o: ../i2c_com.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

image_process.o: ../image_process.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lito.o: ../lito.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

propeller.o: ../propeller.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rs232.o: ../rs232.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi_com.o: ../spi_com.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

digital_clk.o: ../digital_clk.c
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
	-rm -rf $(OBJECTS) lito.elf dep/* lito.hex lito.eep lito.lss lito.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

