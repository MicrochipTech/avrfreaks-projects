
# ***************************************************************************
#
#  Module:       $Filename: D:\Projects\micro\atmel\harddisk\default.mak $
#
#  Author:       $Author: SH $
#
#  Revision:     $Revision: 1.0 $
#
#  Version:      $Version: NONE $
#
#  Date:         $Date: Mon May 01 13:58:56 2000 $
#
#  Description:  AVR default makefile, taken Volker Oth's simple makefile
#
# ***************************************************************************
#
#  History:
#
#  $Log: Q:\archive\harddisk\default.nbl $
# 
#   Auto-added
# 
# Revision 1.0  by: SH  Rev date: Tue Jun 06 23:12:42 2000
#   Initial revision.
# 
# $Endlog$
#
# ***************************************************************************

# ***************************************************************************
#   Standard variables base on AVR base in $(AVR)
# ***************************************************************************
CC	= avr-gcc
AS	= avr-gcc -x assembler-with-cpp	
RM	= rm -f
RN	= mv
BIN	= avr-objcopy
INCDIR	= .
LIBDIR	= $(AVR)/avr/lib
SHELL   = $(AVR)/bin/sh.exe

# ***************************************************************************
#   Standard output fromat, can be either 'srec' or 'ihex'
# ***************************************************************************
	FORMAT = srec

# ***************************************************************************
#   Standard MCU type e.g. at90s8515, at90s8535 
# ***************************************************************************
	MCU = at90s8515

# ***************************************************************************
#   Default compiler flags
# ***************************************************************************
	CPFLAGS	= -D__ATMEL__ -g -O3 -Wall -Wstrict-prototypes -Wa,-ahlms=$(<:.c=.lst)
	
# ***************************************************************************
#   Default assembler flags
# ***************************************************************************
	ASFLAGS = -gstabs

# ***************************************************************************
#   Default linker flags
# ***************************************************************************
	LDFLAGS = -Wl,-Map=$(TRG).map,--cref