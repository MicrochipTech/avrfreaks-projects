.SUFFIXES:

TARGETS = avr-console avr-console.dump

MANIFEST = Makefile project.h main.c librb.h librb.a console.c

OBJS_DIR = .objects
DEPS_DIR = .depends

AR = avr-ar
ARFLAGS = cru
CC = avr-gcc
CFLAGS = -Wall -Wno-main -O2 -std=c99 -mmcu=atmega328p -D__AVR_ATmega328P__    \
         -DF_CPU=16000000
CPPFLAGS = -MMD -MF $(DEPS_DIR)/$*.d
LD = avr-ld
LDFLAGS =
LEX = flex
YACC = bison
RAGEL = ragel

# target libraries
LIBS = $(filter %.a, $(TARGETS))

# target dumps
DUMPS = $(filter %.dump, $(TARGETS))

# target applications
APPS = $(filter-out $(LIBS) $(DUMPS), $(TARGETS))

# C source and header files
C_SRCS = $(filter %.c, $(MANIFEST))
C_HDRS = $(filter %.h, $(MANIFEST))

# libraries
LIBS = $(filter %.a, $(MANIFEST))

# ragel source files
RL_SRCS = $(filter %.rl, $(MANIFEST))

# ragel generated C source and header files
RL_C_SRCS += $(patsubst %.rl, %.c, $(RL_SRCS))
C_SRCS += $(RL_C_SRCS)

# flex source files
L_SRCS = $(filter %.l, $(MANIFEST))

# bison source files
Y_SRCS = $(filter %.y, $(MANIFEST))

# flex and bison generated C source and header files
L_C_SRCS += $(patsubst %.l, %.yy.c, $(L_SRCS))
L_C_HDRS += $(patsubst %.l, %.yy.h, $(L_SRCS))
Y_C_SRCS += $(patsubst %.y, %.tab.c, $(Y_SRCS))
Y_C_HDRS = $(patsubst %.y, %.tab.h, $(Y_SRCS))
Y_OUTPUTS = $(patsubst %.y, %.output, $(Y_SRCS))
C_SRCS += $(L_C_SRCS)
C_HDRS += $(L_C_HDRS)
C_SRCS += $(Y_C_SRCS)
C_HDRS += $(Y_C_HDRS)

# C object and dependancy files
C_OBJS = $(patsubst %.c, $(OBJS_DIR)/%.o, $(C_SRCS))
C_SRC_DEPS = $(patsubst %.c, $(DEPS_DIR)/%.d, $(C_SRCS))

.PHONY : all
all : $(TARGETS)

.PHONY : burn
burn : $(APPS)
	avrdude -p atmega328p -U $<

.PHONY : vars
vars :
	@echo C_SRCS = $(C_SRCS)
	@echo C_HDRS = $(C_HDRS)
	@echo L_SRCS = $(L_SRCS)
	@echo Y_SRCS = $(Y_SRCS)
	@echo L_C_SRCS = $(L_C_SRCS)
	@echo L_C_HDRS = $(L_C_HDRS)
	@echo Y_C_SRCS = $(Y_C_SRCS)
	@echo Y_C_HDRS = $(Y_C_HDRS)
	@echo Y_OUTPUTS = $(Y_OUTPUTS)
	@echo C_OBJS = $(C_OBJS)
	@echo C_SRC_DEPS = $(C_SRC_DEPS)

%.tab.c %.tab.h %.output : %.y
	$(YACC) --output=$*.tab.c --defines=$*.tab.h --report-file=$*.output   \
	        --verbose $<

%.yy.c %.yy.h : %.l
	$(LEX) --outfile=$*.yy.c --header-file=$*.yy.h $<

%.c : %.rl
	$(RAGEL) -G2 -Vp -o $<.dot $<
	dot -Tps -o $<.ps $<.dot
	$(RAGEL) -G2 $<


$(APPS) : $(C_OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $^

librb.a : librb/librb.a
	cp $< $@

librb.h : librb/librb.h
	cp $< $@

$(DUMPS) : $(APPS)
	avr-objdump -d $< > $@

$(C_OBJS) : | $(MANIFEST) $(L_C_HDRS) $(Y_C_HDRS) $(OBJS_DIR) $(DEPS_DIR)

$(OBJS_DIR) :
	mkdir $(OBJS_DIR)

$(DEPS_DIR) :
	mkdir $(DEPS_DIR)

$(C_OBJS) : $(OBJS_DIR)/%.o : %.c $(DEPS_DIR)/%.d
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $*.c

$(C_SRC_DEPS) : $(DEPS_DIR)/%.d :

-include $(C_SRC_DEPS)

.PHONY : clean
clean :
	-@rm 2> /dev/null $(TARGETS) $(C_OBJS) $(C_SRC_DEPS) $(RL_C_SRCS)          \
	                  $(L_C_SRCS) $(L_C_HDRS) $(Y_C_SRCS) $(Y_C_HDRS)          \
	                  $(Y_OUTPUTS)
	-@rmdir 2> /dev/null $(OBJS_DIR)
	-@rmdir 2> /dev/null $(DEPS_DIR)

