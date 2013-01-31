###############################################################################
# Makefile for project 
###############################################################################

## General options
PROJECT = 
MCU = ATmega8
COMPILER = "WinAVR"
TARGET = Debug
CLOCK = 4000000
SHELL = C:\Windows\system32\cmd.exe

## Tools general options
CCFLAGS=-Wall -gdwarf-2 -fsigned-char -MD -MP -DF_CPU=4000000 -O1 -mmcu=atmega8
LDFLAGS=-mmcu=atmega8

## Processing Tools
CC=avr-gcc.exe
ASM=avr-gcc.exe
LD=avr-gcc.exe
#  E:/work/AVR_PROJECT/Remote_Dimmer/remote_Dimming/Debug/Debug.elf: TOOL=LD
#  Post: $(OBJ) avr-objcopy -O ihex -R .eeprom "Debug/Debug.elf" "Debug/Debug.hex"
#  Post: $(OBJ) avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex "Debug/Debug.elf" "Debug/Debug.eep" || exit 0 
#     Debug/remote_Dimming.o: TOOL=CC
#        E:/work/AVR_PROJECT/Remote_Dimmer/remote_Dimming/remote_Dimming.c

# Build tree:
all:	Debug

Debug:	Debug/Debug.elf

Debug/Debug.elf:	Debug/remote_Dimming.o 
	$(LD) $(LDFLAGS)  -o "Debug/Debug.elf" "Debug/remote_Dimming.o"
	$(OBJ) avr-objcopy -O ihex -R .eeprom "Debug/Debug.elf" "Debug/Debug.hex"
	$(OBJ) avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex "Debug/Debug.elf" "Debug/Debug.eep" || exit 0 

Debug/remote_Dimming.o:	remote_Dimming.c 
	$(CC) $(CCFLAGS)  -o "Debug/remote_Dimming.o" -c "remote_Dimming.c"

# tidy - delete all temporary files which are not involved in the target generation
tidy:
	rm -rf Debug/remote_Dimming.d

# cleanup - delete all generated files
clean:	tidy
	rm -rf Debug/Debug.elf
	rm -rf Debug/remote_Dimming.o
