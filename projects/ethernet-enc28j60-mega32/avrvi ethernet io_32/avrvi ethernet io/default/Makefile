###############################################################################
# Makefile for the project avrETH1
###############################################################################

## General Flags
PROJECT = avrETH1
MCU = atmega32
TARGET = avrETH1.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=7372800UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=avrETH1.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = avrETH1.o servo.o base64.o port.o mpf10.o mca25.o logger.o filesystem.o lm75.o string.o plot.o httpd.o tcp.o enc28j60.o crc8.o dataflash.o softuart.o clock.o icmp.o nethelp.o mynic.o arp.o udp.o ip.o ntp_client.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) avrETH1.hex avrETH1.eep avrETH1.lss size

## Compile
avrETH1.o: ../avrETH1.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

servo.o: ../io/servo.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

base64.o: ../net/base64.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

port.o: ../io/port.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mpf10.o: ../io/mpf10.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mca25.o: ../io/mca25.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

logger.o: ../apps/logger.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

filesystem.o: ../io/filesystem.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lm75.o: ../io/lm75.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

string.o: ../util/string.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

plot.o: ../io/plot.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

httpd.o: ../net/httpd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tcp.o: ../net/tcp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

enc28j60.o: ../io/enc28j60.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

crc8.o: ../util/crc8.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dataflash.o: ../io/dataflash.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

softuart.o: ../io/softuart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

clock.o: ../io/clock.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

icmp.o: ../net/icmp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

nethelp.o: ../net/nethelp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mynic.o: ../net/mynic.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

arp.o: ../net/arp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

udp.o: ../net/udp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ip.o: ../net/ip.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ntp_client.o: ../net/ntp_client.c
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
	-rm -rf $(OBJECTS) avrETH1.elf dep/* avrETH1.hex avrETH1.eep avrETH1.lss avrETH1.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

