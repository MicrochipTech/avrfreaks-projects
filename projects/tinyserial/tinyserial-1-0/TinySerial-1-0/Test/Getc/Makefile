#############################################################################
# This is a makefile for a Tinyserial test program.
#############################################################################

include ../../configuration.mk

TARGET= testgetc

CFLAGS= -std=gnu99 -Wall -mmcu=$(MCU) -I../../Lib
OFLAGS= -O ihex -R .eeprom
AFLAGS= -V -p $(MCU) -P $(PORT) -b $(UPLOAD_RATE) -c $(AVRDUDE_PROGRAMMER) -C $(AVRDUDE_CONFIG)

all : $(TARGET).ihex

$(TARGET).ihex : $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET).elf  $(TARGET).c ../../Lib/libtinyserial.a
	$(OBJCOPY) $(OFLAGS) $(TARGET).elf $(TARGET).ihex

upload : $(TARGET).ihex
	$(AVRDUDE) $(AFLAGS) -U flash:w:$(TARGET).ihex

clean :
	$(RM) -f $(TARGET).ihex $(TARGET).elf *~
