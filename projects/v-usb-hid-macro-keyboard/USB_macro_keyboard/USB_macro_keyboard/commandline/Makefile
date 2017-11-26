# Name: Makefile
# Project: Macro_keyboard
# Creation Date: 2017-11-25
# Tabsize: 4

# Change the following definitions according to your system.

# Use the following 3 lines on Unix (uncomment the framework on Mac OS X):
USBFLAGS = `pkg-config --cflags libusb-1.0`
USBLIBS = `pkg-config --libs libusb-1.0`
EXE_SUFFIX =

NAME = macro_keyboard

OBJECTS = $(NAME).o

CC		= g++
CFLAGS	= $(CPPFLAGS) $(USBFLAGS) -O2 -Wall -s -std=c++11
LIBS	= $(USBLIBS)

PROGRAM = $(NAME)$(EXE_SUFFIX)


all: $(PROGRAM)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

$(PROGRAM): $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LIBS)

strip: $(PROGRAM)
	strip $(PROGRAM)

clean:
	rm -f *.o $(PROGRAM)
