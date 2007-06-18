###############################################################################
# Makefile for the project kitchen-sink
###############################################################################

## General Flags
PROJECT = kitchen-sink
MCU = atmega168
TARGET = kitchen-sink.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -I"../../" -Os -fsigned-char
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
OBJECTS = kitchen-sink.o pwm.o analog.o buzzer.o lcd.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) kitchen-sink.hex kitchen-sink.eep size

## Compile
kitchen-sink.o: ../kitchen-sink.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwm.o: ../../pwm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

analog.o: ../../analog.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

buzzer.o: ../../buzzer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd.o: ../../lcd.c
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
	-rm -rf $(OBJECTS) kitchen-sink.elf dep/* kitchen-sink.hex kitchen-sink.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

