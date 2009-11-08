###############################################################################
# Makefile for the project Logger
###############################################################################

## General Flags
PROJECT = Logger
MCU = atmega169p
TARGET = Logger.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99   -DF_CPU=1000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=Logger.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"E:\Projects\AVRbutterfly\Logger\." 

## Libraries
LIBS = -lc 

## Objects that must be built in order to link
OBJECTS = ADC.o BCD.o button.o dataflash.o LCD_driver.o LCD_functions.o main.o RTC.o usart.o timer0.o record.o eeprom169.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Logger.hex Logger.eep Logger.lss size

## Compile
ADC.o: ../ADC.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

BCD.o: ../BCD.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

button.o: ../button.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dataflash.o: ../dataflash.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

LCD_driver.o: ../LCD_driver.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

LCD_functions.o: ../LCD_functions.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

RTC.o: ../RTC.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart.o: ../usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

timer0.o: ../timer0.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

record.o: ../record.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

eeprom169.o: ../eeprom169.c
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
	-rm -rf $(OBJECTS) Logger.elf dep/* Logger.hex Logger.eep Logger.lss Logger.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

