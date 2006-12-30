#MCU_TARGET		= atmega8
MCU_TARGET		= at90s8515
#AVRDUDE_DEV		= m8
AVRDUDE_DEV		= 8515
AVRDUDE_PROG	= stk200

BINFORMAT		= hex
DEFS			+= -DF_CPU=4000000

SST_SRC_DIR		= ../$(SST_DIR)
SST_SRCS		= $(addprefix $(SST_SRC_DIR)/, $(SST_SOURCES))
SST_OBJS		= $(SST_SOURCES:.c=.o)
AVR_SOURCES		= avr_example.c
TEST_OBJS		= $(MAIN_TEST).o $(RB_SOURCES:.c=.o) $(AVR_SOURCES:.c=.o) $(SST_OBJS)
MUTEX_OBJS		= $(MUTEX_TEST).o $(RB_SOURCES:.c=.o) $(AVR_SOURCES:.c=.o) $(SST_OBJS)
INC				= -I$(SST_SRC_DIR) -I../$(EXAMPLES_DIR) -I.

PROGS			= $(MAIN_TEST) $(MUTEX_TEST)

COMMAND_PREFIX = avr-
CC           = $(COMMAND_PREFIX)gcc
LD           = $(CC)
AR           = $(COMMAND_PREFIX)ar
RANLIB       = $(COMMAND_PREFIX)ranlib
OBJDUMP      = $(COMMAND_PREFIX)objdump
OBJCOPY      = $(COMMAND_PREFIX)objcopy
STRIP        = $(COMMAND_PREFIX)strip

CFLAGS		= $(C_STD) $(WARN_LEVEL) $(OPTIMIZE) -g
CFLAGS		+= -mmcu=$(MCU_TARGET) $(DEFS) $(INC)
LDFLAGS 	= -g $(SST_OBJS) $(AVR_SOURCES:.c=.o) $(RB_SOURCES:.c=.o)
CPPFLAGS    = $(C_STD) -mmcu=$(MCU_TARGET) $(DEFS) $(INC)

vpath %.c $(SST_SRC_DIR):../$(EXAMPLES_DIR)

.PHONY: all text lst clean $(PROGS)

all: $(PROGS) text lst

$(MAIN_TEST) : $(TEST_OBJS) $(MAIN_TEST).elf

$(MUTEX_TEST) : $(MUTEX_OBJS) $(MUTEX_TEST).elf

%o: %c
	$(CC) $(CFLAGS) $< -c -o $@

%.d: %.c
	@set -e; $(CPP) -MM $(CPPFLAGS) $< \
	| sed 's=\($*\)\.o[ :]*=\1.o $@ : =g' > $@; \
	[ -s $@ ] || rm -f $@

%.elf: %.o
	$(CC) -Wl,-Map,$(@:.elf=.map) -o $@ $^ $(LDFLAGS)

#$(CC) -Wl,-Map,$(@:.elf=.map),-u,vfprintf -lprintf_min -o $@ $^ $(LDFLAGS)

clean:
	-@rm -f *\.d *\.o *\.elf *~
	-@rm -f *\.lst *\.map *\.bin *\.srec *\.hex

main_prog: $(MAIN_TEST)
	avrdude -p $(AVRDUDE_DEV) -c $(AVRDUDE_PROG) -U flash:w:$(MAIN_TEST).$(BINFORMAT) -y

mutex_prog: $(MUTEX_TEST)
	avrdude -p $(AVRDUDE_DEV) -c $(AVRDUDE_PROG) -U flash:w:$(MUTEX_TEST).$(BINFORMAT) -y

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


DSOURCES =	$(MAIN_TEST).d $(MUTEX_TEST).d $(AVR_SOURCES:.c=.d) \
			$(SST_SOURCES:.c=.d) $(RB_SOURCES:.c=.d)
-include $(DSOURCES)
