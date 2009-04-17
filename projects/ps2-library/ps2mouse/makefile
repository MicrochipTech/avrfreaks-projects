#Makefile created by New Project tool


#put the name of the target mcu here (at90s8515, at90s8535, attiny22, atmega603 etc.)
	MCU	= at90s8515
#put the name of the target file here (without extension)
	TRG	= testps2
#put your C sourcefiles here
	SRC	= $(TRG).c ps2.c
#put additional assembler source file here
	ASRC    = 
#additional libraries and object files to link
	LIB	= 
#additional includes to compile
	INC	=  ps2.h
#assembler flags
	ASFLAGS = -Wa, -gstabs
#compiler flags
	CPFLAGS	= -g  -O1 -Wall -Wstrict-prototypes -Wa,-ahlms=$(<:.c=.lst)
#linker flags
	LDFLAGS = -Wl,-Map=$(TRG).map,--cref

########### you should not need to change the following line #############
include $(AVR)/avrfreaks/avr_make

###### dependencies, add any dependencies you need here ###################
$(TRG).o : $(TRG).c ps2.c ps2.h
ps2.o : ps2.c ps2.h