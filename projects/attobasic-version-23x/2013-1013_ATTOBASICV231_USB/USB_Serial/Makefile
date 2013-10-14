# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
# WinAVR Makefile Template written by Eric B. Weddington, J\u00F6rg Wunsch, et al.
#
# Released to the Public Domain
#
# Additional material for this makefile was written by:
# Peter Fleury
# Tim Henigan
# Colin O'Flynn
# Reiner Patommel
# Markus Pfaff
# Sander Pool
# Frederik Rouleau
# Carlos Lamas
# Dean Camera
# Opendous Inc.
# Denver Gingerich
#
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make dfu = Download the hex file to the device, using dfu-programmer (must
#            have dfu-programmer installed).
#
# make flip = Download the hex file to the device, using Atmel FLIP (must
#             have Atmel FLIP installed).
#
# make dfu-ee = Download the eeprom file to the device, using dfu-programmer
#               (must have dfu-programmer installed).
#
# make flip-ee = Download the eeprom file to the device, using Atmel FLIP
#                (must have Atmel FLIP installed).
#
# make doxygen = Generate DoxyGen documentation for the project (must have
#                DoxyGen installed)
#
# make debug = Start either simulavr or avarice as specified for debugging,
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------
# Target file name (without extension).
TARGET = usb_serial
#
# Added by KSV
# Add extra command line define for different targets
ifeq ($(MAKECMDGOALS),TEENSYPP20)
  MCU = at90usb1286
  F_CPU = 16000000
  F_USB = 16000000
  PROG_START = 0x1EF40
  PROGDATA_START = 0x1F580
  DATA_START = 0x8020A0
  CODE_FILE = Code_$(TARGET)_TEENSYPP20.inc
  DEF_FILE = Defs_$(TARGET)-TEENSYPP20.inc
endif

ifeq ($(MAKECMDGOALS),USB1286-8M)
  MCU = at90usb1286
  F_CPU = 8000000
  F_USB = $(F_CPU)
# for DFU Bootloader at 0xF800
#  PROG_START = 0x1E820
#  PROGDATA_START = 0x1EE00
# for DFU Bootloader at 0xF000 
  PROG_START = 0x1D7E0
  PROGDATA_START = 0x1DE20
  DATA_START = 0x8020A0
  CODE_FILE = Code_$(TARGET)_$(MCU)-8M.inc
  DEF_FILE = Defs_$(TARGET)-$(MCU).inc
endif

ifeq ($(MAKECMDGOALS),USB1286-16M)
  MCU = at90usb1286
  F_CPU = 16000000
  F_USB = $(F_CPU)
# for DFU Bootloader at 0xF800
#  PROG_START = 0x1E820
#  PROGDATA_START = 0x1EE00
# for DFU Bootloader at 0xF000 
  PROG_START = 0x1D7E0
  PROGDATA_START = 0x1DE20
  DATA_START = 0x8020A0
  CODE_FILE = Code_$(TARGET)_$(MCU)-16M.inc
  DEF_FILE = Defs_$(TARGET)-$(MCU).inc
endif

ifeq ($(MAKECMDGOALS),M32U4-8M)
  MCU = atmega32u4
  F_CPU = 8000000
  F_USB = $(F_CPU)
  PROG_START = 0x6820
  PROGDATA_START = 0x6E00
  DATA_START = 0x800AA0
  CODE_FILE = Code_$(TARGET)_$(MCU)-8M.inc
  DEF_FILE = Defs_$(TARGET)-$(MCU).inc
endif

ifeq ($(MAKECMDGOALS),M32U4-16M)
  MCU = atmega32u4
  F_CPU = 16000000
  F_USB = $(F_CPU)
  PROG_START = 0x6820
  PROGDATA_START = 0x6E00
  DATA_START = 0x800AA0
  CODE_FILE = Code_$(TARGET)_$(MCU)-16M.inc
  DEF_FILE = Defs_$(TARGET)-$(MCU).inc
endif


#=======================================================================
# MCU name
ifndef MCU
  MCU = atmega32u4
  #MCU = at90usb1286
endif

# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the
#     processor frequency in Hz. You can then use this symbol in your source code to
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
#
#     This will be an integer division of F_USB below, as it is sourced by
#     F_USB after it has run through any CPU prescalers. Note that this value
#     does not *change* the processor frequency - it should merely be updated to
#     reflect the processor speed set externally so that the code can use accurate
#     software delays.
ifndef F_CPU
  F_CPU = 16000000
  #F_CPU = 8000000
endif

# Input clock frequency.
#     This will define a symbol, F_USB, in all source code files equal to the
#     input clock frequency (before any prescaling is performed) in Hz. This value may
#     differ from F_CPU if prescaling is used on the latter, and is required as the
#     raw input clock is fed directly to the PLL sections of the AVR for high speed
#     clock generation for the USB and other AVR subsections. Do NOT tack on a 'UL'
#     at the end, this will be done automatically to create a 32-bit value in your
#     source code.
#
#     If no clock division is performed on the input clock inside the AVR (via the
#     CPU clock adjust registers or the clock division fuses), this will be equal to F_CPU.
F_USB = $(F_CPU)

# Starting byte address of the bootloader, as a byte address - computed via the formula
#   BOOT_START = ((FLASH_SIZE_KB - BOOT_SECTION_SIZE_KB) * 1024)
#
# Note that the bootloader size and start address given in AVRStudio is in words and not
# bytes, and so will need to be doubled to obtain the byte address needed by AVR-GCC.
#FLASH_SIZE_KB        = 32
#BOOT_SECTION_SIZE_KB = 4
#BOOT_START           = 0x$(shell echo "obase=16; ($(FLASH_SIZE_KB) - $(BOOT_SECTION_SIZE_KB)) * 1024" | bc)


# Added by Scott Vitale
#m32u4-16: F_CPU = 16000000L
#m32u4-8: F_CPU = 8000000L
#-------------------------------------

# Starting byte address of the program
#PROG_START = 0x6500
#PROG_START = 0x6800
#PROGDATA_START = 0x6E00

# Starting byte address of this code's RAM usage (+0x800000 offset)
#DATA_START = 0x800100
#DATA_START = 0x800AC0

# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Object files directory
#     To put object files in current directory, use a dot (.), do NOT make
#     this an empty or blank macro!
OBJDIR = .


# List C source files here. (C dependencies are automatically generated.)
SRC = $(TARGET).c


# List C++ source files here. (C dependencies are automatically generated.)
CPPSRC =


# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
ASRC =


# Optimization level, can be [0, 1, 2, 3, s].
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s
#OPT = 0


# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2
#DEBUG = stabs


# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS = 

# detect OS
OSUPPER = $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:])
OSLOWER = $(shell uname -s 2>/dev/null | tr [:upper:] [:lower:])

# 'linux' is output for Linux system, 'darwin' for OS X
LINUX = $(strip $(findstring LINUX, $(OSUPPER)))
WINDOWS = $(strip $(findstring WINDOWS, $(OSUPPER)))
#ifneq ($(LINUX),)
#   SNOWLEOPARD = $(strip $(findstring 10.6, $(shell egrep "<string>10\.6" /System/Library/CoreServices/SystemVersion.plist)))
#endif

# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
#CSTANDARD = -std=c99
CSTANDARD = -std=gnu99

# Place -D or -U options here for C sources
CDEFS  = -DF_CPU=$(F_CPU)UL
CDEFS += -DF_USB=$(F_USB)UL
#CDEFS += -DBOOT_START_ADDR=$(BOOT_START)UL
CDEFS += -DPROG_START_ADDR=$(PROG_START)UL


# Place -D or -U options here for ASM sources
ADEFS  = -DF_CPU=$(F_CPU)UL
ADEFS += -DF_USB=$(F_USB)UL

# Place -D or -U options here for C++ sources
CPPDEFS  = -DF_CPU=$(F_CPU)UL
CPPDEFS += -DF_USB=$(F_USB)UL
#CPPDEFS += -D__STDC_LIMIT_MACROS
#CPPDEFS += -D__STDC_CONSTANT_MACROS

#---------------- Compiler Options C ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -ffunction-sections
CFLAGS += -fno-inline-small-functions
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -fno-strict-aliasing
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes

#CFLAGS += -mno-interrupts

#uncomment next line to enable viewing preprocessor directives
#CFLAGS += -dD -E

#CFLAGS += -mshort-calls
#CFLAGS += -fno-unit-at-a-time
#CFLAGS += -Wundef
#CFLAGS += -Wunreachable-code
#CFLAGS += -Wsign-compare
CFLAGS += -Wa,-adhlns=$(<:%.c=$(OBJDIR)/%.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CPPFLAGS = -g$(DEBUG)
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -O$(OPT)
CPPFLAGS += -funsigned-char
CPPFLAGS += -funsigned-bitfields
CPPFLAGS += -fpack-struct
CPPFLAGS += -fshort-enums
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wall
CPPFLAGS += -Wundef
#CPPFLAGS += -mshort-calls
#CPPFLAGS += -fno-unit-at-a-time
#CPPFLAGS += -Wstrict-prototypes
#CPPFLAGS += -Wunreachable-code
#CPPFLAGS += -Wsign-compare
CPPFLAGS += -Wa,-adhlns=$(<:%.cpp=$(OBJDIR)/%.lst)
CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
#CPPFLAGS += $(CSTANDARD)


#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -adhlns:   create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex
#       dump that will be displayed for a given single line of source input.
ASFLAGS = $(ADEFS) -Wa,-adhlns=$(<:%.S=$(OBJDIR)/%.lst),-gstabs,--listing-cont-lines=100


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB =
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


MATH_LIB = -lm


# List any extra directories to look for libraries here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRALIBDIRS =


#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--section-start,.data=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

#---------------- Linker Options ----------------
#  -Wl,...: tell GCC to pass this to linker.
#  -Map:    create map file
#  --cref:  add cross reference to  map file
#  -x		delete all local symbols
#  -S		Omit debugger symbol information
#  --retain-symbols-file [filename]
#
LDFLAGS  = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += -Wl,--relax
# enable to emit only specific symbols
LDFLAGS += -Wl,--retain-symbols-file $(TARGET)-api.lst

#LDFLAGS += -Wl,-u,vfprintf -lprintf_min

#disable the following line to enable linking sections
#LDFLAGS += -Wl,--gc-sections

LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(patsubst %,-L%,$(EXTRALIBDIRS))
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
#LDFLAGS += -T linker_script.x

#Define sections where to store signal tables
#LDFLAGS += -Wl,-section-start=.WaveData1=0x3600
#LDFLAGS += -Wl,-section-start=.WaveData2=0x3700
#LDFLAGS += -Wl,-section-start=.MySection3=0x3C00
#LDFLAGS += -Wl,-section-start=.MySection4=0x3D00
#LDFLAGS += -Wl,-section-start=.MySection5=0x3E00
#LDFLAGS += -Wl,-section-start=.MySection6=0x3F00 

#--------------- linker Options for placing code at specific location -------
LDFLAGS += -Wl,--section-start=.text=$(PROG_START) -nostartfiles -nodefaultlibs
#LDFLAGS += -Wl,-Ttext,$(PROG_START)	# same as line above
LDFLAGS += -Wl,--section-start=.progdata=$(PROGDATA_START)

#--------------- This code's specific RAM addreses as linker Options -------
LDFLAGS += -Wl,--section-start=.data=$(DATA_START)
#LDFLAGS += -Wl,-Tdata,$(DATA_START)	# same as line above

#--------------- bootloader linker Options -------
#LDFLAGS += -Wl,--section-start=.text=$(BOOT_START)
#LDFLAGS += -Wl,--section-start=.text=$(BOOT_START) -nostartfiles -nodefaultlibs

#---------------- Programming Options (avrdude) ----------------

# Programming hardware
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = jtagmkII

# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = usb

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)

#---------------- Debugging Options ----------------

# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(F_CPU)

# Set the DEBUG_UI to either ddd, gdb or insight.
DEBUG_UI = ddd
#DEBUG_UI = gdb
#DEBUG_UI = insight

# Set the debugging back-end to either avarice, simulavr.
DEBUG_BACKEND = avarice
#DEBUG_BACKEND = simulavr

# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit

# When using with AVR Dragon, enable these 
AVR_DRAGON = --dragon
VEND_PROD = 03eb:2107 
DRAGON_USBBUS = $(shell TEMP=`lsusb -d $(VEND_PROD) 2>/dev/null` ; \
  BUS=$${TEMP:4:3} ; echo $${BUS} )
DRAGON_USBDEV = $(shell TEMP=`lsusb -d $(VEND_PROD) 2>/dev/null` ; \
  DEV=$${TEMP:15:3} ; echo $${DEV} )

# When using avarice settings for the JTAG
#JTAG_DEV = /dev/com1
JTAG_DEV = --jtag usb
JTAG_MCLK = --jtag-bitrate 250KHz
#ifeq ($(F_CPU),1000000) 
#	JTAG_MCLK = --jtag-bitrate 250KHz
#endif

# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242

# Debugging host used to communicate between GDB / avarice / simulavr, normally
#     just set to localhost unless doing some sort of crazy debugging when
#     avarice is running on a different computer.
DEBUG_HOST = localhost

#============================================================================


# Define programs and commands.
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AR = avr-ar rcs
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp
SREC = srec_cat
CAT = cat
SED = sed
CUT = cut
WINSHELL = cmd

INC_DIR = ../Include

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_CPP = Compiling C++:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:
MSG_CREATING_LIBRARY = Creating library:

# Define all object files.
OBJ = $(SRC:%.c=$(OBJDIR)/%.o) $(CPPSRC:%.cpp=$(OBJDIR)/%.o) $(ASRC:%.S=$(OBJDIR)/%.o)

# Define all listing files.
LST = $(SRC:%.c=$(OBJDIR)/%.lst) $(CPPSRC:%.cpp=$(OBJDIR)/%.lst) $(ASRC:%.S=$(OBJDIR)/%.lst)

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_CPPFLAGS = -mmcu=$(MCU) -I. -x c++ $(CPPFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)

# Default target.
#all: begin gccversion sizebefore build join_hex sizeafter end
all: begin gccversion sizebefore build sizeafter end
#all: $(TARGET).hex join_hex size 

# Change the build target to build a HEX file or a library.
build: elf hex eep lss sym inc
#build: lib

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym
inc: $(TARGET).inc
LIBNAME=lib$(TARGET).a
lib: $(LIBNAME)

# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo

force:

# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) $(MCU_FLAG) $(FORMAT_FLAG) $(TARGET).elf
MCU_FLAG = $(shell $(SIZE) --help | grep -- --mcu > /dev/null && echo --mcu=$(MCU) )
FORMAT_FLAG = $(shell $(SIZE) --help | grep -- --format=.*avr > /dev/null && echo --format=avr )

sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
	2>/dev/null; echo; fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
	2>/dev/null; echo; fi

version.c:
	@echo '#include <avr/pgmspace.h>' > $@ 
	@echo '#include "version.h"' >> $@ 
	@echo -n 'const char fwVersionStr[] PROGMEM = "Build: "' >> $@ 
	@date +'"%Y%m%d.%H%M\r\n";' >> $@;

# Generate Code for 8M Target
#8M: all
USB1286-8M: all
M32U4-8M: all

# Generate Code for 16M Target
#16M: all
USB1286-16M: all
M32U4-16M: all
TEENSYPP20: all		# Teensy++ 2.0 support

# Always compile version.c to get correct compilation date and time
.PHONY:	version.c

# Display compiler version information.
gccversion :
	@$(CC) --version

join_hex: $(TARGET).hex 
	srec_cat $(TARGET).hex -intel -exclude 0x7000 -0x7FFF -o $(TARGET).bin -binary
	srec_cat $(TARGET).bin -binary -o $(TARGET).hex -intel
	rm -f $(TARGET).bin
	srec_cat ../LUFA110528/Bootloaders/DFU/BootloaderDFU.hex -intel $(TARGET).hex -intel -o $(TARGET)_Bldr.hex -intel

# Program the device.
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

flip: $(TARGET).hex
	batchisp -hardware usb -device $(MCU) -operation erase f
	batchisp -hardware usb -device $(MCU) -operation loadbuffer $(TARGET).hex program
	batchisp -hardware usb -device $(MCU) -operation start reset 0

dfu: $(TARGET).hex
	dfu-programmer $(MCU) erase
	dfu-programmer $(MCU) flash $(TARGET).hex
	dfu-programmer $(MCU) reset

flip-ee: $(TARGET).hex $(TARGET).eep
	$(COPY) $(TARGET).eep $(TARGET)eep.hex
	batchisp -hardware usb -device $(MCU) -operation memory EEPROM erase
	batchisp -hardware usb -device $(MCU) -operation memory EEPROM loadbuffer $(TARGET)eep.hex program
	batchisp -hardware usb -device $(MCU) -operation start reset 0
	$(REMOVE) $(TARGET)eep.hex

dfu-ee: $(TARGET).hex $(TARGET).eep
	dfu-programmer $(MCU) eeprom-flash $(TARGET).eep
	dfu-programmer $(MCU) reset


# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set
#     a breakpoint at main().
gdb-config:
	@$(REMOVE) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file $(TARGET).elf >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load  >> $(GDBINIT_FILE)
endif
	@echo break main >> $(GDBINIT_FILE)

debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
  ifeq ($(LINUX),LINUX)
	@echo
	@echo Host OS is $(LINUX)
	@echo Starting AVaRICE
	@avarice --detach --erase --file $(TARGET).elf $(AVR_DRAGON) $(JTAG_DEV) $(JTAG_MCLK) \
	-P $(MCU) $(DEBUG_HOST):$(DEBUG_PORT) > /dev/null 2>&1 
  else
	@echo
	@echo Host OS is $(WINDOWS)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice $(AVR_DRAGON) $(JTAG_MCLK) $(JTAG_DEV) -P $(MCU) --erase --program --file \
	$(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause
  endif
else
  ifeq ($(LINUX),LINUX)
	@simulavr --gdbserver --device $(MCU) --clock-freq \
	$(DEBUG_MFREQ) --port $(DEBUG_PORT)
  else
	@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
	$(DEBUG_MFREQ) --port $(DEBUG_PORT)
  endif
endif
 ifeq ($(LINUX),LINUX)
   ifeq ($(DEBUG_UI),ddd)
	@ddd --debugger "avr-gdb -x $(GDBINIT_FILE)" >/dev/null 2>&1
   else
	@avr-$(DEBUG_UI) -x $(GDBINIT_FILE)
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)
   endif
	# reset the AVR Dragon
	@$(shell) usb_reset /dev/bus/usb/$(DRAGON_USBBUS)/$(DRAGON_USBDEV) >/dev/null 2>&1
 else
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)
 endif
 
# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT = $(OBJCOPY) --debugging
COFFCONVERT += --change-section-address .data-0x800000
COFFCONVERT += --change-section-address .bss-0x800000
COFFCONVERT += --change-section-address .noinit-0x800000
COFFCONVERT += --change-section-address .eeprom-0x810000

coff: $(TARGET).elf
	@echo
	@echo $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	@echo
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) --gap-fill 0xFF -R .eeprom -R .fuse -R .lock $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 --no-change-warnings -O $(FORMAT) $< $@ || exit 0

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -z $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@

# Create library from object files.
.SECONDARY : $(TARGET).a
.PRECIOUS : $(OBJ)
%.a: $(OBJ)
	@echo
	@echo $(MSG_CREATING_LIBRARY) $@
	$(AR) $@ $(OBJ)

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)

# Compile: create object files from C source files.
$(OBJDIR)/%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@

# Compile: create object files from C++ source files.
$(OBJDIR)/%.o : %.cpp
	@echo
	@echo $(MSG_COMPILING_CPP) $<
	$(CC) -c $(ALL_CPPFLAGS) $< -o $@

# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@

# Compile: create assembler files from C++ source files.
%.s : %.cpp
	$(CC) -S $(ALL_CPPFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Create preprocessed source for use in sending a bug report.
%.i : %.c
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@

#####  Generating a gdb initialisation file    #####
##### Use by launching simulavr and avr-gdb:   #####
#####   avr-gdb -x gdbinit-myproject           #####
#gdbinit: $(GDBINITFILE)
#
#$(GDBINITFILE): $(TRG)
#	@echo "file $(TRG)" > $(GDBINITFILE)
#	@echo "target remote localhost:1212" >> $(GDBINITFILE)
#	@echo "load"        >> $(GDBINITFILE) 
#	@echo "break main"  >> $(GDBINITFILE)
#	@echo "continue"    >> $(GDBINITFILE)
#	@echo
#	@echo "Use 'avr-gdb -x $(GDBINITFILE)'"


# Create the include hex listing and subroutine pointers for use with the AttoBASIC project.
%.inc:
	@echo $(MSG_CREATING_INCLUDE) $< $@ at $(PROG_START)
#	@$(SREC) $(TARGET).hex -intel -offset -$(PROG_START) -fill 0xFF -within $(TARGET).hex -o  $(INC_DIR)/$(CODE_FILE) -asm -HEXadecimal_STyle
#	@$(SREC) $(TARGET).hex -intel -offset -$(PROG_START) -o $(INC_DIR)/$(CODE_FILE) -asm -HEXadecimal_STyle
	@$(SREC) $(TARGET).hex -intel -offset -$(PROG_START) -o - -asm -HEXadecimal_STyle | \
		$(SED) 's/ DB / .db/g' > $(INC_DIR)/$(CODE_FILE)
	@$(CAT) $(TARGET).sym | $(SED) 's/ T / FLASH /g' | \
		$(SED) 's/0000/0x/g' | \
		$(SED) 's/0001/0x1/g' | \
		$(SED) 's/ d / RAM /g' | \
		$(SED) 's/ b / RAM /g' | \
		$(SED) 's/0080/0x/g' | \
		$(SED) 's/__vector_10/USB_GEN_int/g' | \
		$(SED) 's/__vector_11/USB_COM_int/g' > temp.sym
	@$(REMOVE) $(INC_DIR)/$(DEF_FILE)
	@$(CAT) temp.sym | while read ADDR TYPE SYMBOL ; do \
		if [ $$TYPE == FLASH ] ; then echo ".equ $$SYMBOL = $$ADDR/2" >> $(INC_DIR)/$(DEF_FILE) ; fi ;\
		if [ $$TYPE == RAM ] ; then echo ".equ $$SYMBOL = $$ADDR" >> $(INC_DIR)/$(DEF_FILE) ; fi ;\
		done
	@$(REMOVE) temp.sym

# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo $(WINSHELL)
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(SRC:%.c=$(OBJDIR)/%.o) $(CPPSRC:%.cpp=$(OBJDIR)/%.o) $(ASRC:%.S=$(OBJDIR)/%.o)
	$(REMOVE) $(SRC:%.c=$(OBJDIR)/%.lst) $(CPPSRC:%.cpp=$(OBJDIR)/%.lst) $(ASRC:%.S=$(OBJDIR)/%.lst)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(SRC:.c=.i)
	$(REMOVEDIR) .dep
	$(REMOVE) $(LST) $(GDBINITFILE)

doxygen:
	@echo Generating Project Documentation \($(TARGET)\)...
	@doxygen Doxygen.conf
	@echo Documentation Generation Complete.

clean_doxygen:
	rm -rf Documentation

checksource:
	@for f in $(SRC) $(CPPSRC) $(ASRC); do \
		if [ -f $$f ]; then \
			echo "Found Source File: $$f" ; \
		else \
			echo "Source File Not Found: $$f" ; \
		fi; done 


# Create object files directory
$(shell mkdir $(OBJDIR) 2>/dev/null)


# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff doxygen clean          \
clean_list clean_doxygen program dfu flip flip-ee dfu-ee      \
debug gdb-config checksource
