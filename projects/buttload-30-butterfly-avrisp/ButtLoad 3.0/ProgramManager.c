/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#define  INC_FROM_PM
#include "ProgramManager.h"

// PROGMEM CONSTANTS:
const char ProgTypes[4][5] PROGMEM = {"DATA", "EPRM", "FUSE", "LOCK"};

// ======================================================================================

/*
 NAME:      | PM_ShowStoredItemSizes
 PURPOSE:   | Shows the size of the various data types stored in the dataflash onto the LCD
 ARGUMENTS: | None
 RETURNS:   | None
*/
void PM_ShowStoredItemSizes(void)
{
	char    Buffer[14];
	uint8_t ItemInfoIndex = 0;
	uint8_t TempB;
	
	JoyStatus = JOY_INVALID;                                            // Use an invalid joystick value to force the program to write the
	                                                                    // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_UP)
			  (ItemInfoIndex == 0)? ItemInfoIndex = 3 : ItemInfoIndex--;
			else if (JoyStatus & JOY_DOWN)
			  (ItemInfoIndex == 3)? ItemInfoIndex = 0 : ItemInfoIndex++;
			else if (JoyStatus & JOY_LEFT)
			  return;
		
			strcpy_P(Buffer, ProgTypes[ItemInfoIndex]);

			SPI_SPIInit();
			DF_ENABLEDATAFLASH(TRUE);

			if (!(DF_CheckCorrectOnboardChip()))
			  return;

			switch (ItemInfoIndex)
			{
				case 0:
					ultoa(SM_GetStoredDataSize(TYPE_FLASH), &Buffer[5], 10);
					break;
				case 1:
					ultoa(SM_GetStoredDataSize(TYPE_EEPROM), &Buffer[5], 10);
					break;
				case 2:
					TempB = eeprom_read_byte(&EEPROMVars.TotalFuseBytes);
					ultoa(((TempB == 0xFF)? 0x00 : TempB), &Buffer[5], 10);
					break;
				case 3:
					TempB = eeprom_read_byte(&EEPROMVars.TotalLockBytes);
					ultoa(((TempB == 0xFF)? 0x00 : TempB), &Buffer[5], 10);
			}

			DF_ENABLEDATAFLASH(FALSE);
			SPI_SPIOFF();
	
			Buffer[4] = '-';
			LCD_PutStr(Buffer);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | PM_StartProgAVR
 PURPOSE:   | Begins programming a target AVR from data stored in the dataflash
 ARGUMENTS: | None
 RETURNS:   | None
*/
void PM_StartProgAVR(void)
{
	uint8_t StoredLocksFuses;
	uint8_t ProgOptions = eeprom_read_byte(&EEPROMVars.PGOptions);
	char ProgTypeBuffer[7];

	if (!(ProgOptions) || (ProgOptions > 15))
	{
		MAIN_ShowError(PSTR("NOTHING SELECTED"));
		return;
	}
	else if (eeprom_read_byte(&EEPROMVars.SCKDuration) == 6) // Recovery ISP speed
	{
		MAIN_ShowError(PSTR("RECOVERY MODE"));
		return;		
	}

	LCD_PutStr(BusyText);
	
	DF_ENABLEDATAFLASH(TRUE);
	SPI_SPIInit();
	
	if (!(DF_CheckCorrectOnboardChip()))
	  return;

	TIMEOUT_SLEEP_TIMER_OFF();

	USI_SPIInitMaster();
	MAIN_SetTargetResetLine(MAIN_RESET_ACTIVE);        // Capture the RESET line of the slave AVR
			
	for (uint8_t PacketB = 0; PacketB < 12; PacketB++) // Read the enter programming mode command bytes
	  PacketBytes[PacketB] = eeprom_read_byte(&EEPROMVars.EnterProgMode[PacketB]);
		
	V2P_ClearCurrAddress();
	ProgrammingFault = ISPCC_NO_FAULT;

	strcpy_P(ProgTypeBuffer, PSTR("PRG>  "));
	
	ISPCC_EnterChipProgrammingMode();            // Try to sync with the slave AVR
	if (InProgrammingMode)                       // ISPCC_EnterChipProgrammingMode alters the InProgrammingMode flag
	{						
		if ((ProgOptions & PM_OPT_FLASH) && (ProgrammingFault == ISPCC_NO_FAULT))
		{
			ProgTypeBuffer[5] = 'C';
			LCD_PutStr(ProgTypeBuffer);
			
			if (!(eeprom_read_byte(&EEPROMVars.EraseCmdStored) == TRUE))
			{
				ProgrammingFault = ISPCC_FAULT_NOERASE;
				MAIN_ShowError(PSTR("NO ERASE CMD"));
			}
			else
			{
				PM_SendEraseCommand();
			}
		}

		if ((ProgOptions & PM_OPT_FLASH) && (ProgrammingFault == ISPCC_NO_FAULT))
		{
			ProgTypeBuffer[5] = 'D';
			LCD_PutStr(ProgTypeBuffer);

			if (eeprom_read_byte(&EEPROMVars.StoredData) != TRUE) // Check to make sure a program is present in memory
			{
				ProgrammingFault = ISPCC_FAULT_NODATATYPE;					
				MAIN_ShowError(PSTR("NO DATA"));
			}
			else
			{
				MemoryType = TYPE_FLASH;
				PM_CreateProgrammingPackets();
			}
		}
	
		if ((ProgOptions & PM_OPT_EEPROM) && (ProgrammingFault == ISPCC_NO_FAULT))
		{
			ProgTypeBuffer[5] = 'E';
			LCD_PutStr(ProgTypeBuffer);

			if (eeprom_read_byte(&EEPROMVars.StoredEEPROM) != TRUE) // Check to make sure EEPROM data is present in memory
			{
				ProgrammingFault = ISPCC_FAULT_NODATATYPE;
				MAIN_ShowError(PSTR("NO EEPROM"));
			}
			else
			{
				MemoryType = TYPE_EEPROM;
				PM_CreateProgrammingPackets();
			}
		}

		if ((ProgOptions & PM_OPT_FUSE) && (ProgrammingFault == ISPCC_NO_FAULT))
		{
			ProgTypeBuffer[5] = 'F';
			LCD_PutStr(ProgTypeBuffer);
			
			StoredLocksFuses = eeprom_read_byte(&EEPROMVars.TotalFuseBytes);
			if (!(StoredLocksFuses) || (StoredLocksFuses == 0xFF))
			{
				ProgrammingFault = ISPCC_FAULT_NODATATYPE;
				MAIN_ShowError(PSTR("NO FUSE BYTES"));
			}
			else
			{
				PM_SendFuseLockBytes(TYPE_FUSE);
			}
		}

		if ((ProgOptions & PM_OPT_LOCK) && (ProgrammingFault == ISPCC_NO_FAULT))
		{
			if (ProgOptions & PM_OPT_FUSE)               // If fusebytes have already been written, we need to re-enter programming mode to latch them
			{
				MAIN_SetTargetResetLine(MAIN_RESET_INACTIVE);
				MAIN_Delay10MS(1);
				MAIN_SetTargetResetLine(MAIN_RESET_ACTIVE);
				ISPCC_EnterChipProgrammingMode();
			}

			if (InProgrammingMode)
			{
				ProgTypeBuffer[5] = 'L';
				LCD_PutStr(ProgTypeBuffer);
			
				StoredLocksFuses = eeprom_read_byte(&EEPROMVars.TotalLockBytes);
				if (!(StoredLocksFuses) || (StoredLocksFuses == 0xFF))
				{
					ProgrammingFault = ISPCC_FAULT_NODATATYPE;
					MAIN_ShowError(PSTR("NO LOCK BYTES"));
				}
				else
				{
					PM_SendFuseLockBytes(TYPE_LOCK);
				}
			}
			else
			{
				MAIN_ShowError(SyncErrorMessage);
			}
		}

		if (ProgrammingFault == ISPCC_FAULT_TIMEOUT)
		  MAIN_ShowError(PSTR("TIMEOUT"));

		if (eeprom_read_byte(&EEPROMVars.StartupMode) != 1) // Supress final PROG DONE/FAIL message if in production mode
		{
			if (ProgrammingFault != ISPCC_NO_FAULT)
			{
				TIMSK1 = (1 << OCIE1A);
				OCR1A  = TIMEOUT_HZ_TO_COMP(8, TIMEOUT_SRC_CPU, 1024); // Compare rate of 8Hz at 7372800Hz system clock, 1024 prescaler
				TCCR1B = ((1 << WGM12) | (1 << CS12) | (1 << CS10));   // Start timer at Fcpu/1024 speed in CTC mode, flash the red status LED

				LCD_PutStr_f(PSTR("PROG FAILED"));
				TG_PlayToneSeq(TONEGEN_SEQ_PROGFAIL);
			}
			else
			{
				MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);

				LCD_PutStr_f(PSTR("PROG DONE"));
				TG_PlayToneSeq(TONEGEN_SEQ_PROGDONE);		
			}
	
			LCD_WAIT_FOR_SCROLL_DONE();

			TCCR1B = 0;
			TIMSK1 = 0;
		}
	}
	else
	{
		MAIN_ShowError(SyncErrorMessage);
	}
	
	TOUT_SetupSleepTimer();
	MAIN_SetTargetResetLine(MAIN_RESET_INACTIVE);
	USI_SPIOff();
	DF_ENABLEDATAFLASH(FALSE);
	SPI_SPIOFF();
	MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);
}

/*
 NAME:      | PM_ChooseProgAVROpts
 PURPOSE:   | Allows the user to select and deselect different data types for programming into a target AVR from the dataflash
 ARGUMENTS: | None
 RETURNS:   | None
*/
void PM_ChooseProgAVROpts(void)
{
	char    Buffer[7];
	uint8_t ProgOptions     = eeprom_read_byte(&EEPROMVars.PGOptions);
	uint8_t SelectedOpt     = 0;
	uint8_t SelectedOptMask = 0x01;

	if (ProgOptions > 15)
	  ProgOptions = 0;

	MAIN_WaitForJoyRelease();

	JoyStatus = JOY_INVALID;                     // Use an invalid joystick value to force the program to write the
	                                             // name of the default command onto the LCD
	for (;;)
	{
		if (JoyStatus)
		{
			if (JoyStatus & JOY_LEFT)
			{
				eeprom_write_byte(&EEPROMVars.PGOptions, ProgOptions);
				return;
			}
			else if (JoyStatus & JOY_PRESS)
			{
				ProgOptions ^= SelectedOptMask;
			}
			else if (JoyStatus & JOY_UP)
			{
				(SelectedOpt == 0)? SelectedOpt = ARRAY_UPPERBOUND(ProgTypes) : SelectedOpt--;
			}
			else if (JoyStatus & JOY_DOWN)
			{
				(SelectedOpt == ARRAY_UPPERBOUND(ProgTypes))? SelectedOpt = 0 : SelectedOpt++;
			}
			
			SelectedOptMask = pgm_read_byte(&BitTable[SelectedOpt]);
		
			strcpy_P(Buffer, ProgTypes[SelectedOpt]);
			Buffer[4] = '>';
			Buffer[5] = ((ProgOptions & SelectedOptMask) ? 'Y' : 'N');
			Buffer[6] = '\0';

			LCD_PutStr(Buffer);

			MAIN_WaitForJoyRelease();
		}

		MAIN_MenuSleep();
	}
}

/*
 NAME:      | PM_SetProgramDataType
 PURPOSE:   | Toggles the selected flag for the specified datatype
 ARGUMENTS: | Mask of the datatype flag to enable (PM_OPT_FLASH, PM_OPT_EEPROM, PM_OPT_FUSE or PM_OPT_LOCK) or PM_OPT_CLEARFLAGS to clear
 RETURNS:   | None
*/
void PM_SetProgramDataType(uint8_t Mask)
{
	uint8_t ProgOptions = eeprom_read_byte(&EEPROMVars.PGOptions);

	if (ProgOptions > 15)
	  ProgOptions = 0;

	if (Mask == PM_OPT_CLEARFLAGS)
	  ProgOptions  = 0;
	else
	  ProgOptions |= Mask;
	  
	eeprom_write_byte(&EEPROMVars.PGOptions, ProgOptions);
}

/*
 NAME:      | PM_WaitWhileTargetBusy
 PURPOSE:   | Busy-waits (with timeout) while target's status is busy
 ARGUMENTS: | None
 RETURNS:   | None
*/
void PM_WaitWhileTargetBusy(void)
{
	TCNT1  = 0;
	TCCR1B = ((1 << CS12) | (1 << CS10));                           // Start timer 1 with a Fcpu/1024 clock

	do
	  USI_SPITransmitWord(0xF000);
	while ((USI_SPITransmitWord(0x0000) & 0x01) && (TCNT1 < ISPCC_COMM_TIMEOUT));

	if (TCNT1 >= ISPCC_COMM_TIMEOUT)
	  ProgrammingFault = ISPCC_FAULT_TIMEOUT;
	
	TCCR1B = 0;
}

/*
 NAME:      | PM_SendFuseLockBytes (static)
 PURPOSE:   | Programs either the stored fuse or stored lock bytes into the target AVR
 ARGUMENTS: | Type of bytes to program (TYPE_FUSE or TYPE_LOCK)
 RETURNS:   | None
*/
static void PM_SendFuseLockBytes(const uint8_t Type)
{
	uint8_t  TotalBytes;
	uint8_t* EEPROMAddress;

	if (Type == TYPE_FUSE)
	{
		TotalBytes    = eeprom_read_byte(&EEPROMVars.TotalFuseBytes);
		EEPROMAddress = &EEPROMVars.FuseBytes[0][0];
	}
	else
	{
		TotalBytes    = eeprom_read_byte(&EEPROMVars.TotalLockBytes);
		EEPROMAddress = &EEPROMVars.LockBytes[0][0];	
	}

	while (TotalBytes--)
	{
		for (uint8_t CommandByte = 0; CommandByte < 4; CommandByte++)
		{
			USI_SPITransmit(eeprom_read_byte(EEPROMAddress));
			EEPROMAddress++;
		}
		
		// Add some delay before programming next byte, if there is one:
		if (TotalBytes)
		  MAIN_Delay10MS(2);
	}
}

/*
 NAME:      | PM_SendEraseCommand (static)
 PURPOSE:   | Erases the target AVR via the stored erase command
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void PM_SendEraseCommand(void)
{
	for (uint8_t EraseByte = 2; EraseByte < 6 ; EraseByte++)            // Read out the erase chip command bytes
	  USI_SPITransmit(eeprom_read_byte(&EEPROMVars.EraseChip[EraseByte]));
			
	if (eeprom_read_byte(&EEPROMVars.EraseChip[1]))                     // Value of 1 indicates a busy flag test
	  PM_WaitWhileTargetBusy();
	else                                                                // Cleared flag means use a predefined delay
	  MAIN_Delay1MS(eeprom_read_byte(&EEPROMVars.EraseChip[0]));
}

/*
 NAME:      | PM_CreateProgrammingPackets (static)
 PURPOSE:   | Builds V2Protocol compatible programming packets from the stored flash or EEPROM data
 ARGUMENTS: | Type of stored data to program into target AVR (TYPE_FLASH or TYPE_EEPROM)
 RETURNS:   | None
*/
static void PM_CreateProgrammingPackets(void)
{			
	uint32_t BytesRead        = 0;
	uint32_t BytesToRead      = SM_GetStoredDataSize(MemoryType);
	uint16_t BytesPerProgram  = (((uint16_t)PacketBytes[1] << 8) | PacketBytes[2]);
	uint16_t PageLength       = eeprom_read_word((MemoryType == TYPE_FLASH)? &EEPROMVars.PageLength : &EEPROMVars.EPageLength);
	uint16_t BytesPerProgress = (BytesToRead / LCD_BARGRAPH_SIZE);
	uint8_t  ContinuedPage    = FALSE;
	uint8_t* EEPROMAddress;
	
	V2P_ClearCurrAddress();

	if (MemoryType == TYPE_FLASH)
	{
		EEPROMAddress = (uint8_t*)&EEPROMVars.WriteProgram;
		PacketBytes[0] = AICB_CMD_PROGRAM_FLASH_ISP;
	}
	else
	{
		EEPROMAddress = (uint8_t*)&EEPROMVars.WriteEEPROM;
		PacketBytes[0] = AICB_CMD_PROGRAM_EEPROM_ISP;
	}

	for (uint8_t HeaderByte = 0; HeaderByte < 10; HeaderByte++)         // Load in the write data command bytes
	{
		PacketBytes[HeaderByte] = eeprom_read_byte(EEPROMAddress);      // Synthesise a write packet header
		EEPROMAddress++;                                                // Increment the EEPROM location counter
	}

	while (BytesRead < BytesToRead)
	{
		if (PacketBytes[3] & ISPCC_PROG_MODE_PAGE)
		{
			if (PageLength > PM_LARGE_PAGE_LENGTH)                      // Max 160 bytes at a time
			{
				if (!(ContinuedPage))                                   // Start of a new page, program in the first 160 bytes
				{
					BytesPerProgram  = PM_LARGE_PAGE_LENGTH;
					PacketBytes[3]  &= ~ISPCC_PROG_MODE_PAGEDONE;
					ContinuedPage    = TRUE;
				}
				else                                                    // Middle of a page, program in the remainder
				{
					BytesPerProgram  = (PageLength - PM_LARGE_PAGE_LENGTH);
					PacketBytes[3]  |= ISPCC_PROG_MODE_PAGEDONE;
					ContinuedPage    = FALSE;
				}
				
				for (uint16_t LoadB = 0; LoadB < BytesPerProgram; LoadB++)
				  PacketBytes[10 + LoadB] = VAMM_ReadConsec();          // Load in the page				

				PacketBytes[1] = (uint8_t)(BytesPerProgram >> 8);
				PacketBytes[2] = (uint8_t)(BytesPerProgram);

				BytesRead += BytesPerProgram;                           // Increment the counter
			}
			else
			{
				for (uint16_t LoadB = 0; LoadB < PageLength; LoadB++)
				  PacketBytes[10 + LoadB] = VAMM_ReadConsec();          // Load in the page
			
				PacketBytes[1]  = (uint8_t)(PageLength >> 8);
				PacketBytes[2]  = (uint8_t)(PageLength);
				PacketBytes[3] |= ISPCC_PROG_MODE_PAGEDONE;

				BytesRead += PageLength;                                // Increment the counter
			}
		}
		else
		{
			if ((BytesRead + BytesPerProgram) > BytesToRead)            // Less than a whole BytesPerProgram left of data to write
			{
				BytesPerProgram = (BytesToRead - BytesRead);            // Next lot of bytes will be the remaining data length
				PacketBytes[1]  = (uint8_t)(BytesPerProgram >> 8);      // \. Save the new length
				PacketBytes[2]  = (uint8_t)(BytesPerProgram);           // /  into the data packet
			}

			for (uint16_t LoadB = 0; LoadB < BytesPerProgram; LoadB++)
			  PacketBytes[10 + LoadB] = VAMM_ReadConsec();              // Load in the page
		
			BytesRead += BytesPerProgram;                               // Increment the counter
		}
	
		if (!(BytesRead & 0x0000FFFF) && (BytesRead & 0x00FF0000))      // Extended address required
		{
			USI_SPITransmit(V2P_LOAD_EXTENDED_ADDR_CMD);                // Load extended address command
			USI_SPITransmit(0x00);
			USI_SPITransmit((BytesRead & 0x00FF0000) >> 16);            // The 3rd byte of the long holds the extended address
			USI_SPITransmit(0x00);
		}

		ISPCC_ProgramChip();                                            // Start the program cycle
		LCD_Bargraph(BytesRead / BytesPerProgress);                     // Show the progress onto the LCD

		if (ProgrammingFault)                                           // Error out early if there's a problem such as a timeout
		  break;
	}

	LCD_Bargraph(0);
	VAMM_Cleanup();
}
