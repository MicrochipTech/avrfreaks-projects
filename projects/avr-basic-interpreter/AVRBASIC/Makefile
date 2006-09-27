# Makefile for avrproject
# Pascal Stang

########### change this lines according to your project ##################
#put the name of the target mcu here (at90s8515, at90s8535, attiny22, atmega603 etc.)
#	MCU = atmega163
#	MCU = atmega323
	MCU = atmega128
#	MCU = atmega32
#	MCU = atmega16

#put the name of the target file here (without extension)
	TRG	= MBASIC

#put your C sourcefiles here 

##	SRC = $(AVRLIB)/buffer.c $(AVRLIB)/uart2.c $(AVRLIB)/rprintf.c $(AVRLIB)/timer.c $(AVRLIB)/a2d.c $(AVRLIB)/vt100.c $(AVRLIB)/cmdline.c $(TRG).c

ifeq ($(MCU),atmega128)
	SRC = $(AVRLIB)/buffer.c $(AVRLIB)/uart2.c $(AVRLIB)/rprintf.c $(AVRLIB)/vt100.c $(AVRLIB)/a2d.c $(AVRLIB)/param.c $(TRG).c
else
	SRC = $(AVRLIB)/buffer.c $(AVRLIB)/uart.c $(AVRLIB)/rprintf.c $(AVRLIB)/vt100.c $(AVRLIB)/a2d.c $(AVRLIB)/param.c $(TRG).c
endif

#put additional assembler source file here
	ASRC =

#additional libraries and object files to link
	LIB	=

#additional includes to compile
	INC	= 

#assembler flags
	ASFLAGS = -Wa, -gstabs

#compiler flags
	CPFLAGS	= -g  -Os -Wall -Wstrict-prototypes -I$(AVRLIB) -Wa,-ahlms=$(<:.c=.lst)

#linker flags
	LDFLAGS = -Wl,-Map=$(TRG).map,--cref

	
########### you should not need to change the following line #############
include $(AVRLIB)/make/avrproj_make
		  
###### dependecies, add any dependencies you need here ###################
buffer.o		: buffer.c		buffer.h
ifeq ($(MCU),atmega128)
uart2.o			: uart2.c		uart2.h			global.h
else
uart.o			: uart.c		uart.h			global.h
endif

rprintf.o		: rprintf.c		rprintf.h
a2d.o			: a2d.c			a2d.h
timer128.o		: timer128.c		timer128.h		global.h
lcd.o			: lcd.c			lcd.h			global.h
i2c.o			: i2c.c			i2c.h			global.h
spi.o			: spi.c			spi.h			global.h
rtc.o			: rtc.c			rtc.h			global.h
param.o			: param.c		param.h			global.h
$(TRG).o		: $(TRG).c					global.h
