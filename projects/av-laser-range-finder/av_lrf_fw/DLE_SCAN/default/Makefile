###############################################################################
# Makefile for the project dle_scan
###############################################################################

## General Flags
PROJECT = dle_scan
MCU = atmega169p
TARGET = dle_ctrl.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -D__AVR_ATmega169P__ -std=gnu99  -save-temps -O2 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=dle_ctrl.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = dle_scan_act.o dle_scan.o dle_scan_lcd.o dle_scan_meas.o dle_scan_keyb.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) dle_ctrl.hex dle_ctrl.eep dle_ctrl.lss size

## Compile
dle_scan_act.o: ../dle_scan_act.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -O2 $<

dle_scan.o: ../dle_scan.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dle_scan_lcd.o: ../dle_scan_lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -O2 $<

dle_scan_meas.o: ../dle_scan_meas.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dle_scan_keyb.o: ../dle_scan_keyb.c
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
	-rm -rf $(OBJECTS) dle_ctrl.elf dep/* dle_ctrl.hex dle_ctrl.eep dle_ctrl.lss dle_ctrl.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

