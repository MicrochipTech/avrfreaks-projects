/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

/*
	Compatible with most Atmel dataflash memory devices. This is a re-written, bare-bones
	version of the generic Atmel Dataflash driver, with some specific ButtLoad routines
	added in.
*/

#define  INC_FROM_DF
#include "Dataflash.h"

const char     DataFlashError[] PROGMEM = "DATAFLASH ERROR";
DFinfo         DataflashInfo            = {CurrPageAddress: 0, CurrBuffByte: 0};

// ======================================================================================

/*
 NAME:      | DF_CheckCorrectOnboardChip
 PURPOSE:   | Checks for the existance, correct operation and type of the onboard dataflash chip
 ARGUMENTS: | None
 RETURNS:   | Boolean flag for whether the dataflash is valid and present
*/
uint8_t DF_CheckCorrectOnboardChip(void)
{
	DF_WaitWhileBusy();
	
	if (((SPI_SPITransmit(0x00) & DF_DFINFOMASK)) != DF_AT45DB041B_DENSITYMASK)
	{
		DF_ENABLEDATAFLASH(FALSE);
		SPI_SPIOFF();
		MAIN_ShowError(DataFlashError);
		
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*
 NAME:      | DF_EraseBlock
 PURPOSE:   | Erases a block of dataflash memory (8 pages)
 ARGUMENTS: | Block address to erase
 RETURNS:   | None
*/
void DF_EraseBlock(const uint8_t BlockToErase)
{
	DF_WaitWhileBusy();
	DF_TOGGLEENABLE();

	SPI_SPITransmit(DFCB_BLOCKERASE);
	SPI_SPITransmit((uint8_t)(BlockToErase >> 4));
	SPI_SPITransmit((uint8_t)(BlockToErase << 4));
	SPI_SPITransmit(0x00);
}

/*
 NAME:      | DF_CopyPage
 PURPOSE:   | Copys a page to or from the dataflash's internal buffer
 ARGUMENTS: | Page address to copy to/from, operation to perform (DF_BUFFER_TO_FLASH_NE, DF_BUFFER_TO_FLASH or DF_FLASH_TO_BUFFER)
 RETURNS:   | None
*/
void DF_CopyPage(const uint16_t PageAddress, uint8_t Operation)
{
	DF_WaitWhileBusy();
	DF_TOGGLEENABLE();
	
	SPI_SPITransmit(Operation);
	SPI_SPITransmit((uint8_t)(PageAddress >> DF_PAGESHIFT_HIGH));
	SPI_SPITransmit((uint8_t)(PageAddress << DF_PAGESHIFT_LOW));
	SPI_SPITransmit(0x00);		
}

/*
 NAME:      | DF_BufferWriteEnable
 PURPOSE:   | Prepares the dataflash's internal buffer for write operations
 ARGUMENTS: | Address in the dataflash's buffer to begin writing from
 RETURNS:   | None
*/
void DF_BufferWriteEnable(const uint16_t BuffAddress)
{
	DF_WaitWhileBusy();
	DF_TOGGLEENABLE();

	SPI_SPITransmit(DFCB_BUF1WRITE);
	SPI_SPITransmit(0x00);
	SPI_SPITransmit((uint8_t)(BuffAddress >> 8));
	SPI_SPITransmit((uint8_t)(BuffAddress));
}

/*
 NAME:      | DF_ContinuousReadEnable
 PURPOSE:   | Prepares the dataflash's flash memory for read operations
 ARGUMENTS: | Address in the dataflash's flash memory, address within the selected page to being reading from
 RETURNS:   | None
*/
void DF_ContinuousReadEnable(const uint16_t PageAddress, const uint16_t BuffAddress)
{
	DF_WaitWhileBusy();
	DF_TOGGLEENABLE();
	
	SPI_SPITransmit(DFCB_CONTARRAYREAD);
	SPI_SPITransmit((uint8_t)(PageAddress >> DF_PAGESHIFT_HIGH));
	SPI_SPITransmit((uint8_t)((PageAddress << DF_PAGESHIFT_LOW) | (BuffAddress >> DF_BUFFERSHIFT)));
	SPI_SPITransmit((uint8_t)(BuffAddress));
	
	for (uint8_t DByte = 0; DByte < 4; DByte++)  // Perform 4 dummy writes to intiate the DataFlash address pointers
	  SPI_SPITransmit(0x00);
}

/*
 NAME:      | DF_BufferCompare
 PURPOSE:   | Compares a page in dataflash memory with the contents of the dataflash's internal buffer
 ARGUMENTS: | Page address to compare with
 RETURNS:   | DF_COMPARE_MATCH if identical, otherwise DF_COMPARE_MISMATCH
*/
uint8_t DF_BufferCompare(const uint16_t PageAddress)
{
	DF_WaitWhileBusy();
	DF_TOGGLEENABLE();
	
	SPI_SPITransmit(DFCB_FLASHTOBUF1COMPARE);
	SPI_SPITransmit((uint8_t)(PageAddress >> DF_PAGESHIFT_HIGH));
	SPI_SPITransmit((uint8_t)(PageAddress << DF_PAGESHIFT_LOW));
	SPI_SPITransmit(0x00);

	DF_WaitWhileBusy();

	return (SPI_SPITransmit(0x00) & DF_COMPAREMASK);
}

/*
 NAME:      | DF_WaitWhileBusy (static)
 PURPOSE:   | Performs an infinite loop while the dataflash is busy completing an operation
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void DF_WaitWhileBusy(void)
{
	DF_TOGGLEENABLE();
	
	SPI_SPITransmit(DFCB_STATUSREG);
	
	while (!(SPI_SPITransmit(0x00) & DF_BUSYMASK));
}
