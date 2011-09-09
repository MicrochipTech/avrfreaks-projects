###############################################################################
# Makefile for the project ImuxVS4
###############################################################################

## General Flags
PROJECT = ImuxVS4
MCU = atxmega128a1
TARGET = ImuxVS4.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99              -DF_CPU=32000000UL -O1 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=ImuxVS4.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"I:\IMUX\TestIMUX\TestIMUX\src" -I"I:\IMUX\TestIMUX\TestIMUX\src\config" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\boards" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\services" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\services\clock" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\utils" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\services\serial" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\services\sleepmgr" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\utils\interrupt" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\adc" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\cpu" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\ioport" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\nvm" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\rtc" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\sleep" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\spi" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\tc" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\twi" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\usart" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\utils" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\utils\assembler" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\utils\bit_handling" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\utils\preprocessor" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\boards\user_board" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\services\clock\xmega" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\services\serial\xmega_usart" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\common\services\sleepmgr\xmega" -I"C:\Program Files\Atmel\AVR Studio 5.0\AVR Toolchain\avr\include\avr" -I"I:\IMUX\TestIMUX\TestIMUX\src\asf\xmega\drivers\pmic" 

## Objects that must be built in order to link
OBJECTS = Drivers.o main.o init.o sysclk.o usart_serial.o sleepmgr.o adc.o ccp.o ioport.o nvm.o nvm_asm.o rtc.o spi.o tc.o twim.o twis.o usart.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) ImuxVS4.hex ImuxVS4.eep ImuxVS4.lss size

## Compile
Drivers.o: ../src/Drivers.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../src/main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

init.o: ../src/asf/common/boards/user_board/init.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sysclk.o: ../src/asf/common/services/clock/xmega/sysclk.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart_serial.o: ../src/asf/common/services/serial/usart_serial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sleepmgr.o: ../src/asf/common/services/sleepmgr/xmega/sleepmgr.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

adc.o: ../src/asf/xmega/drivers/adc/adc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ccp.o: ../src/asf/xmega/drivers/cpu/ccp.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

ioport.o: ../src/asf/xmega/drivers/ioport/ioport.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

nvm.o: ../src/asf/xmega/drivers/nvm/nvm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

nvm_asm.o: ../src/asf/xmega/drivers/nvm/nvm_asm.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

rtc.o: ../src/asf/xmega/drivers/rtc/rtc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi.o: ../src/asf/xmega/drivers/spi/spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tc.o: ../src/asf/xmega/drivers/tc/tc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

twim.o: ../src/asf/xmega/drivers/twi/twim.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

twis.o: ../src/asf/xmega/drivers/twi/twis.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart.o: ../src/asf/xmega/drivers/usart/usart.c
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
	-rm -rf $(OBJECTS) ImuxVS4.elf dep/* ImuxVS4.hex ImuxVS4.eep ImuxVS4.lss ImuxVS4.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

