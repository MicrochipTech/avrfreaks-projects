# ***************************************************************************
#   Directories
# ***************************************************************************

OBJDIR	= ../../obj/avr
SRCDIR	= ../../source
INCDIR	= ../../include
LIBDIR	= $(AVR)/avr/lib


# ***************************************************************************
#   Standard variables base on AVR base in $(AVR)
# ***************************************************************************
CC	= avr-gcc
AS	= avr-gcc -x assembler-with-cpp	
RM	= rm -f
RN	= mv
BIN	= avr-objcopy
SHELL   = $(AVR)/bin/sh.exe

# ***************************************************************************
#   Standard output fromat, can be either 'srec' or 'ihex'
# ***************************************************************************
FORMAT = srec
MCU = at90s8515

# ***************************************************************************
#   Default flags for compiler, assembler and linker
# ***************************************************************************
CPFLAGS	= -g -O3 -Wall -Wstrict-prototypes -Wa,-ahlms=$(<:.c=.lst)
ASFLAGS = -gstabs
LDFLAGS = -Wl,-Map=$(TRG).map,--cref

# ***************************************************************************
#  All project specific flags
# ***************************************************************************
#
# Due to a hardware bug on my prototype board, I use bit swapping. For other 
# hardware this must be undefined!
#
#CPFLAGS += -mmcu=$(MCU) -D__ATMEL__ -DBITSWAP
CPFLAGS += -mmcu=$(MCU) -D__ATMEL__

ASFLAGS += -mmcu=$(MCU)
LDFLAGS += -mmcu=$(MCU)
   

# ***************************************************************************
# Target file name (without extension!)
# ***************************************************************************
TRG	= slipdump
	
# ***************************************************************************
# C Source files seperated by spaces
# ***************************************************************************
SRC	= slipdump.c 
	
# ***************************************************************************
# Assembler Source files
# ***************************************************************************
ASRC	=

# ***************************************************************************
# Additional libraries and object files
# ***************************************************************************
LIB	= 


# ***************************************************************************
# Object files
# ***************************************************************************
OBJS	= 	$(OBJDIR)/serial_avr.o  \
		$(OBJDIR)/serial.o      \
		$(OBJDIR)/slip.o        \
		$(OBJDIR)/slipdump.o
		
# ***************************************************************************
# Additional include files
# ***************************************************************************
INC	= 

# ***************************************************************************
#
# Make commands, don't edit this section
#
# ***************************************************************************

# ***************************************************************************
# Serial IDE interface (SIDE)
# ***************************************************************************
$(OBJDIR)/slipdump.rom: $(OBJDIR)/slipdump.elf
	$(BIN) -O $(FORMAT) $< $@
	$(BIN) -j .eeprom --set-section-flags=.eeprom="alloc,load" -O $(FORMAT) $< $(@:.rom=.eep)

$(OBJDIR)/slipdump.obj : $(OBJDIR)/slipdump.elf
	$(BIN) -O avrobj $< $@

$(OBJDIR)/slipdump.elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIB)  


# ***************************************************************************
# Object files
# ***************************************************************************
$(OBJDIR)/serial_avr.o : $(SRCDIR)/os/serial_avr.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

$(OBJDIR)/serial.o : $(SRCDIR)/os/serial.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

$(OBJDIR)/slip.o : $(SRCDIR)/slip.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

$(OBJDIR)/timer_avr.o : $(SRCDIR)/os/timer_avr.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

$(OBJDIR)/ide_avr.o : $(SRCDIR)/os/ide_avr.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

$(OBJDIR)/ide.o : $(SRCDIR)/ide.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

$(OBJDIR)/ata.o : $(SRCDIR)/ata.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

$(OBJDIR)/slipdump.o : slipdump.c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@


# ***************************************************************************
# End of makefile
# ***************************************************************************



# ***************************************************************************
#  Target definitions
# ***************************************************************************

all:	$(TRG).obj $(TRG).rom

serial:	$(OBJDIR)/serial.o $(OBJDIR)/serial_avr.o 

slipdump:	$(OBJDIR)/slipdump.o

timer:  $(OBJDIR)/timer_avr.o


clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(OBJDIR)/*.rom
	$(RM) $(OBJDIR)/*.elf
	$(RM) $(OBJDIR)/*.eep
	


# ***************************************************************************
#  compile: create assembler and/or object files from C source
# ***************************************************************************
.c.{$(OBJDIR)}.o::
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $(OBJDIR)/$@
#
#.SUFFIXES:	.c .o


%s : %c
	$(CC) -S $(CPFLAGS) -I$(INCDIR) $< -o $@

# ***************************************************************************
# assemble: create object file from assembler files
# ***************************************************************************
%o : %s
	$(AS) $(ASFLAGS) -I$(INCDIR) $< -o $@

# ***************************************************************************
# link: create elf and rom output file from object files
# ***************************************************************************
%elf: $(OBJS)
	$(CC) $(OBJS) $(LIB) $(LDFLAGS) -o $@

%obj: %elf
	$(BIN) -O avrobj $< $@





