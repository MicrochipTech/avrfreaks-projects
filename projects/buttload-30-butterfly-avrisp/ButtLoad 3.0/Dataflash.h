/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef DATAFLASH_H
#define DATAFLASH_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	
	#include "DataflashCommandBytes.h"
	#include "SPI.h"
	
	// TYPE DEFINITIONS:
	typedef struct
	{
		uint16_t   CurrPageAddress;
		uint16_t   CurrBuffByte;
	} DFinfo;
	
	// DEFINES AND MACROS:
	#define DF_TOGGLEENABLE()         MACROS{ DF_ENABLEDATAFLASH(FALSE); DF_ENABLEDATAFLASH(TRUE); }MACROE
	#define DF_ENABLEDATAFLASH(x)     MACROS{ if (x == TRUE) { PORTB &= ~(1 << 0); } else { PORTB |= (1 << 0); } }MACROE
	
	#define DF_COMPAREMASK            (1 << 6)
	#define DF_BUSYMASK               (1 << 7)
	#define DF_AT45DB041B_DENSITYMASK (3 << 3)
	#define DF_DFINFOMASK             0x38
	#define DF_INTERNALDF_BUFFBYTES   264
	#define DF_DATAFLASH_PAGES        2048
	#define DF_DATAFLASH_BLOCKS       256

	#define DF_PAGESHIFT_HIGH         (16 - 9)
	#define DF_PAGESHIFT_LOW          ( 9 - 8)
	#define DF_BUFFERSHIFT            8
	
	#define DF_BUFFER_TO_FLASH_NE     DFCB_BUF1TOFLASH
	#define DF_BUFFER_TO_FLASH        DFCB_BUF1TOFLASHWE
	#define DF_FLASH_TO_BUFFER        DFCB_FLASHTOBUF1TRANSFER

	#define DF_COMPARE_MATCH          0
	#define DF_COMPARE_MISMATCH       DF_COMPAREMASK
	
	// GLOBAL VARIABLES:
	extern DFinfo                     DataflashInfo;
	
	// PROTOTYPES:
	uint8_t DF_CheckCorrectOnboardChip(void) ATTR_WARN_UNUSED_RESULT;
	void    DF_EraseBlock(const uint8_t BlockToErase);
	void    DF_CopyPage(const uint16_t PageAddress, uint8_t Operation);
	void    DF_ContinuousReadEnable(const uint16_t PageAddress, const uint16_t BuffAddress);
	void    DF_BufferWriteEnable(const uint16_t BuffAddress);
	uint8_t DF_BufferCompare(const uint16_t PageAddress) ATTR_WARN_UNUSED_RESULT;

	#if defined(INC_FROM_DF)
	  static void DF_WaitWhileBusy(void);
	#endif
	
#endif
