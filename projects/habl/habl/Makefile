# makefile, written by guido socher
# downloaded from http://linuxfocus.org/English/November2004/article352.shtml
# modified 2005-02-16 by Henrik Björkman to fit my application

PROJ_NAME=habl

HEADER_FILES=avr_cfg.h avr_uart.h xmodem.h
OBJECT_FILES=habl.o avr_uart.o xmodem.o



# Change here if you have another atmega microcontroller (MCU)
MCU=atmega88
#MCU=atmega8

# Some generic macros
CC=avr-gcc
OBJCOPY=avr-objcopy

# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues \
       -Wl,--section-start=.text=0x01C00



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
