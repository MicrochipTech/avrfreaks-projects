MCU_TARGET		= atmega8
#MCU_TARGET		= at90s8515
AVRDUDE_DEV		= m8
#AVRDUDE_DEV	= 8515
AVRDUDE_PROG	= stk200
OPTIMIZE		= -Os

DEFS			=  -DUSE_UART0
#DEFS			+=  -DUSE_UART1

UART_LIB_DIR	= ..

BINFORMAT		= hex

DEFS			+= -DF_CPU=4000000

####
# You should not have to change anything below here.

UART_LIB		= libuart_$(MCU_TARGET).a

EX_SIMPLE_PRG	= example_simple
EX_IO_PRG		= example_io
EX_ECHO_PRG		= example_echo
LOOP_PRG		= loop

PROGS			= $(EX_SIMPLE_PRG) $(EX_IO_PRG) $(EX_ECHO_PRG) $(LOOP_PRG)

CC	     	= avr-gcc
CPP	     	= avr-cpp
OBJCOPY		= avr-objcopy
OBJDUMP		= avr-objdump
STRIP		= avr-strip

CFLAGS		= -std=gnu99 -pedantic -Wall $(OPTIMIZE) -gdwarf-2 -mmcu=$(MCU_TARGET) $(DEFS) -I$(UART_LIB_DIR)
LDFLAGS 	= $(UART_LIB_DIR)/$(UART_LIB)
CPPFLAGS    = -std=gnu99 $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -I$(UART_LIB_DIR)


.PHONY: all text lst clean $(PROGS)

all: $(PROGS) text lst

$(EX_SIMPLE_PRG) : $(UART_LIB_DIR)/$(UART_LIB) $(EX_SIMPLE_PRG).elf

$(EX_IO_PRG) : $(UART_LIB_DIR)/$(UART_LIB) $(EX_IO_PRG).elf

$(LOOP_PRG) : $(UART_LIB_DIR)/$(UART_LIB) $(LOOP_PRG).elf


%.d: %.c
	@set -e; $(CPP) -MM $(CPPFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

%.elf: %.o
	$(CC) -Wl,-Map,$(@:.elf=.map),-u,vfprintf -lprintf_min -o $@ $^ $(LDFLAGS)

clean:
	-@rm -f *.o *.elf *~
	-@rm -f *.lst *.map *.bin *.srec *.hex

prog_ex_echo: $(EX_ECHO_PRG)
	avrdude -p $(AVRDUDE_DEV) -c $(AVRDUDE_PROG) -U flash:w:$(EX_ECHO_PRG).$(BINFORMAT)  -y

prog_ex_simple: $(EX_SIMPLE_PRG)
	avrdude -p $(AVRDUDE_DEV) -c $(AVRDUDE_PROG) -U flash:w:$(EX_SIMPLE_PRG).$(BINFORMAT)  -y

prog_ex_io: $(EX_IO_PRG)
	avrdude -p $(AVRDUDE_DEV) -c $(AVRDUDE_PROG) -U flash:w:$(EX_IO_PRG).$(BINFORMAT)  -y

prog_loop: $(LOOP_PRG)
	avrdude -p $(AVRDUDE_DEV) -c $(AVRDUDE_PROG) -U flash:w:$(LOOP_PRG).$(BINFORMAT) -y


lst:  $(addsuffix .lst,$(PROGS))


%.lst: %.elf
	$(OBJDUMP) -h -S -D $< > $@


# Rules for building the .text FLASH images

text: $(BINFORMAT)

hex:  $(addsuffix .hex,$(PROGS))
bin:  $(addsuffix .bin,$(PROGS))
srec: $(addsuffix .srec,$(PROGS))

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@


DSOURCES = $(addsuffix .d,$(PROGS))
include $(DSOURCES)
