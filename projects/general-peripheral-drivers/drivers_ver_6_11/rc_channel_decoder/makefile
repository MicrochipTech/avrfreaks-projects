#Compatible with all versions of AVRGCC makefile (i hope!)
#Written by Chris Efstathiou 
#Date 20/Dec/2002


###### define some variables based on the AVR base path in $(AVR) #######
	CC	= avr-gcc
	AS	= avr-gcc -x assembler-with-cpp	
	RM	= rm -f
	RN	= mv
	BIN	= avr-objcopy
	SIZE	= avr-size
#Use the below commented line instead of the above line if you use avr-sizex
#	SIZE	= avr-sizex
	INCDIR	= .
	LIBDIR	= $(AVR)/avr/lib



######################################################################################
#           U S E R   C O N F I G U R A T I O N  B L O C K  S T A R T                  
######################################################################################

#put the AVRGCC version here (needed for finding the correct header files)
#	AVRGCC_VERSION	= 330

#put the MCU clock Frequency in Hertz here (This setting is passed to all files)
	F_CPU	= 8000000

#put the name of the target mcu here (at90s8515, at90s8535, attiny22, atmega603 etc.)
	MCU = attiny26

#put the name of the target file here (without extension)
        TRG     = test_rc_channel

#put your C sourcefiles here 
        SRC     = $(TRG).c rc_channel.c 
        
#put additional assembler source file here
        ASRC    = 

#additional libraries and object files to link
        LIB     =

#additional includes to compile
        INC     = 

###### output format can be srec, ihex (avrobj is always created) #######
	FORMAT	= ihex

######################################################################################
#           U S E R   C O N F I G U R A T I O N  B L O C K  E N D                  
######################################################################################

########## Normally you shouldn't need to change anything in the below lines #########

#compiler flags
        CPFLAGS = -g -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wa,-ahlms=$(<:.c=.lst)

#Assembler flags
	ASFLAGS = -Wa,-gstabs

#linker flags
        LDFLAGS = -Wl,-Map=$(TRG).map,--cref


#define all project specific object files
	OBJ	= $(ASRC:.s=.o) $(SRC:.c=.o) 
	CPFLAGS += -mmcu=$(MCU)
	ASFLAGS += -mmcu=$(MCU)
	LDFLAGS += -mmcu=$(MCU)

#Pass the MPU clock frequency and version info to flags
#	ASFLAGS += -DAVRGCC_VERSION=$(AVRGCC_VERSION)
#	CPFLAGS += -DAVRGCC_VERSION=$(AVRGCC_VERSION)
	ASFLAGS += -DF_CPU=$(F_CPU)
	CPFLAGS += -DF_CPU=$(F_CPU)
  
#this defines the aims of the make process
all:	$(TRG).elf $(TRG).hex $(TRG).eep $(TRG).ok 


#compile: instructions to create assembler and/or object files from C source
%o : %c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

%s : %c
	$(CC) -S $(CPFLAGS) -I$(INCDIR) $< -o $@


#assemble: instructions to create object file from assembler files
%o : %s
	$(AS) -c $(ASFLAGS) -I$(INCDIR) $< -o $@


#link: instructions to create elf output file from object files
%elf: $(OBJ)
	$(CC) $(OBJ) $(LIB) $(LDFLAGS) -o $@

#create bin (ihex, srec) file from elf output file
%hex: %elf
	$(BIN) -O $(FORMAT) -R .eeprom $< $@

%eep: %elf
	$(BIN) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O $(FORMAT) $< $@



		
#If all other steps compile ok then echo "Errors: none".
#Necessary for AVR Studio to understand that everything went ok.
%ok:
	$(SIZE)  $(TRG).elf
#Use the below commented line instead of the above line if you use avr-sizex
#	$(SIZE)  --long --mcu $(MCU) $(TRG).elf
	@echo "Errors: none"
 

#make instruction to delete created files

clean:
	$(RM) $(OBJ)
	$(RM) $(SRC:.c=.s)
	$(RM) $(SRC:.c=.lst)
	$(RM) $(TRG).map
	$(RM) $(TRG).elf
	$(RM) $(TRG).obj
	$(RM) $(TRG).a90
	$(RM) $(TRG).hex
	$(RM) $(TRG).eep
	$(RM) *.bak
	$(RM) *.log
	
size:
	$(SIZE)  $(TRG).elf
#Use the below commented line instead of the above line if you use avr-sizex
#	$(SIZE)  --long --mcu $(MCU) $(TRG).elf


######################################################################################
#           U S E R   C O N F I G U R A T I O N  B L O C K  S T A R T                  
######################################################################################
###### dependecies, add any dependencies you need here                     ###########
###### IF A FILE FROM THE ONES LISTED BELOW IS EDITED A REBUILD IS FORCED  ########### 
######################################################################################

$(TRG).o   : $(TRG).c  delay.h rc_channel.c rc_channel.h
#rc_channel.o      : rc_channel.c 


######################################################################################
#           U S E R   C O N F I G U R A T I O N  B L O C K  E N D                  
######################################################################################





