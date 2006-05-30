#MCU_TARGET	= 	your_avr_here
MCU_TARGET	= 	atmega8

DEFS		= 	-DUSE_UART0
#DEFS		+= 	-DUSE_UART1
DEFS		+=	-DUART0_TX_BUFSIZE=8 -DUART0_RX_BUFSIZE=8
#DEFS		+=	-DUART1_TX_BUFSIZE=8 -DUART1_RX_BUFSIZE=8
#DEFS		+=	-DSKIP_DEVNO_CHECK

OPTIMIZE	= 	-Os

#
# You should not have to change anything below here.
#

LIB_C_SRC = uart_util_puts.c uart_util_gets.c uart_util_gets_delim.c \
			uart_util_sleep_tx.c uart_util_sleep_rx.c
LIB_A_SRC = uart.S uart_rx.S uart_tx.S uart_util_puts_P.S uart_util_tx_len.S \
	    	uart_util_rx_len.S uart_put.S uart_get.S \
			uart_util_tx_empty.S uart_util_rx_wait.S

HEADERS = uart_guard.h uart.h uart_util.h

LIB_OBJ		= $(LIB_A_SRC:.S=.o) $(LIB_C_SRC:.c=.o)
LIB			= uart_$(MCU_TARGET)
LIBFILE		= lib$(LIB).a

ASFLAGS		= -c -mmcu=$(MCU_TARGET) -gdwarf-2 -DMCU=$(MCU_TARGET) -Wa $(DEFS)
CFLAGS		= -std=gnu99 -pedantic -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -gdwarf-2 $(DEFS) -I.
CPPFLAGS    = $(OPTIMIZE) -std=gnu99 -mmcu=$(MCU_TARGET) $(DEFS) -I.

AS		= avr-gcc -x assembler-with-cpp
CC		= avr-gcc
CPP		= avr-cpp
AR		= avr-ar
RANLIB		= avr-ranlib
OBJDUMP		= avr-objdump
OBJCOPY		= avr-objcopy

.PHONY: all clean doc


all: Makefile $(LIBFILE)
	
doc: uart.dox main.dox $(HEADERS)
	doxygen uart.dox

%o: %S
	$(AS) $(ASFLAGS) $< -o $@

%.d: %.c
	set -e; $(CPP) -MM $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

%.d: %.S
	set -e; $(CPP) -MM $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@


$(LIBFILE): $(LIBFILE)($(LIB_OBJ))
	$(RANLIB) $@


clean:
	-@rm -rf *.o *~ $(LIBFILE) *.lst *.map $(EXTRA_CLEAN_FILES)


lst: $(LIB_OBJ:%.o=%.lst)
	

%.lst: %.o
	$(OBJDUMP) -h -S $< > $@


DSOURCES = $(LIB_C_SRC:.c=.d) $(LIB_A_SRC:.S=.d)
include $(DSOURCES)


#EOF
