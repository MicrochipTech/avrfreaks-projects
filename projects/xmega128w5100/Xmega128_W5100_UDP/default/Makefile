###############################################################################
# Makefile for the project Xmega128_W5100_UDP
###############################################################################

## General Flags
PROJECT = Xmega128_W5100_UDP
MCU = atxmega128a1
TARGET = Xmega128_W5100_UDP.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99     -DF_CPU=32000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=Xmega128_W5100_UDP.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = clksys_driver.o main.o spi_xmega.o usart.o wiznetw5100.o wdt_driver.o port_driver.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) Xmega128_W5100_UDP.hex Xmega128_W5100_UDP.eep Xmega128_W5100_UDP.lss size

## Compile
clksys_driver.o: ../clksys_driver.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi_xmega.o: ../spi_xmega.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart.o: ../usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

wiznetw5100.o: ../wiznetw5100.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

wdt_driver.o: ../wdt_driver.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

port_driver.o: ../port_driver.c
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
	-rm -rf $(OBJECTS) Xmega128_W5100_UDP.elf dep/* Xmega128_W5100_UDP.hex Xmega128_W5100_UDP.eep Xmega128_W5100_UDP.lss Xmega128_W5100_UDP.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

