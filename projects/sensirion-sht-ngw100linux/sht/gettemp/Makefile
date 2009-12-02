CROSS_COMPILE		:= avr32-linux-
CC			:= $(CROSS_COMPILE)gcc

CPPFLAGS		:= -D_GNU_SOURCE
CFLAGS			:= -pipe -O2 -g -Wall

all: gettemp gettemp-cgi

gettemp: gettemp.o
	$(CC) $(CFLAGS) -o $@ $^
	
gettemp-cgi: gettemp-cgi.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	$(RM) *.o *~ gettemp gettemp-cgi
