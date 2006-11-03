# $Id: Makefile,v 1.3 2006/10/22 18:20:19 nemo Exp $ 
# generic avr makefile

CC=/usr/bin/avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=attiny26
OBJ2HEX=/usr/bin/avr-objcopy
UISP=/usr/bin/uisp
TARGET=gloglobe

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@ 

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

program : $(TARGET).hex
	$(UISP) -dprog=stk500 -dserial=/dev/ttyS0 --erase -dpart=attiny26
	$(UISP) -dprog=stk500 -dserial=/dev/ttyS0 --upload -dpart=attiny26 \
		if=$(TARGET).hex -v=2

#do not uncomment unless you are sure this is what you want!  
#with only an ISP this can brick your AVR by fooling with the fuses
#	$(UI_oops_SP) -dprog=stk500 -dserial=/dev/ttyS0 -dpart=atmega32 \
                --wr_fuse_l=0xee
clean :
	rm -f *.hex *.obj *.o
