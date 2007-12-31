MCU_TARGET     	= atmega168
OPTIMIZE       	= -O2

PRG             = rc_tx

OBJ             = rc_tx.o \
		  rc_hmi.o \
		  rc_timer.o \
		  ../common/rc_rf.o \
		  ../common/rc_twi.o

ifdef DEBUG
OBJ		+= ../common/rc_serial.o
DEFS		= -g -D_DEBUG
endif

DEFS           	+= -Iinclude \
		   -I../common/include

LIBS           	=


# Programming
AVRDUDE 		= avrdude
AVRDUDE_MCU 		= m168
AVRDUDE_PROGRAMMER 	= stk500
AVRDUDE_PORT 		= COM4
AVRDUDE_UPLOAD_RATE 	= 19200
AVRDUDE_WRITE_FLASH 	= -U flash:w:$(PRG).hex
AVRDUDE_FLAGS 		= -F -p $(AVRDUDE_MCU) -P $(AVRDUDE_PORT) \
			  -c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_UPLOAD_RATE)

# You should not have to change anything below here.

CC             = avr-gcc
OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

# Override is only needed by avr-lib build system.
override CFLAGS        = -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

all: $(PRG).elf lst text eeprom 

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# Program the device.  
upload: $(PRG).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

clean:
	rm -rf $(OBJ) $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
	|| { echo empty $@ not generated; exit 0; }

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@

