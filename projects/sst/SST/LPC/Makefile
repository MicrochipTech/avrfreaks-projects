MCU_TARGET		= arm7tdmi-s

DEFS		+= -DCPU_ARM_LPC2114

SST_SRC_DIR		= ../$(SST_DIR)
SST_SRCS		= $(addprefix $(SST_SRC_DIR)/, $(SST_SOURCES))
SST_OBJS		= $(SST_SOURCES:.c=.o)
LPC_SOURCES		= arm_example.c
LPC_STARTUP		= startup.S
TEST_OBJS		= $(MAIN_TEST).o $(LPC_STARTUP:.S=.o) $(RB_SOURCES:.c=.o) $(LPC_SOURCES:.c=.o) $(SST_OBJS)
MUTEX_OBJS		= $(MUTEX_TEST).o $(LPC_STARTUP:.S=.o) $(RB_SOURCES:.c=.o) $(LPC_SOURCES:.c=.o) $(SST_OBJS)
INC				= -I$(SST_SRC_DIR) -I../$(EXAMPLES_DIR) -I.

PROGS			= $(MAIN_TEST) $(MUTEX_TEST)

COMMAND_PREFIX = arm-elf-
CC           = $(COMMAND_PREFIX)gcc
LD           = $(CC)
AR           = $(COMMAND_PREFIX)ar
RANLIB       = $(COMMAND_PREFIX)ranlib
OBJDUMP      = $(COMMAND_PREFIX)objdump
OBJCOPY      = $(COMMAND_PREFIX)objcopy
STRIP        = $(COMMAND_PREFIX)strip

CFLAGS		= $(C_STD) $(WARN_LEVEL) $(OPTIMIZE) -g
CFLAGS 		+= -mcpu=$(MCU_TARGET) $(DEFS) $(INC)
ASFLAGS		= -mcpu=$(MCU_TARGET) -D__ASSEMBLER__ -g $(DEFS) $(INC)
CPPFLAGS	= $(C_STD) -mcpu=$(MCU_TARGET) $(DEFS) $(INC)
CPPFLAGS_AS	= -mcpu=$(MCU_TARGET) $(WARN_LEVEL) -D__ASSEMBLER__ $(DEFS) $(INC)
LDFLAGS		= -mcpu=$(MCU_TARGET) -nostdlib -Tlpc2114_flash.ld \
			$(LPC_STARTUP:.S=.o) $(SST_OBJS) $(LPC_SOURCES:.c=.o) $(RB_SOURCES:.c=.o)

vpath %.c $(SST_SRC_DIR):../$(EXAMPLES_DIR)

.PHONY: all hex lst clean $(PROGS)

all: $(PROGS) hex lst

$(MAIN_TEST) : $(TEST_OBJS) $(MAIN_TEST).elf

$(MUTEX_TEST) : $(MUTEX_OBJS) $(MUTEX_TEST).elf

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<
#	$(OBJDUMP) -h -S $@ > $@.lst

%.o: %.S
	$(CC) -c -o $@ $(ASFLAGS) $<
#	$(OBJDUMP) -h -S $@ > $@.lst

%.d: %.c
	@set -e; $(CC) -MM $(CPPFLAGS) $< \
	| sed 's=\($*\)\.o[ :]*=\1.o $@ : =g' > $@; \
	[ -s $@ ] || rm -f $@

%.d: %.S
	@set -e; $(CC) -MM $(CPPFLAGS_AS) $< \
	| sed 's=\($*\)\.o[ :]*=\1.o $@ : =g' > $@; \
	[ -s $@ ] || rm -f $@

%.elf: %.o
	$(CC) -Wl,-Map,$(@:.elf=.map) -o $@ $^ $(LDFLAGS)

clean:
	-@rm -f *\.d *\.o *\.elf *~
	-@rm -f *\.lst *\.map *\.bin *\.srec *\.hex

lst:  $(addsuffix .lst,$(PROGS))

%.lst: %.elf
	$(OBJDUMP) -h -S -D $< > $@

# Rules for building the .text FLASH images

hex:  $(addsuffix .hex,$(PROGS))

%.hex: %.elf
	$(OBJCOPY) -v -O ihex $< $@


DSOURCES =	$(MAIN_TEST).d $(MUTEX_TEST).d $(LPC_SOURCES:.c=.d) \
			$(SST_SOURCES:.c=.d) $(RB_SOURCES:.c=.d) 
-include $(DSOURCES)

