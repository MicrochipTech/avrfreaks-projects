# ***************************************************************************
#
#  Module:       $Filename: D:\Projects\micro\atmel\harddisk\command.mak $
#
#  Author:       $Author: SH $
#
#  Revision:     $Revision: 1.0 $
#
#  Version:      $Version: NONE $
#
#  Date:         $Date: Mon May 01 14:06:44 2000 $
#
#  Description:  AVR Makefile taken from Volker Oth's simple makefile
#
# ***************************************************************************
#
#  History:
#
#  $Log: Q:\archive\harddisk\command.nbl $
# 
#   Auto-added
# 
# Revision 1.0  by: SH  Rev date: Tue Jun 06 23:12:40 2000
#   Initial revision.
# 
# $Endlog$
#
# ***************************************************************************

# ***************************************************************************
#  All project specific object files
# ***************************************************************************
	OBJ	= $(ASRC:.s=.o) $(SRC:.c=.o) 
	CPFLAGS += -mmcu=$(MCU)
	ASFLAGS += -mmcu=$(MCU)
	LDFLAGS += -mmcu=$(MCU)

# ***************************************************************************
#  Target definitions
# ***************************************************************************
all:	$(TRG).obj $(TRG).rom

clean:
	$(RM) $(OBJ)
	$(RM) $(SRC:.c=.s)
	$(RM) $(SRC:.c=.lst)
	$(RM) $(TRG).map
	$(RM) $(TRG).elf
	$(RM) $(TRG).obj
	$(RM) $(TRG).eep
	$(RM) $(TRG).rom
	$(RM) *.bak
	$(RM) *.log


# ***************************************************************************
#  compile: create assembler and/or object files from C source
# ***************************************************************************
%o : %c 
	$(CC) -c $(CPFLAGS) -I$(INCDIR) $< -o $@

%s : %c
	$(CC) -S $(CPFLAGS) -I$(INCDIR) $< -o $@

# ***************************************************************************
# assemble: create object file from assembler files
# ***************************************************************************
%o : %s
	$(AS) $(ASFLAGS) -I$(INCDIR) $< -o $@

# ***************************************************************************
# link: create elf and rom output file from object files
# ***************************************************************************
%elf: $(OBJ)
	$(CC) $(OBJ) $(LIB) $(LDFLAGS) -o $@

%obj: %elf
	$(BIN) -O avrobj $< $@

%rom: %elf
	$(BIN) -O $(FORMAT) $< $@
	$(BIN) -j .eeprom --set-section-flags=.eeprom="alloc,load" -O $(FORMAT) $< $(@:.rom=.eep)


