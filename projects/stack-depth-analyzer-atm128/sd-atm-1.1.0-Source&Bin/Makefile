#/********************************************************************************/
#/*  sd-atm, John Kim, john-k.home@rcn.com,  October 2007                        */
#/*   Stack depth analyzer, works for avr atm128.                                */
#/*   Free code.                                                                 */
#/*   Compiles on Windows, MSYS, MinGW, and "gcc (GCC) 3.4.2 (mingw-special)"    */
#/*                                                                              */
#/********************************************************************************/

# Reference: C:\MSYS\home\johnk\avrgcc\use skip\binutils-2.16.1\binutils

ROOT = c:/MinGW
BINS = $(ROOT)/bin
SHELL = $(ROOT)/bin/sh

CC = $(BINS)/gcc
CPP = $(BINS)/gcc -E
VERSION = 2.16.1

TARGET = sd-atm

WARN_CFLAGS = -W -Wall -Wstrict-prototypes

INCLUDES = -D_GNU_SOURCE -I. -D__USE_MINGW_FSEEK


###


LDFLAGS = 
CFLAGS = -g -O2 

COMPILE = $(CC) $(INCLUDES) $(WARN_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = --mode=link $(CCLD) $(WARN_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@

SOURCES = $(TARGET).c decode.c process.c
OBJECTS = $(TARGET).o decode.o process.o
DEPENDENCIES = *.h $(SOURCES)

*.o : *.h

.c.o:
	$(COMPILE) -c $<

$(TARGET).exe: $(OBJECTS) $(DEPENDENCIES) Makefile
	@rm -f $(TARGET).exe
	$(LINK) $(OBJECTS) $(LIBS)


all: $(TARGET).exe

clean:
	rm *.o
	rm *.exe



# IF YOU PUT ANYTHING HERE IT WILL GO AWAY

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
