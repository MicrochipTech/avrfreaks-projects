###############################################################################
# Makefile for the project D12_Test1
###############################################################################

## General Flags
PROJECT = D12_Test1
MCU = atmega168
TARGET = D12_Test1.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2                 -DF_CPU=8000000UL -Os -fsigned-char
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
OBJECTS = D12_Test1.o D12_Operation.o D12_Int.o CHAP_9.o MMC_SD.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) D12_Test1.hex D12_Test1.eep size

## Compile
D12_Test1.o: ../D12_Test1.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

D12_Operation.o: ../D12/D12_Operation.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

D12_Int.o: ../D12/D12_Int.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

CHAP_9.o: ../D12/CHAP_9.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MMC_SD.o: ../MMC_SD/MMC_SD.c
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
	-rm -rf $(OBJECTS) D12_Test1.elf dep/* D12_Test1.hex D12_Test1.eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

