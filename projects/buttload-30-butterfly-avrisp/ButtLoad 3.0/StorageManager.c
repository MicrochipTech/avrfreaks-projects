/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#define  INC_FROM_SM
#include "StorageManager.h"

// GLOBAL VARIABLES:
       uint8_t  MemoryType           = TYPE_FLASH;
static uint8_t  CurrentMode          = SM_NO_SETUP;
static uint16_t GPageLength          = 0;
static uint8_t  WriteCmdStored       = FALSE;
static uint8_t  PageLengthFound      = FALSE;

const char StorageText[] PROGMEM     = "*STORAGE MODE*";

// ======================================================================================

/*
 NAME:      | SM_GetStoredDataSize
 PURPOSE:   | Returns the size of the requested stored data type
 ARGUMENTS: | Type of data to get the size of (TYPE_FLASH or TYPE_EEPROM)
 RETURNS:   | Size of requested data type in bytes
*/
uint32_t SM_GetStoredDataSize(const uint8_t Type)
{
	uint16_t TargetPageLength;
	uint32_t ProgDataSize = 0;
	int16_t PageStart;
	int16_t PageEnd;
	
	DF_BufferWriteEnable(0);

	for (uint16_t b = 0; b < DF_INTERNALDF_BUFFBYTES; b++) // Erase buffer for page empty comparisons
	  SPI_SPITransmit(0xFF);

	if (Type == TYPE_FLASH)
	{
		PageStart = 0;
		PageEnd   = (DF_DATAFLASH_PAGES / 2);
		TargetPageLength = eeprom_read_word(&EEPROMVars.PageLength);
	}
	else
	{
		PageStart = (DF_DATAFLASH_PAGES / 2);
		PageEnd   = DF_DATAFLASH_PAGES;
		TargetPageLength = eeprom_read_word(&EEPROMVars.EPageLength);
	}

	for (int16_t CurrPage = (PageEnd - 1); CurrPage >= PageStart; CurrPage--)
	{
		if (DF_BufferCompare(CurrPage) == DF_COMPARE_MISMATCH)          // Found last non-empty page
		{
			ProgDataSize = (((uint32_t)(CurrPage - PageStart)) * DF_INTERNALDF_BUFFBYTES);

			for (int16_t CurrByte = (DF_INTERNALDF_BUFFBYTES - 1); CurrByte >= 0; CurrByte--) // Found last non-blank byte
			{
				DF_ContinuousReadEnable(CurrPage, CurrByte);

				if (SPI_SPITransmit(0x00) != 0xFF)                      // Find last used byte
				{
					ProgDataSize += (CurrByte + 1);
					
					break;
				}
			}

			ProgDataSize += (ProgDataSize % TargetPageLength);          // Round up to nearest page size

			break;
		}
	}
	
	return ProgDataSize;
}

/*
 NAME:      | SM_InterpretAVRISPPacket
 PURPOSE:   | Manages the interpretation of V2Protocol packets for STORE PRGM mode operation
 ARGUMENTS: | None
 RETURNS:   | None
*/
void SM_InterpretAVRISPPacket(void)
{
	uint8_t* EEPROMAddress;

	switch (PacketBytes[0])
	{
		case AICB_CMD_ENTER_PROGMODE_ISP:
			MessageSize = 2;
						
			for (uint8_t PacketB = 0; PacketB < 12; PacketB++)          // Save the enter programming mode command bytes
			  eeprom_write_byte(&EEPROMVars.EnterProgMode[PacketB], PacketBytes[PacketB]);
			
			DF_ENABLEDATAFLASH(TRUE);
			SPI_SPIInit();

			InProgrammingMode = TRUE;                                   // Set the flag, prevent the user from exiting the V2P state machine			
			CurrentMode = SM_NO_SETUP;
			WriteCmdStored = FALSE;
			V2P_ClearCurrAddress();

			MAIN_SETSTATUSLED(MAIN_STATLED_RED);
			PacketBytes[1] = AICB_STATUS_CMD_OK;

			break;			
		case AICB_CMD_LEAVE_PROGMODE_ISP:
			MessageSize = 2;
			
			SM_CheckEndOfFuseLockData();                                // Check for remaining bytes to be stored and general cleanup
			VAMM_Cleanup();

			if (InProgrammingMode == FALSE)                             // Flag is reset if an error occured in STORAGE MODE
			  TG_PlayToneSeq(TONEGEN_SEQ_ERROR);			
			else if (EraseDataflash)
			  TG_PlayToneSeq(TONEGEN_SEQ_ERASINGDF);
			else
			  TG_PlayToneSeq(TONEGEN_SEQ_PROGDONE);
		
			InProgrammingMode = FALSE;                                  // Clear the flag, allow the user to exit the V2P state machine

			DF_ENABLEDATAFLASH(FALSE);
			SPI_SPIOFF();

			MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);
			PacketBytes[1] = AICB_STATUS_CMD_OK;

			break;
		case AICB_CMD_READ_SIGNATURE_ISP:
			MessageSize = 4;

			PacketBytes[1] = AICB_STATUS_CMD_OK;                        // Data byte is encased in CMD_OKs
			PacketBytes[2] = 0x01;                                      // Signature bytes all return 0x01 in storage mode
			PacketBytes[3] = AICB_STATUS_CMD_OK;                        // Data byte is encased in CMD_OKs

			break;
		case AICB_CMD_CHIP_ERASE_ISP:
			MessageSize = 2;

			EraseDataflash = TRUE;
			VAMM_EraseAVRMemory(VAMM_ERASE_STOREPACKET);
			PM_SetProgramDataType(PM_OPT_CLEARFLAGS);
			
			PacketBytes[1] = AICB_STATUS_CMD_OK;
			
			break;
		case AICB_CMD_READ_OSCCAL_ISP:
			MessageSize = 4;

			PacketBytes[1] = AICB_STATUS_CMD_OK;                        // Data byte is encased in CMD_OKs
			PacketBytes[2] = 0x00;                                      // Return 0x00 for the OSCCAL byte in storage mode
			PacketBytes[3] = AICB_STATUS_CMD_OK;                        // Data byte is encased in CMD_OKs

			break;
		case AICB_CMD_PROGRAM_FUSE_ISP:
		case AICB_CMD_PROGRAM_LOCK_ISP:
			MessageSize = 3;

			if (CurrentMode != SM_LOCKFUSEBITS_WRITE)                   // First lock or fuse byte being written, set the EEPROM pointer
			{
				CurrentMode   = SM_LOCKFUSEBITS_WRITE;
				DataflashInfo.CurrBuffByte  = 0;                        // CurrBuffByte is used to store the total fuse/lock bytes written
			}

			if (PacketBytes[0] == AICB_CMD_PROGRAM_FUSE_ISP)
			{
				EEPROMAddress = &EEPROMVars.FuseBytes[DataflashInfo.CurrBuffByte][0];
				MemoryType    = TYPE_FUSE;
				PM_SetProgramDataType(PM_OPT_FUSE);
			}
			else
			{
				EEPROMAddress = &EEPROMVars.LockBytes[DataflashInfo.CurrBuffByte][0];
				MemoryType    = TYPE_LOCK;
				PM_SetProgramDataType(PM_OPT_LOCK);
			}				
			
			if (DataflashInfo.CurrBuffByte < SM_MAX_FUSELOCKBITS)
			{
				for (uint8_t FLByte = 1; FLByte < 5; FLByte++)
				{
					eeprom_write_byte(EEPROMAddress, PacketBytes[FLByte]);
					EEPROMAddress++;
				}

				DataflashInfo.CurrBuffByte++;                           // Increment the total fuse/lock bytes written counter
			}
			
			PacketBytes[1] = AICB_STATUS_CMD_OK;                        // Two CMD_OKs are always returned
			PacketBytes[2] = AICB_STATUS_CMD_OK;                        // Two CMD_OKs are always returned

			break;
		case AICB_CMD_READ_FUSE_ISP:
		case AICB_CMD_READ_LOCK_ISP:
			MessageSize = 4;
	
			if (CurrentMode != SM_LOCKFUSEBITS_READ)                    // First lock or fuse byte being read, set the EEPROM pointer
			{
				SM_CheckEndOfFuseLockData();                            // Check for remaining bytes to be stored and general cleanup
				
				DataflashInfo.CurrBuffByte = 0;
				CurrentMode  = SM_LOCKFUSEBITS_READ;
			}
			
			if (DataflashInfo.CurrBuffByte > eeprom_read_byte((PacketBytes[0] == AICB_CMD_READ_FUSE_ISP)? &EEPROMVars.TotalFuseBytes : &EEPROMVars.TotalLockBytes))  // Trying to read more fuse/lock bytes than are stored in memory
			{
				PacketBytes[2] = 0xFF;                                  // Return 0xFF for the fuse/lock byte
			}
			else
			{
				uint8_t FuseLockNum  = DataflashInfo.CurrBuffByte;
				uint8_t FuseLockByte = (PacketBytes[1] - 1);
				
				PacketBytes[2] = eeprom_read_byte((PacketBytes[0] == AICB_CMD_READ_FUSE_ISP)? &EEPROMVars.FuseBytes[FuseLockNum][FuseLockByte] : &EEPROMVars.LockBytes[FuseLockNum][FuseLockByte]);
			}

			DataflashInfo.CurrBuffByte++;

			PacketBytes[1] = AICB_STATUS_CMD_OK;                        // Data byte is encased in CMD_OKs
			PacketBytes[3] = AICB_STATUS_CMD_OK;                        // Data byte is encased in CMD_OKs

			break;
		case AICB_CMD_PROGRAM_FLASH_ISP:
		case AICB_CMD_PROGRAM_EEPROM_ISP:
			MessageSize = 2;

			if (CurrentMode != SM_DATAFLASH_WRITE)                      // First programming packet
			{
				uint8_t NeedToEraseDF = FALSE;
			
				if (PacketBytes[0] == AICB_CMD_PROGRAM_FLASH_ISP)       // Flash programming mode
				{
					if (eeprom_read_byte(&EEPROMVars.StoredData) == TRUE)
					  NeedToEraseDF = TRUE;
					else
					  eeprom_write_byte(&EEPROMVars.StoredData, TRUE);

					EEPROMAddress = (uint8_t*)&EEPROMVars.WriteProgram;
					PM_SetProgramDataType(PM_OPT_FLASH);
					MemoryType  = TYPE_FLASH;
				}
				else                                                    // EEPROM programming mode
				{
					if (eeprom_read_byte(&EEPROMVars.StoredEEPROM) == TRUE)
					  NeedToEraseDF = TRUE;
					else
					  eeprom_write_byte(&EEPROMVars.StoredEEPROM, TRUE);

					EEPROMAddress = (uint8_t*)&EEPROMVars.WriteEEPROM;
					PM_SetProgramDataType(PM_OPT_EEPROM);
					MemoryType  = TYPE_EEPROM;
				}

				if (NeedToEraseDF)
				{
					LCD_PutStr_f(PSTR("ERASE NEEDED"));
					
					InProgrammingMode = FALSE;
					PacketBytes[1] = AICB_STATUS_CMD_FAILED;

					break;
				}

				GPageLength = 0;
				PageLengthFound	= FALSE;
				CurrentMode = SM_DATAFLASH_WRITE;
				
				if (!(WriteCmdStored))
				{
					for (uint8_t StoreByte = 0; StoreByte < 10; StoreByte++)  // Save the command bytes
					{
						eeprom_write_byte(EEPROMAddress, PacketBytes[StoreByte]);
						EEPROMAddress++;
					}
					
					WriteCmdStored = TRUE;
				}
			}

			uint16_t BytesToWrite = (((uint16_t)PacketBytes[1] << 8) | PacketBytes[2]);

			for (uint16_t CurrByte = 0; CurrByte < BytesToWrite; CurrByte++)
			{
				VAMM_StoreByte(PacketBytes[10 + CurrByte]);             // Store the program's bytes into the dataflash
				GPageLength++;
			}

			if (!(PageLengthFound) && (PacketBytes[3] & ISPCC_PROG_MODE_PAGEDONE)) // Page done - save the target's memory page length to EEPROM for later use if not already done so
			{
				eeprom_write_word(((MemoryType == TYPE_FLASH)? &EEPROMVars.PageLength : &EEPROMVars.EPageLength), GPageLength);

				PageLengthFound = TRUE;
			}

			PacketBytes[1] = AICB_STATUS_CMD_OK;
		
			break;
		case AICB_CMD_READ_FLASH_ISP:
		case AICB_CMD_READ_EEPROM_ISP:	
			if (CurrentMode != SM_DATAFLASH_READ)
			{
				SM_CheckEndOfFuseLockData();                            // Check for remaining bytes to be stored and general cleanup				
				CurrentMode = SM_DATAFLASH_READ;

				if (PacketBytes[0] == AICB_CMD_READ_FLASH_ISP)          // Flash reading mode
				  MemoryType = TYPE_FLASH;
				else                                                    // EEPROM reading mode
				  MemoryType = TYPE_EEPROM;
			}

			uint16_t BytesToRead = (((uint16_t)PacketBytes[1] << 8) | PacketBytes[2]);
	 
			for (uint16_t ReadByte = 0; ReadByte < BytesToRead; ReadByte++)
			  PacketBytes[2 + ReadByte] = VAMM_ReadByte();              // Read in the next dataflash byte if present

			MessageSize = BytesToRead + 3;

			PacketBytes[1]               = AICB_STATUS_CMD_OK;          // Return data should be encompassed in STATUS_CMD_OKs
			PacketBytes[2 + BytesToRead] = AICB_STATUS_CMD_OK;          // Return data should be encompassed in STATUS_CMD_OKs
		
			break;
		default:
			MessageSize = 2;
			
			PacketBytes[1] = AICB_STATUS_CMD_UNKNOWN;
	}

	V2P_SendPacket();                                                   // Send the response packet

	if (EraseDataflash && !(InProgrammingMode))
	{
		LCD_PutStr_f(BusyText);

		DF_ENABLEDATAFLASH(TRUE);
		SPI_SPIInit();
		MAIN_SETSTATUSLED(MAIN_STATLED_ORANGE);
	
		VAMM_EraseAVRMemory(VAMM_ERASE_ERASEDATA);

		MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);
		SPI_SPIOFF();
		DF_ENABLEDATAFLASH(FALSE);

		LCD_PutStr_f(StorageText);

		EraseDataflash = FALSE;
		TG_PlayToneSeq(TONEGEN_SEQ_PROGDONE);
	}
}

/*
 NAME:      | SM_CheckEndOfFuseLockData (static)
 PURPOSE:   | Checks if last operation was the writing of fuse/lock bytes and if so writes total stored bytes to EEPROM
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void SM_CheckEndOfFuseLockData(void)
{
	if (CurrentMode == SM_LOCKFUSEBITS_WRITE)
	{
		// CurrBuffByte stores the total number of fuse/lock bytes written in this case:
		eeprom_write_byte(((MemoryType == TYPE_FUSE)? &EEPROMVars.TotalFuseBytes : &EEPROMVars.TotalLockBytes), DataflashInfo.CurrBuffByte);
	}
}

