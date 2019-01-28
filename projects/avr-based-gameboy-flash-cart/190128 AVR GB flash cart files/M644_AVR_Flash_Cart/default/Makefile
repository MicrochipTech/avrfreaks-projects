###############################################################################
# Makefile for the project M644_512KB_FLASH_PROTO
###############################################################################

## General Flags
PROJECT = M644_512KB_FLASH_PROTO
MCU = atmega644
TARGET = M644_512KB_FLASH_PROTO.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99                                                           -DF_CPU=25804800UL -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=M644_512KB_FLASH_PROTO.map
LDFLAGS += -Wl,-section-start=.MBC1_16M_0000=0x8000
LDFLAGS += -Wl,-section-start=.MBC1_16M_0010=0x8040
LDFLAGS += -Wl,-section-start=.MBC1_16M_0100=0x8080
LDFLAGS += -Wl,-section-start=.MBC1_16M_0110=0x80c0
LDFLAGS += -Wl,-section-start=.MBC1_16M_1000=0x8100
LDFLAGS += -Wl,-section-start=.MBC1_16M_1010=0x8140
LDFLAGS += -Wl,-section-start=.MBC1_16M_1100=0x8180
LDFLAGS += -Wl,-section-start=.MBC1_16M_1110=0x81c0
LDFLAGS += -Wl,-section-start=.MBC1_16M_0001=0x8020
LDFLAGS += -Wl,-section-start=.MBC1_16M_0011=0x8060
LDFLAGS += -Wl,-section-start=.MBC1_16M_0101=0x80a0
LDFLAGS += -Wl,-section-start=.MBC1_16M_0111=0x80e0
LDFLAGS += -Wl,-section-start=.MBC1_16M_1001=0x8120
LDFLAGS += -Wl,-section-start=.MBC1_16M_1011=0x8160
LDFLAGS += -Wl,-section-start=.MBC1_16M_1101=0x81a0
LDFLAGS += -Wl,-section-start=.MBC1_16M_1111=0x81e0
LDFLAGS += -Wl,-section-start=.MBC1_4M_0000=0x8200
LDFLAGS += -Wl,-section-start=.MBC1_4M_0010=0x8240
LDFLAGS += -Wl,-section-start=.MBC1_4M_0100=0x8280
LDFLAGS += -Wl,-section-start=.MBC1_4M_0110=0x82c0
LDFLAGS += -Wl,-section-start=.MBC1_4M_1000=0x8300
LDFLAGS += -Wl,-section-start=.MBC1_4M_1010=0x8340
LDFLAGS += -Wl,-section-start=.MBC1_4M_1100=0x8380
LDFLAGS += -Wl,-section-start=.MBC1_4M_1110=0x83c0
LDFLAGS += -Wl,-section-start=.MBC1_4M_0001=0x8220
LDFLAGS += -Wl,-section-start=.MBC1_4M_0011=0x8260
LDFLAGS += -Wl,-section-start=.MBC1_4M_0101=0x82a0
LDFLAGS += -Wl,-section-start=.MBC1_4M_0111=0x82e0
LDFLAGS += -Wl,-section-start=.MBC1_4M_1001=0x8320
LDFLAGS += -Wl,-section-start=.MBC1_4M_1011=0x8360
LDFLAGS += -Wl,-section-start=.MBC1_4M_1101=0x83a0
LDFLAGS += -Wl,-section-start=.MBC1_4M_1111=0x83e0
LDFLAGS += -Wl,-section-start=.MBC5_0000=0x8400
LDFLAGS += -Wl,-section-start=.MBC5_0010=0x8440
LDFLAGS += -Wl,-section-start=.MBC5_0100=0x8480
LDFLAGS += -Wl,-section-start=.MBC5_0110=0x84c0
LDFLAGS += -Wl,-section-start=.MBC5_1000=0x8500
LDFLAGS += -Wl,-section-start=.MBC5_1010=0x8540
LDFLAGS += -Wl,-section-start=.MBC5_1100=0x8580
LDFLAGS += -Wl,-section-start=.MBC5_1110=0x85c0
LDFLAGS += -Wl,-section-start=.MBC5_0001=0x8420
LDFLAGS += -Wl,-section-start=.MBC5_0011=0x8460
LDFLAGS += -Wl,-section-start=.MBC5_0101=0x84a0
LDFLAGS += -Wl,-section-start=.MBC5_0111=0x84e0
LDFLAGS += -Wl,-section-start=.MBC5_1001=0x8520
LDFLAGS += -Wl,-section-start=.MBC5_1011=0x8560
LDFLAGS += -Wl,-section-start=.MBC5_1101=0x85a0
LDFLAGS += -Wl,-section-start=.MBC5_1111=0x85e0
LDFLAGS += -Wl,-section-start=.MBC_BASE=0x7e00
LDFLAGS += -Wl,-section-start=.MBC3_0000=0x8600
LDFLAGS += -Wl,-section-start=.MBC3_0010=0x8640
LDFLAGS += -Wl,-section-start=.MBC3_0100=0x8680
LDFLAGS += -Wl,-section-start=.MBC3_0110=0x86c0
LDFLAGS += -Wl,-section-start=.MBC3_1000=0x8700
LDFLAGS += -Wl,-section-start=.MBC3_1010=0x8740
LDFLAGS += -Wl,-section-start=.MBC3_1100=0x8780
LDFLAGS += -Wl,-section-start=.MBC3_1110=0x87c0
LDFLAGS += -Wl,-section-start=.MBC3_0001=0x8620
LDFLAGS += -Wl,-section-start=.MBC3_0011=0x8660
LDFLAGS += -Wl,-section-start=.MBC3_0101=0x86a0
LDFLAGS += -Wl,-section-start=.MBC3_0111=0x86e0
LDFLAGS += -Wl,-section-start=.MBC3_1001=0x8720
LDFLAGS += -Wl,-section-start=.MBC3_1011=0x8760
LDFLAGS += -Wl,-section-start=.MBC3_1101=0x87a0
LDFLAGS += -Wl,-section-start=.MBC3_1111=0x87e0
LDFLAGS += -Wl,-section-start=.MBC5_DSM_0000=0x8800
LDFLAGS += -Wl,-section-start=.MBC5_DSM_0010=0x8840
LDFLAGS += -Wl,-section-start=.MBC5_DSM_0100=0x8880
LDFLAGS += -Wl,-section-start=.MBC5_DSM_0110=0x88c0
LDFLAGS += -Wl,-section-start=.MBC5_DSM_1000=0x8900
LDFLAGS += -Wl,-section-start=.MBC5_DSM_1010=0x8940
LDFLAGS += -Wl,-section-start=.MBC5_DSM_1100=0x8980
LDFLAGS += -Wl,-section-start=.MBC5_DSM_1110=0x89c0
LDFLAGS += -Wl,-section-start=.MBC5_DSM_0001=0x8820
LDFLAGS += -Wl,-section-start=.MBC5_DSM_0011=0x8860
LDFLAGS += -Wl,-section-start=.MBC5_DSM_0101=0x88a0
LDFLAGS += -Wl,-section-start=.MBC5_DSM_0111=0x88e0
LDFLAGS += -Wl,-section-start=.MBC5_DSM_1001=0x8920
LDFLAGS += -Wl,-section-start=.MBC5_DSM_1011=0x8960
LDFLAGS += -Wl,-section-start=.MBC5_DSM_1101=0x89a0
LDFLAGS += -Wl,-section-start=.MBC5_DSM_1111=0x89e0
LDFLAGS += -Wl,-section-start=.MBC2_0000=0x8a00
LDFLAGS += -Wl,-section-start=.MBC2_0100=0x8a80
LDFLAGS += -Wl,-section-start=.MBC2_1000=0x8b00
LDFLAGS += -Wl,-section-start=.MBC2_1100=0x8b80
LDFLAGS += -Wl,-section-start=.MBC2_0001=0x8a20
LDFLAGS += -Wl,-section-start=.MBC2_0101=0x8aa0
LDFLAGS += -Wl,-section-start=.MBC2_1001=0x8b20
LDFLAGS += -Wl,-section-start=.MBC2_1101=0x8ba0


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = M644_512KB_FLASH_PROTO.o usart_funcs.o ymodem_funcs.o memory_latched.o terminal_menu.o GameBoy_Info.o eeprom_stuff.o MBC_Modes_V5.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) M644_512KB_FLASH_PROTO.hex M644_512KB_FLASH_PROTO.eep M644_512KB_FLASH_PROTO.lss size

## Compile
M644_512KB_FLASH_PROTO.o: ../M644_512KB_FLASH_PROTO.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart_funcs.o: ../../01_INCLUDE/usart_funcs.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ymodem_funcs.o: ../../01_INCLUDE/ymodem_funcs.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

memory_latched.o: ../memory_latched.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

terminal_menu.o: ../terminal_menu.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

GameBoy_Info.o: ../GameBoy_Info.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

eeprom_stuff.o: ../eeprom_stuff.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

MBC_Modes_V5.o: ../MBC_Modes_V5.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

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
	-rm -rf $(OBJECTS) M644_512KB_FLASH_PROTO.elf dep/* M644_512KB_FLASH_PROTO.hex M644_512KB_FLASH_PROTO.eep M644_512KB_FLASH_PROTO.lss M644_512KB_FLASH_PROTO.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

