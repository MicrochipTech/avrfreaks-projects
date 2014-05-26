
.SUFFIXES:

TARGETS = librb.a

MANIFEST = Makefile librb.h rb_echo.c rb_erase.c rb_get.c rb_init.c rb_kill.c  \
           rb_put.c rb_put_echo.c rb_unput.c

LIBRARIES =

OBJS_DIR = .objects
DEPS_DIR = .depends

AR = avr-ar
ARFLAGS = cru
CC = avr-gcc
CFLAGS = -Wall -O2 -std=c99 -mmcu=atmega328p -D__AVR_ATmega328P__
CPPFLAGS = -MMD -MF $(DEPS_DIR)/$*.d
LEX = flex
YACC = bison

# C source and header files
C_SRCS = $(filter %.c, $(MANIFEST))
C_HDRS = $(filter %.h, $(MANIFEST))

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

$(TARGETS) : $(C_OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(C_OBJS) : | $(L_C_HDRS) $(Y_C_HDRS) $(OBJS_DIR) $(DEPS_DIR)

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
	-@rm 2> /dev/null $(TARGETS) $(C_OBJS) $(C_SRC_DEPS) $(L_C_SRCS)       \
	                  $(L_C_HDRS) $(Y_C_SRCS) $(Y_C_HDRS) $(Y_OUTPUTS)
	-@rmdir 2> /dev/null $(OBJS_DIR)
	-@rmdir 2> /dev/null $(DEPS_DIR)

