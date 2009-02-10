
# Makefile for avrlirc and avrlirc2udp
# Paul Fox, April 2007

# current code assumes ATTiny2313.  (it uses UART, 16 bit timer, and
# input capture event interrupt.)
MCU = attiny2313

# location of cross-compiler -- edit to suit
CROSS = /opt/avr-gcc-070314/bin/avr-

CC=$(CROSS)gcc
LD=$(CROSS)gcc
NM=$(CROSS)nm
SIZE=$(CROSS)size
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump

CFLAGS = -c -Os -Wwrite-strings -Wall -mmcu=$(MCU)
CFLAGS += -Wa,-adhlns=$(<:%.c=%.lst)
LFLAGS = -mmcu=$(MCU)

HOSTCC = gcc

PROG = avrlirc
SRCS = avrlirc.c
OBJS = avrlirc.o

VERSION = $(shell date +%y%m%d-%H%M)
CFLAGS += -DAVRLIRC_VERSION="\"$(VERSION)\""


all: $(PROG).hex $(PROG).lss avrlirc2udp 

$(PROG).out: $(OBJS)
	@-test -f $(PROG).out && (echo size was: ; $(SIZE) $(PROG).out)
	$(LD) -o $@ $(LFLAGS) $(OBJS)
	$(NM) -n $@ >$(PROG).map
	@echo size is:
	@$(SIZE) $(PROG).out

$(PROG).hex: $(PROG).out
	$(OBJCOPY) -R .eeprom -O ihex $^ $@

# Create extended listing file from ELF output file.
%.lss: %.out
	$(OBJDUMP) -h -S $< > $@


sizes: $(OBJS)
	@echo
	@echo Individual:
	$(SIZE) -t $(OBJS)
	@echo
	@echo Complete:
	$(SIZE) $(PROG).out

avrlirc2udp: avrlirc2udp.c
	$(HOSTCC) -Wall avrlirc2udp.c -o avrlirc2udp
 
tarball: all clean
	mkdir -p oldfiles
	mv $(PROG)-*.hex *.tar.gz oldfiles || true
	mv $(PROG).hex $(PROG)-$(VERSION).hex || true
	ln -s avrlirc ../avrlirc-$(VERSION)
	tar -C .. --dereference \
	    --exclude CVS \
	    --exclude oldfiles \
	    --exclude web \
	    --exclude '*.tar.gz' \
	    -czf ../avrlirc-$(VERSION).tar.gz avrlirc-$(VERSION)
	mv ../avrlirc-$(VERSION).tar.gz .
	rm -f ../avrlirc-$(VERSION)

foo.out: foo.o
	$(LD) -o $@ $(LFLAGS) foo.o
	$(NM) -n $@ >foo.map

clean:
	rm -f *.o *.flash *.flash.* *.out *.map *.lst *.lss avrlirc2udp
	
clobber: clean
	rm -f avrlirc.hex

