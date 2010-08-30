#
# Makefile for AVR program
#

		TARGET		=	usbl

		SOURCES		=	usbl.S

#		CHIP		=	atmega32u4
		CHIP		=	at90usb1287

#############################################################################
#############################################################################
#############################################################################

		CPUFLAGS	=	-mmcu=$(CHIP)
		DEFINES		+=	-DF_CPU=8000000UL \
						-DGCC_AVR \
						-Dgcc

		INCLUDES	+=	-I.

		CPPFLAGS	?=	$(DEFINES) $(INCLUDES)

		CDEBUGFLAGS	?=	-O3 -g -Wall \
						$(CPUFLAGS)

		ASFLAGS		+=	$(CPUFLAGS)
		CFLAGS		+=	$(CDEBUGFLAGS)
		CXXFLAGS	+=	$(CDEBUGFLAGS) \
						-fno-rtti \
						--no-exceptions

		LDFLAGS		=	-Wl,--verbose \
						-Wl,-N,-M,--demangle,--sort-common,-u,__boot__ \
						-Wl,--defsym,__cxa_pure_virtual=0 \
						-mfloat-abi=soft -mfpu=vfp -nostdlib \
						$(CPUFLAGS) \
						$(LIBDIRS)

		LOADLIBES	=	-lc

		TOOLPREFIX	=	avr-elf-
			CC		=	$(TOOLPREFIX)gcc
			CXX		=	$(TOOLPREFIX)g++
			LD		=	$(TOOLPREFIX)ld
			AS		=	$(TOOLPREFIX)as
			AR		=	$(TOOLPREFIX)ar
		OBJCOPY		=	$(TOOLPREFIX)objcopy
		OBJDUMP		=	$(TOOLPREFIX)objdump

			aOBJ	=	$(SOURCES:.S=.o)
			cOBJ	=	$(aOBJ:.c=.o)
			OBJ		=	$(cOBJ:.cc=.o)

all:	$(TARGET).hex
	cp $< /tmp

$(TARGET):	$(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LOADLIBES) > $@.map
	$(OBJDUMP) --disassemble-all --demangle $@ > $@.lst

clean:
	$(RM) $(TARGET) *.lst *.map *.bin *.hex \
			$(OBJ) *.d

%.bin:	%
	$(OBJCOPY) -S -O binary $< $@

%.hex:	%
	$(OBJCOPY) -S -O ihex $< $@
