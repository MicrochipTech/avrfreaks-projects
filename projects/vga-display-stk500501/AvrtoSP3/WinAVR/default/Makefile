###############################################################################
# Makefile for the project AvrDebugger
###############################################################################

## General Flags
PROJECT = AvrDebugger
MCU = atmega2561
TARGET = AcornIITestFixture.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2  -O1 -fsigned-char
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = swap.o task.o BlockAlloc.o ByteQueue.o main.o MessageQueue.o rs232.o pq.o malloc.o cio.o display.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) AcornIITestFixture.hex AcornIITestFixture.eep size

## Compile
swap.o: ../swap.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

task.o: ../task.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

BlockAlloc.o: ../BlockAlloc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ByteQueue.o: ../ByteQueue.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MessageQueue.o: ../MessageQueue.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

rs232.o: ../rs232.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pq.o: ../pq.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

malloc.o: ../malloc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

cio.o: ../cio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

display.o: ../display.c
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
	-rm -rf $(OBJECTS) AcornIITestFixture.elf dep/* AcornIITestFixture.hex AcornIITestFixture.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

