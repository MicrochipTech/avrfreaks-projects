# Hey Emacs, this is a -*- makefile -*-

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend

MCU = atmega16
#MCU = attiny45
F_CPU = 8000000
FORMAT = ihex
TARGET = main
SRC = $(TARGET).c lcd.c ps2_kb.c ps2_low.c flash_utils.c menu_utils.c
OPT = s

CSTANDARD = -std=gnu99
CDEFS = -DF_CPU=$(F_CPU)UL
CDEBUG = -g
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)
ALL_CFLAGS = -mmcu=$(MCU) $(CDEBUG) $(CINCS) $(CDEFS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CTUNING)


LDFLAGS =  -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
#LDFLAGS += -llcd -ldevboard -lrtc
LDFLAGS += -Wl,--section-start=.text=0x0000
#new section: bootloader - starting at byte address 0x3C00 
#note that datasheet gives word address (here, 0x1E00) for different bootloader sections)
LDFLAGS += -Wl,--section-start=.bootloader=0x3C00

####### AVRDUDE #######

AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PORT = usb
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)


###### COMPILE AND LINK ########
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f
SIZE = avr-size

# Define all object files.
OBJ = $(SRC:.c=.o)  


# Define all listing files.
LST = $(SRC:.c=.lst)


# Default target.
all: elf hex eep lss size

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym

size: 
	$(SIZE) --mcu=$(MCU) --format=avr $(TARGET).elf

# Program the device.  
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

.SUFFIXES: .elf .hex .eep .lss .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@


# Link:
# create ELF output file from object files.
$(TARGET).elf: $(OBJ)
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)


# Compile: 
# create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Target: clean project.
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d)
