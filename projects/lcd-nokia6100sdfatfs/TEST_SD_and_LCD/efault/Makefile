###############################################################################
# Makefile for the project SD_and_LCD
###############################################################################

## General Flags
PROJECT = SD_and_LCD
MCU = atmega128
TARGET = SD_and_LCD.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                         -DF_CPU=14745600UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=SD_and_LCD.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o lcd_driver.o UART_routines.o ff.o diskio.o ccsbcs.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) SD_and_LCD.hex SD_and_LCD.eep SD_and_LCD.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd_driver.o: ../lcd_driver.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

UART_routines.o: ../UART_routines.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ff.o: ../ff.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

diskio.o: ../diskio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ccsbcs.o: ../ccsbcs.c
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
	-rm -rf $(OBJECTS) SD_and_LCD.elf dep/* SD_and_LCD.hex SD_and_LCD.eep SD_and_LCD.lss SD_and_LCD.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

