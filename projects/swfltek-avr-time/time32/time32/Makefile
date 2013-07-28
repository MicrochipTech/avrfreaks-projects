SHELL = /bin/sh

ifneq (,)
This makefile requires GNU Make.
endif

.SUFFIXES:
.SUFFIXES: .c .cpp .S


# determine the OS
OS := $(shell uname)

ifeq ($(OS),Darwin)
OS := unix
ARDUINO := /Applications/Arduino/Contents/Resources/Java/hardware/tools/avr/bin/
else

ifeq ($(OS),FreeBSD)
OS := unix
ARDUINO := ~/Arduino/hardware/tools/avr/bin/
else

ifeq ($(OS),Linux)
OS := unix
ARDUINO := ~/Arduino/hardware/tools/avr/bin/
else

OS := Windows
ARDUINO := c:/progra~1/arduin~1.4/hardware/tools/avr/bin/
endif
endif
endif

# Generate list of source code files in /source
CSOURCES := $(wildcard time/*.c)

# Generate corresponding list of object code files
OBJS := $(patsubst %.c, %.o, $(notdir $(CSOURCES)))
#$(info Objs = $(OBJS))

# specify compiler and archiver paths
CC := $(TOOLCHAIN)avr-gcc
CCVERSION := $(shell $(CC) -dumpversion)
AR := $(TOOLCHAIN)avr-ar

# see what optimizations we can use
CCFLAGS += -Wall -Wextra -Werror -pedantic -Wstrict-prototypes -fpack-struct -frename-registers -c -Os
CCOPTIONS := $(shell $(CC) --help=target)
CCFLAGS += $(findstring -mstrict-X,$(CCOPTIONS))
CCFLAGS += $(findstring -maccumulate-args,$(CCOPTIONS))
CCFLAGS += $(findstring -mcall-prologues,$(CCOPTIONS))

# locate path to architecture directories
ifndef LIBPATH
LIBPATH := $(shell $(CC) -print-file-name=libc.a)
LIBPATH := $(realpath $(LIBPATH))
LIBPATH := $(dir $(LIBPATH))

endif

# generate list of architectures to build for
DIRS := $(wildcard $(LIBPATH)avr*)
ARCHES := $(notdir $(DIRS))
ARCHES += avr2

# one copy of each object file for each architecture
TRGTS := $(foreach a,$(ARCHES),$(foreach n,$(OBJS),$(a)/$(n)) )

INCLUDES := -I./include

.PHONY: clean tidy install uninstall all help format
.PRECIOUS: $(ARCHES)

help:
	$(info	)
	$(info				Swfltek Time Library for AVR-LIBC  $(OS))
	$(info	)
	$(info 'make all' to compile with avr-gcc-$(CCVERSION) for...)
	$(info $(ARCHES))
	$(info	)
	$(info	)
	$(info 'make install' to install into $(LIBPATH))
	$(info	)
	$(info	)
	$(info 'make uninstall' to remove )
	$(info	)
	$(info	)
	$(info To specify the toolchain path, use 'TOOLCHAIN=(path/to/avr/bin/)' )
	$(info	( path must end with slash ))
	$(info		'make TOOLCHAIN=/usr/local/avr/bin/ all' )
	$(info		'make TOOLCHAIN=/usr/local/avr/bin/ install' )
	$(info	)
	$(info	)
	$(info To install for Arduino, TOOLCHAIN will be similar to...)
	$(info		$(ARDUINO))
	$(info	)
	$(info	)
	$(info To specify a different library set, use 'LIBPATH=(path/to/avr/lib)' )
	$(info 'make LIBPATH=$(LIBPATH)')
	$(info			questions may be directed to support@swfltek.com)
	$(info	)

all: $(TRGTS)
	$(info Ready to install)

ifeq ($(OS),unix)

$(LIBPATH)/avr2:
	mkdir $(LIBPATH)/avr2
	cp $(LIBPATH)/libc.a $(LIBPATH)/avr2

install: $(LIBPATH)/avr2
	cp include/time.h $(LIBPATH)../include
	cp include/*_dst.h $(LIBPATH)../include/util
	for i in $(ARCHES) ; do $(AR) rcs $(LIBPATH)$$i/libc.a $$i/*.o; \
	done;
	@echo "Library installed"

uninstall: $(LIBPATH)/avr2
	rm $(LIBPATH)../include/time.h
	rm $(LIBPATH)../include/util/*_dst.h
	for i in $(ARCHES) ; do $(AR) ds $(LIBPATH)$$i/libc.a system_tick.o $(OBJS); \
	done;
	@echo "Library removed"
else

$(LIBPATH)/avr2:
	mkdir $(subst /,\,$(LIBPATH))\avr2
	cp $(LIBPATH)/libc.a $(LIBPATH)\avr2

install: $(LIBPATH)/avr2
	cp include/time.h $(LIBPATH)../include
	cp include/*_dst.h $(LIBPATH)../include/util
	for %%i in ($(ARCHES) ) do $(AR) rcs $(LIBPATH)%%i/libc.a %%i/*.o
	@echo "library installed"

uninstall: $(LIBPATH)/avr2
	rm -f $(LIBPATH)../include/time.h
	rm $(LIBPATH)../include/util/*_dst.h
	for %%i in ($(ARCHES)) do $(AR) ds $(LIBPATH)%%i/libc.a system_tick.o $(OBJS)
	@echo "Library removed"

endif

clean:
	rm -rf *.o *.d *.a avr*

tidy:
	rm -rf *.o *.d

avr%:
	mkdir $@
	$(CC) $(CCFLAGS) $(INCLUDES) -mmcu=$@  -o $@/system_tick.o time/system_tick.S

%.o: $(ARCHES)
	$(CC) $(CCFLAGS) $(INCLUDES) -mmcu=$(subst /,,$(dir $@))  -o $@ time/$(basename $(notdir $@)).c


