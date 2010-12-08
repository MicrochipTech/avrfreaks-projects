# makefile, written by angelo dureghello

MCU=atmega88p
CC=avr-gcc
OBJCOPY=avr-objcopy

PROJ=airon
PROJDIR=$(CURDIR)

INCDIR=-I$(PROJDIR)/include
SRCDIR=$(PROJDIR)/src
OBJDIR=$(PROJDIR)/obj
BINDIR=$(PROJDIR)/bin

### binary file output
BINARY=$(BINDIR)/$(PROJ)
HEX=$(BINDIR)/$(PROJ).hex

# optimize for size:
CFLAGS= -c -std=c99 -mmcu=$(MCU) -DF_CPU=8000000 -Os -Wall -fdata-sections -ffunction-sections -Wstrict-prototypes

### linker flags
LDFLAGS=-mmcu=$(MCU) -Wl,--relax,--gc-sections,-Map,airon.map

### files name preparing macroes
SRCS:= $(wildcard $(SRCDIR)/*.c)
OBJS:= $(patsubst %.c,%.o,$(SRCS))
OBJS:= $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(OBJS))

debug:
	make all

all: $(HEX)

$(HEX): $(BINARY)
		$(OBJCOPY) -R .eeprom -O ihex $< $@
$(BINARY): $(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.c
		$(CC) $(CFLAGS) $(INCDIR) $< -o $@

# you need to erase first before loading the program.
# load (program) the software into the eeprom:
burn: $(HEX)
	sudo avrdude -c avrispmkII -P usb -p m88 -F -i 22 -V -U flash:w:bin/airon.hex
	sudo avrdude -c avrispmkII -P usb -p m88 -F -i 22 -V -U lfuse:w:0xe2:m


cleandebug:
	make clean

clean:
	rm -f obj/*.o bin/*
