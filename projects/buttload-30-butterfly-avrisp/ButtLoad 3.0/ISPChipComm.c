/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#define  INC_FROM_ISPCC
#include "ISPChipComm.h"

const char SyncErrorMessage[] PROGMEM = "SYNC ERROR";

// ======================================================================================

/*
 NAME:      | ISPCC_EnterChipProgrammingMode
 PURPOSE:   | Attempts to sync with a target AVR, and enters programming mode in the target
 ARGUMENTS: | None
 RETURNS:   | None
*/
void ISPCC_EnterChipProgrammingMode(void)
{
	uint8_t ByteDelay = PacketBytes[5];
	uint8_t Attempts  = PacketBytes[4];
	uint8_t Response;

	MAIN_SETSTATUSLED(MAIN_STATLED_ORANGE);               // Orange = Busy

	MAIN_Delay1MS(PacketBytes[2]);                        // Wait before continuing, amount specified in the packet

	if ((!(Attempts)) || (Attempts > 100))
	   Attempts = 24;
		
	while (Attempts--)
	{
		USI_SPITransmit(PacketBytes[8]);
		MAIN_Delay1MS(ByteDelay);
		USI_SPITransmit(PacketBytes[9]);
		MAIN_Delay1MS(ByteDelay);
			
		Response = USI_SPITransmit(PacketBytes[10]);
		MAIN_Delay1MS(ByteDelay);

		if (PacketBytes[7] == ISPCC_POLL_MODE_AVR)
		  USI_SPITransmit(PacketBytes[11]);
		else
		  Response = USI_SPITransmit(PacketBytes[11]);

		if (!(PacketBytes[7]) || (Response == PacketBytes[6])) // Polling disabled, or returned value matches expected poll value
		{
			MAIN_Delay1MS(ByteDelay);

			InProgrammingMode = TRUE;
			MAIN_SETSTATUSLED(MAIN_STATLED_RED);
			return;
		}
		
		MAIN_Delay1MS(ByteDelay);
		USI_SPIToggleClock();                             // Out of sync, shift in one bit and try again
	}

	// If function hasn't returned by now, all the attempts have failed. Show this by
	// resetting the status leds to green (ready) and send a fail message.

	MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);
	InProgrammingMode = FALSE;
	TG_PlayToneSeq(TONEGEN_SEQ_SYNCFAIL);
}

/*
 NAME:      | ISPCC_ProgramChip
 PURPOSE:   | Programs a target with the data located in PacketBytes (must be well formed V2Protocol programming packet)
 ARGUMENTS: | None
 RETURNS:   | None
*/
void ISPCC_ProgramChip(void)
{
	uint16_t PollAddress   = 0;
	uint8_t  ProgMode      = PacketBytes[3];
	uint8_t  WriteCommandH = PacketBytes[5] | ISPCC_HIGH_BYTE_WRITE;
	uint8_t  WriteCommandL = PacketBytes[5] | ISPCC_LOW_BYTE_WRITE;
	uint8_t  WriteCommandE = PacketBytes[5];
	uint16_t StartAddress  = (uint16_t)CurrAddress;
	uint16_t BytesToWrite  = (((uint16_t)PacketBytes[1] << 8) | PacketBytes[2]);
	uint8_t  CmdMemType    = PacketBytes[0];  
	uint8_t  PollType;
	uint8_t  ByteToWrite;

	if (ProgMode & ISPCC_PROG_MODE_PAGE)                  // Page writing mode
	{
		for (uint16_t WriteByte = 0; ((WriteByte < BytesToWrite) && !(ProgrammingFault)); WriteByte++) // Transmit the page bytes
		{
			ByteToWrite = PacketBytes[10 + WriteByte];
		
			if (CmdMemType == AICB_CMD_PROGRAM_FLASH_ISP) // Flash write mode - word addresses so MSB/LSB masking 
			  USI_SPITransmit((WriteByte & 0x01)? WriteCommandH : WriteCommandL);
			else                                          // EEPROM write mode - byte addresses so no masking 
			  USI_SPITransmit(WriteCommandE);

			USI_SPITransmitWord(CurrAddress & 0xFFFF);    // Only the LSW of the address should be sent
			USI_SPITransmit(ByteToWrite);                 // Send one of the new bytes to be written

			if (!(PollAddress))
			{
				if ((PacketBytes[8] != ByteToWrite)       // Can do polling
				   && ((CmdMemType == AICB_CMD_PROGRAM_FLASH_ISP) || ((CmdMemType == AICB_CMD_PROGRAM_EEPROM_ISP) && (PacketBytes[9] != ByteToWrite))))
				{
					PollAddress = (CurrAddress & 0xFFFF); // Save the current address for later polling
				
					if (CmdMemType == AICB_CMD_PROGRAM_FLASH_ISP)
					  PollAddress = ((PollAddress << 1) + (WriteByte & 0x01));
				}
			}

			// Flash addresses are in words; only increment address on odd byte, OR if it's the EEPROM being programmed (byte addresses)
			if ((WriteByte & 0x01) || (CmdMemType == AICB_CMD_PROGRAM_EEPROM_ISP))
			  V2P_IncrementCurrAddress();
		}

		PollType = ProgMode;

		if (ProgMode & ISPCC_PROG_MODE_PAGEDONE)          // If this packet is the end of a page, we need to send the program page command
		{
			USI_SPITransmit(PacketBytes[6]);              // Send the write program memory page command
			USI_SPITransmitWord(StartAddress);            // Send the page address word
			USI_SPITransmit(0x00);

			if (!(PollAddress))                           // No polling address
			  PollType = ((ProgMode & ~ISPCC_PAGE_POLLTYPE_MASK) | ISPCC_PAGE_POLLTYPE_WAIT);

			ISPCC_PollForProgComplete(PollType, PollAddress);
		}
	}
	else                                                  // Flash Word writing mode or EEPROM byte writing mode
	{
		for (uint16_t WriteByte = 0; ((WriteByte < BytesToWrite) && !(ProgrammingFault)); WriteByte++)
		{
			ByteToWrite = PacketBytes[10 + WriteByte];

			if (CmdMemType == AICB_CMD_PROGRAM_FLASH_ISP)
			  USI_SPITransmit((WriteByte & 0x01)? WriteCommandH : WriteCommandL);
			else
			  USI_SPITransmit(WriteCommandE);
					
			USI_SPITransmitWord(CurrAddress & 0xFFFF);    // Transmit the current address to the slave AVR
			USI_SPITransmit(ByteToWrite);                 // Send one of the new bytes to be written

			PollType = ProgMode;

			if ((PacketBytes[8] != ByteToWrite)           // Can do polling
			   && ((CmdMemType == AICB_CMD_PROGRAM_FLASH_ISP) || ((CmdMemType == AICB_CMD_PROGRAM_EEPROM_ISP) && (PacketBytes[9] != ByteToWrite))))
			{
				PollAddress = (CurrAddress & 0xFFFF);     // Save the current address;

				if (CmdMemType == AICB_CMD_PROGRAM_FLASH_ISP)
				  PollAddress = ((PollAddress << 1) + (WriteByte & 0x01));
			}
			else
			{
				PollType = ((ProgMode & ~ISPCC_WORD_POLLTYPE_MASK) | ISPCC_WORD_POLLTYPE_WAIT);
			}					

			// Flash addresses are in words; only increment address on the odd byte, OR if it's the EEPROM being programmed (byte addresses)
			if ((WriteByte & 0x01) || (CmdMemType == AICB_CMD_PROGRAM_EEPROM_ISP))
			  V2P_IncrementCurrAddress();

			ISPCC_PollForProgComplete(PollType, PollAddress);
		}
	}
}

/*
 NAME:      | ISPCC_PollForProgComplete (static)
 PURPOSE:   | Polls the target AVR with the requested polling method to wait until the target is ready for more data
 ARGUMENTS: | Polling type in the form of a V2Protocol programming packet poll byte, polling address
 RETURNS:   | None
*/
static void ISPCC_PollForProgComplete(const uint8_t PollData, uint16_t PollAddr)
{
	uint8_t PollType;
	uint8_t ProgCommand;
	
	if (PollData & ISPCC_PROG_MODE_PAGE)
	  PollType = (PollData >> ISPCC_PAGE_POLLTYPE_MASKSHIFT);
	else
	  PollType = (PollData >> ISPCC_WORD_POLLTYPE_MASKSHIFT);

	switch (PollType & ISPCC_POLLTYPE_MASK)
	{
		case ISPCC_POLLTYPE_DATA:
			ProgCommand = PacketBytes[7];
			
			if (PacketBytes[0] == AICB_CMD_PROGRAM_FLASH_ISP) // Flash uses word addresses
			{
				ProgCommand  |= ((PollAddr & 0x01)? ISPCC_HIGH_BYTE_READ : ISPCC_LOW_BYTE_READ);
				PollAddr    >>= 1;
			}

			TCNT1  = 0; 
			TCCR1B = ((1 << CS12) | (1 << CS10));         // Start timer 1 with a Fcpu/1024 clock

			do
			{
				USI_SPITransmit(ProgCommand);
				USI_SPITransmitWord(PollAddr);
			}
			while ((USI_SPITransmit(0x00) == PacketBytes[8]) && (TCNT1 < ISPCC_COMM_TIMEOUT));

			TCCR1B = 0;
			
			break;
		case ISPCC_POLLTYPE_READY:
			PM_WaitWhileTargetBusy();
				
			break;
		default:                                          // Default is Wait polling
			MAIN_Delay1MS(PacketBytes[4]);	
	}
}
