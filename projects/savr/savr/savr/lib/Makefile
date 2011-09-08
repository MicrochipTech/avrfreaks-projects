ifneq "$(MAKECMDGOALS)" "clean"
ifndef MCU
$(error MCU not defined)
endif
endif


## General Flags
DIRNAME     = $(shell pwd | sed "s/.*\\///g")
TARGET      = libsavr_$(MCU).a
CC          = avr-gcc
AR          = avr-ar
CXX         = avr-g++
LSS_TARGET  = $(TARGET:%.a=%.lss)

## Options common to compile, link and assembly rules
COMMON  = -mmcu=$(MCU) -Wextra -g -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

## Include Directories
INCLUDES += -I../include

## Compile options common for all C compilation units.
CXXFLAGS  = $(COMMON) -std=gnu++98 -fno-exceptions
CXXFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d $(INCLUDES)

CFLAGS    = $(COMMON) -std=gnu99
CFLAGS   += -MD -MP -MT $(*F).o -MF dep/$(@F).d $(INCLUDES)

## Objects that must be built in order to link
CINPUTS     = $(wildcard *.c)
CXXINPUTS   = $(wildcard *.cpp)
OBJECTS     = $(CINPUTS:%.c=%.o) $(CXXINPUTS:%.cpp=%.o)

## Objects explicitly added by the user
LINKONLYOBJECTS = 

.PHONY: clean all size
.INTERMEDIATE: $(OBJECTS)

## Build
all: $(TARGET) $(LSS_TARGET) size

##Link
$(TARGET): $(OBJECTS)
	$(AR) -c -r $@ $? $(LIBS)

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: $(TARGET)
	@echo
	@avr-size -B -t --mcu=$(MCU) $(TARGET)

## Clean target
clean:
	-rm -rf *.o *.a *.lss dep


ifneq "$(MAKECMDGOALS)" "clean"
## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)
endif
