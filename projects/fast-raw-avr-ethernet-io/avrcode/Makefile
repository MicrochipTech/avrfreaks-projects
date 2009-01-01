MCU=atmega168
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
#-------------------
.PHONY: ethcomavr all
#
all: ethcomavr.hex 
#
ethcomavr: ethcomavr.hex
#-------------------
help: 
	@echo "Usage: make all|ethcomavr|load_ethcomavr"
	@echo "or"
	@echo "Usage: make clean"
	@echo " "
#-------------------
enc28j60.o : enc28j60.c enc28j60.h
	$(CC) $(CFLAGS) -Os -c enc28j60.c
#------------------
ethcomavr.hex : ethcomavr.out 
	$(OBJCOPY) -R .eeprom -O ihex ethcomavr.out ethcomavr.hex 
	avr-size ethcomavr.out
	avr-objdump -S -r ethcomavr.out > ethcomavr.lst
ethcomavr.out : ethcomavr.o enc28j60.o
	$(CC) $(CFLAGS) -o ethcomavr.out -Wl,-Map,ethcomavr.map ethcomavr.o enc28j60.o
ethcomavr.o : ethcomavr.c enc28j60.h
	$(CC) $(CFLAGS) -Os -c ethcomavr.c
#------------------
load_ethcomavr: ethcomavr.hex
	./prg_load_uc_168 ethcomavr.hex
#-------------------
clean:
	rm -f *.o *.map *.out *.hex
#-------------------
