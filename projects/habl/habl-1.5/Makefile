# makefile, written by guido socher
# downloaded from http://linuxfocus.org/English/November2004/article352.shtml
# modified 2005-02-16 by Henrik Björkman to fit my application

PROJ_NAME=habl

HEADER_FILES=avr_cfg.h avr_uart.h xmodem.h version.h
OBJECT_FILES=habl.o avr_uart.o xmodem.o



# Change here if you have another atmega microcontroller (MCU)
#MCU=atmega88
#MCU=atmega8
MCU=atmega168


# Size of boot loader section.
#   You may need to adjust this for:
#   atmega8 and 88 with 512 word boot section it shall be 0x01C00.
#   atmega8 and 88 with 1024 word boot section its probably  0x01800 (I have not tested this)
#   atmega168 with 512 word boot section it shall be 0x03C00
#   atmega168 with 1024 word boot section its probably  0x03800 (I have not tested this)
# This must match the fuse bits you set, see avrdude_usb_atmega168.txt
START_ADRESS_OF_BOOT_SECTION=0x03C00


# Some generic macros
CC=avr-gcc
OBJCOPY=avr-objcopy

# Give compiler options here
# -Wall = all warnings on
# -Werror = Treat warnings as errors
# -Os = optimize for size
# --section-start = Where to put the program in memory.
CFLAGS=-g -mmcu=$(MCU) -Wall -Werror -Wstrict-prototypes -Os -mcall-prologues \
       -Wl,--section-start=.text=$(START_ADRESS_OF_BOOT_SECTION)



#-------------------
all: $(PROJ_NAME).hex

#-------------------
help: 
	@echo "Usage: make [all|clean]"
	@echo ""
	@echo ""
	@echo ""


#-------------------
$(PROJ_NAME).hex : $(PROJ_NAME).out 
	$(OBJCOPY) -R .eeprom -O ihex $(PROJ_NAME).out $(PROJ_NAME).hex 
$(PROJ_NAME).out : $(OBJECT_FILES)
	$(CC) $(CFLAGS) -o $(PROJ_NAME).out -Wl,-Map,$(PROJ_NAME).map $(OBJECT_FILES)

%.o : %.c $(HEADER_FILES) 
	$(CC) $(CFLAGS) -c $< -o $@





#-------------------
clean:
	rm -f *.o *.map *.out *.hex
#-------------------
