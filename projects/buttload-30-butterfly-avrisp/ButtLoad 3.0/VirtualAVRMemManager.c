/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#include "VirtualAVRMemManager.h"

static volatile uint8_t  VAMMSetup        = VAMM_SETUP_NA;
				uint8_t  EraseDataflash   = FALSE;

// ======================================================================================

/*
 NAME:      | VAMM_EraseAVRMemory
 PURPOSE:   | Erases the virtual AVR
 ARGUMENTS: | None
 RETURNS:   | None
*/
void VAMM_EraseAVRMemory(uint8_t Mode)
{
	if (Mode == VAMM_ERASE_STOREPACKET)
	{
		for (uint8_t PacketB = 1; PacketB < 7; PacketB++)       // Save the erase chip command bytes to EEPROM
		  eeprom_write_byte(&EEPROMVars.EraseChip[PacketB - 1], PacketBytes[PacketB]);

		eeprom_write_byte(&EEPROMVars.EraseCmdStored, TRUE);
	}
	else
	{
		uint8_t CurrBlock = (DF_DATAFLASH_BLOCKS - 1);

		eeprom_write_byte(&EEPROMVars.StoredData, FALSE);
		eeprom_write_byte(&EEPROMVars.StoredEEPROM, FALSE);

		do
		{
			DF_EraseBlock(CurrBlock);
			LCD_Bargraph((DF_DATAFLASH_BLOCKS - CurrBlock) / (uint8_t)(DF_DATAFLASH_BLOCKS / LCD_BARGRAPH_SIZE));
		}
		while (CurrBlock--);
		
		LCD_Bargraph(0);
	}
}

/*
 NAME:      | VAMM_SetAddress
 PURPOSE:   | Sets the memory address in the virtual AVR
 ARGUMENTS: | None
 RETURNS:   | None
*/
void VAMM_SetAddress(void)
{
	uint32_t StartAddress;
	
	BYTE(StartAddress, 0) = BYTE(CurrAddress, 0);           // Only need the lower 24 bits (address portion)
	BYTE(StartAddress, 1) = BYTE(CurrAddress, 1);           // So save time and space by only copying the
	BYTE(StartAddress, 2) = BYTE(CurrAddress, 2);           // needed bytes
	BYTE(StartAddress, 3) = 0x00;

	VAMM_Cleanup();

	if (MemoryType == TYPE_FLASH)                           // Type 1 = Flash
	  StartAddress <<= 1;                                   // Convert flash word address to byte address
	else                                                    // Type 2 = EEPROM
	  StartAddress  += SM_EEPROM_OFFSET;                    // EEPROM uses byte addresses, and starts at the 257th kilobyte in Dataflash

	DataflashInfo.CurrPageAddress = (uint16_t)(StartAddress / DF_INTERNALDF_BUFFBYTES);
	DataflashInfo.CurrBuffByte    = (uint16_t)(StartAddress % DF_INTERNALDF_BUFFBYTES);

	VAMMSetup = VAMM_SETUP_ADDR_DONE;
}

/*
 NAME:      | VAMM_StoreByte
 PURPOSE:   | Stores a byte in the virtual AVR at the current address
 ARGUMENTS: | Byte to store
 RETURNS:   | None
*/
void VAMM_StoreByte(const uint8_t Data)
{
	if ((VAMMSetup != VAMM_SETUP_WRITE) || (DataflashInfo.CurrBuffByte == DF_INTERNALDF_BUFFBYTES)) // End of dataflash page or not set up for writing yet
	{
		EraseDataflash = FALSE;
	
		VAMM_SetAddress();

		DF_CopyPage(DataflashInfo.CurrPageAddress, DF_FLASH_TO_BUFFER); // Page contains data already, copy it into the buffer
		DF_BufferWriteEnable(DataflashInfo.CurrBuffByte);   // Set write point to the correct address in the current page

		VAMMSetup = VAMM_SETUP_WRITE;
	}

	if ((DataflashInfo.CurrBuffByte & 0x01) || (MemoryType == TYPE_EEPROM))
	  V2P_IncrementCurrAddress();

	DataflashInfo.CurrBuffByte++;

	SPI_SPITransmit(Data);                                  // Store the byte, dataflash is in write mode due to DF_BufferWriteEnable
}

/*
 NAME:      | VAMM_ReadByte
 PURPOSE:   | Reads a byte in the virtual AVR at the current address
 ARGUMENTS: | None
 RETURNS:   | Byte at current address within the virtual AVR's memory
*/
uint8_t VAMM_ReadByte(void)
{
	if ((VAMMSetup != VAMM_SETUP_READ) || (DataflashInfo.CurrBuffByte == DF_INTERNALDF_BUFFBYTES))
	{
		VAMM_SetAddress();

		DF_ContinuousReadEnable(DataflashInfo.CurrPageAddress, DataflashInfo.CurrBuffByte);
		
		VAMMSetup = VAMM_SETUP_READ;
	}

	if ((DataflashInfo.CurrBuffByte & 0x01) || (MemoryType == TYPE_EEPROM))
	  V2P_IncrementCurrAddress();

	DataflashInfo.CurrBuffByte++;

	return SPI_SPITransmit(0x00);
}

/*
 NAME:      | VAMM_ReadConsec
 PURPOSE:   | Reads bytes consecutively from the dataflash, without modifying CurrAddress
 ARGUMENTS: | None
 RETURNS:   | Byte at next address within the dataflash
*/
uint8_t VAMM_ReadConsec(void)
{
	if (VAMMSetup != VAMM_SETUP_READ)
	{
		VAMM_SetAddress();

		DF_ContinuousReadEnable(DataflashInfo.CurrPageAddress, DataflashInfo.CurrBuffByte);

		VAMMSetup = VAMM_SETUP_READ;
	}	

	if (DataflashInfo.CurrBuffByte == DF_INTERNALDF_BUFFBYTES)
	{
		DataflashInfo.CurrPageAddress++;
		DataflashInfo.CurrBuffByte = 0;
	}

	DataflashInfo.CurrBuffByte++;

	return SPI_SPITransmit(0x00);
}

/*
 NAME:      | VAMM_Cleanup
 PURPOSE:   | Cleans up after data storage - writes any data in the dataflash buffer to the main dataflash memory
 ARGUMENTS: | None
 RETURNS:   | None
*/
void VAMM_Cleanup(void)
{
	if (VAMMSetup == VAMM_SETUP_WRITE)                      // Save partially written page if in write mode
	{
		if (DF_BufferCompare(DataflashInfo.CurrPageAddress) == DF_COMPARE_MISMATCH) // Compare so that write is only executed if page data is different
		  DF_CopyPage(DataflashInfo.CurrPageAddress, DF_BUFFER_TO_FLASH_NE);
	}

	VAMMSetup = VAMM_SETUP_NA;
}
