# written by stephen humble 1998-2005
# any suggestions for improvement please
# send email to stephenhumble@no_spam@telstra.com

HOME=$(shell pwd)
MYPATH=$(shell echo $(HOME)|sed "s/^.*\///g" )
SHELL = /bin/sh
CC = avr-gcc

INCLUDES= -Ilib -Iinclude  -L$(HOME)/lib/avr5 -L/home/steve/monorail/avr/lib/avr5
LIBRARY_PATH=$(HOME)/lib/avr5:$(HOME)/include

# are we compiling for which target hardware set see descriptions below
#WELDING make code for an ATmega16 board for chain maille resistance weld timing 
#MONORAIL make code for the UCC's single motor monorail application 
#PROCESS_TIMER make code for a temperature control application 
#DDRIVE adds code to use the board for a dual differential motor driver 
#ALARMSYS adds code to use the board for a house alarm application
#HW = MONORAIL
#HW = PROCESS_TIMER
HW = WELDING


#compile code for an ATmega16 setup to drive my differential drive robot base
ifeq ($(HW),DDRIVE)
MCLOCK=16
CLOCK=$(MCLOCK)*1000000
MCU = atmega16
DFS = -DMCLOCK=$(MCLOCK) -DDDRIVE -DCLOCK=$(CLOCK) -DVERSI=.13 -DATMEGA=\<avr/iom16.h\>
# set the AVR clock rate here.
endif



#compile code for an ATmega16 setup for my specialised spot welding application
#has a sharp 16x2 LCD interface and quaderature encoder for input
#uses an infra red source and detector to measure the collapse of the wire
#as it heats up and melts.
#the main code for this is the file weldtime.c
ifeq ($(HW),WELDING)
# set the AVR clock rate here.
MCLOCK=18
#SCLOCK =right shifts needed to approx change cpu clocks to micro second clocks
SCLOCK=4
CLOCK=18432000
DFS =  -DLCD_CODE -DQUAD_CODE -DCAP_CODE -DMCLOCK=$(MCLOCK) -DWELDING \
-DCLOCK=$(CLOCK) -DVERSI=.13 -DATMEGA=\<avr/iom16.h\>
MCU = atmega16
endif



# used for the ucc monorail control card with an ATmega32
# drives a 12 volt DC motor , and regulates the battery charging
# has a sharp LCD display to show status information
# main file for this is monorail.c
ifeq ($(HW),MONORAIL)
MCLOCK=22
CLOCK=22118400
DFS = -DIRRX_CODE -DLCD_CODE -DIRDDS_CODE -DMCLOCK=$(MCLOCK) -DMONORAIL -DCLOCK=$(CLOCK) \
 -DVERSI=.14 -DATMEGA=\<avr/iom32.h\>
MCU = atmega32
endif



# used for a process timing thermostat-temperature controller and clock application
# with a sharp 16x2 LCD menu interface
# the target cpu is for a ATmega16
ifeq ($(HW),PROCESS_TIMER)
MCLOCK=18
CLOCK=18432000
DFS = -DTIMER -DLCD_CODE -DQUAD_CODE -DMCLOCK=$(MCLOCK) -DCLOCK=$(CLOCK) \
 -DVERSI=.13 -DATMEGA=\<avr/iom16.h\>
MCU = atmega16
endif



####### Compiler, tools and options
#CPPFLAGS =  -v -w -Os -mcall-prologues $(INCLUDES) $(DFS) -mmcu=$(MCU) -minit-stack=260
CPPFLAGS =  -v -w -O2 -mcall-prologues $(INCLUDES) $(DFS) -mmcu=$(MCU)
######## parts of this project sources depends and objects etc 
SOURCES =$(wildcard *.c)
OBJS = $(subst .c,.o,$(SOURCES))

TARGET = x_$(MCU)_code

.SUFFIXES: .s .bin .out .hex .srec

.c.s:
	$(CC) $(CFLAGS) -S $<

.S.o:
	$(CC) $(ASFLAGS) -c $<

.o.out:
	$(CC) $(CFLAGS) -o $@ $<

.out.bin:
	avr-objcopy -O binary $< $@

.out.hex:
	avr-objcopy -O ihex $< $@

.out.srec:
	avr-objcopy -O srec $< $@

$(TARGET): $(OBJS)
	echo $(LIBRARY_PATH) 
	echo $(OBJS) 
	$(CC) -o $(TARGET) $(OBJS) $(CPPFLAGS) $(INCLUDES) -lc -lm -Xlinker -mavr5

asm:
	echo $(LIBRARY_PATH)
	$(CC) -S -g $(OBJS) $(CPPFLAGS) $(INCLUDES) -lc -lm

#dissassemble 
dis:    $(TARGET)
	echo $(LIBRARY_PATH)
	avr-objdump -d $(TARGET) | less

all:  $(OBJS)
	echo $(OBJS)
#	$(CC) -g -o $(TARGET) $(OBJS) $(CPPFLAGS) $(INCLUDES) -lc -lm -Xlinker -mavr5
	$(CC) -o $(TARGET) $(OBJS) $(CPPFLAGS) $(INCLUDES) -lc -lm -Xlinker -mavr5
	cp $(TARGET) $(TARGET).srec
	objcopy -O srec $(TARGET).srec
	uisp -dvoltage=5 -dprog=dapa --erase
	uisp -dvoltage=5 -dprog=dapa -dt_sck=1 if=$(TARGET).srec --upload -v

info:   $(TARGET)
	avr-readelf -l $(TARGET)
	avr-size $(TARGET)

load:   $(TARGET)
	cp $(TARGET) $(TARGET).srec
	objcopy -O srec $(TARGET).srec
	uisp -dprog=dapa  if=$(TARGET).srec --upload --verify -v

bin:    $(TARGET)
	cp $(TARGET) $(TARGET).bin
	cp $(TARGET) $(TARGET).hex
	cp $(TARGET) $(TARGET).srec
	objcopy -O binary $(TARGET).bin
	objcopy -O srec $(TARGET).srec
	objcopy -O ihex $(TARGET).hex

tar:
	$(shell cd ..;rm -f $(MYPATH).tgz;tar -czpf $(MYPATH).tgz $(MYPATH))
	ls -l  ../$(MYPATH).tgz

save:
	uisp -dprog=dapa  --rd_fuses
	uisp -dprog=dapa  --download of=out
	
erase:
	uisp -dprog=dapa  --erase
	uisp -dprog=dapa  --download

# set for 1 mhz internal clock options 
#	uisp -dprog=dapa  --wr_fuse_l=0x61
fuse1:
	uisp -dprog=dapa  --rd_fuses
	uisp -dprog=dapa  --wr_fuse_h=0x50
	uisp -dprog=dapa  --wr_fuse_l=0x61
	uisp -dprog=dapa  --rd_fuses

# set for 8 mhz internal clock options 
#	uisp -dprog=dapa  --wr_fuse_l=0x61
fuse8:
	uisp -dprog=dapa  --rd_fuses
	uisp -dprog=dapa  --wr_fuse_h=0x50
	uisp -dprog=dapa  --wr_fuse_l=0x64
	uisp -dprog=dapa  --rd_fuses

# set for 16+ mhz external crystal clock options 
#	uisp -dprog=dapa  --wr_fuse_l=0x61
fuse16:
	uisp -dprog=dapa  --rd_fuses
	uisp -dprog=dapa  --wr_fuse_h=0x40
	uisp -dprog=dapa  --wr_fuse_l=0x7f
# lower fuse with enable brown out detection enabled
	uisp -dprog=dapa  --rd_fuses

# MCLOCK fuse automatic setting 
fuse:
ifeq "$(MCLOCK)" "16"
	$(MAKE) fuse16 -C .
endif
ifeq "$(MCLOCK)" "8"
	$(MAKE) fuse8 -C .
endif
ifeq "$(MCLOCK)" "1"
	$(MAKE) fuse1 -C .
endif
 
clean:
	-rm -fv $(OBJS) $(TARGET)* *~ core *.dd *.bak tags

