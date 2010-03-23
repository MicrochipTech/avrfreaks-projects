#############################################################################
# This is the makefile for the tinyserial library.
#############################################################################

include ../configuration.mk

CFLAGS= -std=gnu99 -Wall -mmcu=$(MCU)
ARFLAGS= rcs

all : libTinySerial.a

tinyserial_init.o : tinyserial_init.S tinyserial.h
	$(CC) $(CFLAGS) -c tinyserial_init.S

tinyserial_putc.o : tinyserial_putc.S tinyserial.h
	$(CC) $(CFLAGS) -c tinyserial_putc.S

tinyserial_getc.o : tinyserial_getc.S tinyserial.h
	$(CC) $(CFLAGS) -c tinyserial_getc.S

tinyserial_puts.o : tinyserial_puts.S tinyserial.h tinyserial_putc.o
	$(CC) $(CFLAGS) -c tinyserial_puts.S

tinyserial_gets.o : tinyserial_gets.S tinyserial.h tinyserial_getc.o
	$(CC) $(CFLAGS) -c tinyserial_gets.S

tinyserial_putsf.o : tinyserial_putsf.S tinyserial.h tinyserial_putc.o
	$(CC) $(CFLAGS) -c tinyserial_putsf.S

libTinySerial.a : tinyserial_init.o tinyserial_putc.o tinyserial_getc.o tinyserial_puts.o tinyserial_gets.o tinyserial_putsf.o
	$(AR) $(ARFLAGS) libTinySerial.a tinyserial_init.o tinyserial_putc.o tinyserial_getc.o tinyserial_puts.o tinyserial_gets.o tinyserial_putsf.o

clean :
	$(RM) -f *.o *.a *~

