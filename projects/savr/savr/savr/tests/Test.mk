MCU     := atmega328p
F_CPU   := 16000000

ifneq "$(MAKECMDGOALS)" "clean"
ifndef MCU
$(error MCU not defined)
endif
endif


## General Flags
DIRNAME     = $(shell pwd | sed "s/.*\\///g")
TARGET      = $(DIRNAME).elf
CC          = avr-gcc
AR          = avr-ar
LD          = avr-gcc
CXX         = avr-g++
TARGETS     = $(TARGET) $(TARGET:%.elf=%.lss) $(TARGET:%.elf=%.hex) $(TARGET:%.elf=%.eep)

## Options common to compile, link and assembly rules
LIBNAME     = savr_$(MCU)

## Include/lib Directories
INCLUDES   += -I../../include
LIBS       += -l$(LIBNAME)
LIBDIRS    += -L../../lib

## Options common to compile, link and assembly rules
ARCHFLAGS = -mmcu=$(MCU)
COMMON    = $(ARCHFLAGS) -Wall -g -DF_CPU=$(F_CPU)UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wl,-u,vfprintf -lprintf_flt -lm


## Compile options common for all C compilation units.
CXXFLAGS  = $(COMMON) -std=gnu++98 -fno-exceptions
CXXFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d $(INCLUDES)

CFLAGS    = $(COMMON) -std=gnu99
CFLAGS   += -MD -MP -MT $(*F).o -MF dep/$(@F).d $(INCLUDES)

LDFLAGS   = $(ARCHFLAGS) -Wl,-Map=$(DIRNAME).map,-u,vfprintf -lprintf_flt -lm

## Objects that must be built in order to link
CINPUTS     = $(wildcard *.c)
CXXINPUTS   = $(wildcard *.cpp)
OBJECTS     = $(CINPUTS:%.c=%.o) $(CXXINPUTS:%.cpp=%.o)


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

## Objects explicitly added by the user
LINKONLYOBJECTS = 

.PHONY: clean all size load 
.INTERMEDIATE: $(OBJECTS)

## Build
all: $(TARGETS) size

%.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.a: $(OBJECTS)
	$(AR) -c -r $@ $? $(LIBS)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: $(TARGET)
	@echo
	@avr-size -C -t --mcu=$(MCU) $(TARGET)


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

## Program MCU
load: $(TARGET:%.elf=%.hex)
	$(shell avrdude -cavrisp2 -Pusb -p$(MCU) -U $<)

## Clean target
clean:
	-rm -rf *.o *.a *.lss *.hex *.elf dep *.map *.eep

