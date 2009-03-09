###############################################################################
# Makefile for the project mmc
###############################################################################

## General Flags
PROJECT = mmc
MCU = atmega16
TARGET = sd.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wmissing-prototypes -DF_CPU=16000000UL -Os  -std=gnu99
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=sd.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o queue.o sci.o spi.o terminal.o command.o sd.o utils.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) sd.hex sd.eep sd.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

queue.o: ../queue.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sci.o: ../sci.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi.o: ../spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

terminal.o: ../terminal.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

command.o: ../command.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sd.o: ../sd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

utils.o: ../utils.c
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
	-rm -rf $(OBJECTS) sd.elf dep/* sd.hex sd.eep sd.lss sd.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

