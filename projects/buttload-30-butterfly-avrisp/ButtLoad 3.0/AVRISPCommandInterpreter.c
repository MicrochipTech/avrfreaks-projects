/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#include "AVRISPCommandInterpreter.h"

const char AVRISPModeMessage[] PROGMEM = "*ATAVRISP MODE*";

// ======================================================================================

/*
 NAME:      | AICI_InterpretPacket
 PURPOSE:   | Manages the interpretation of V2Protocol packets for AVRISP mode operation
 ARGUMENTS: | None
 RETURNS:   | None
*/
void AICI_InterpretPacket(void)
{
	switch (PacketBytes[0])
	{
		case AICB_CMD_ENTER_PROGMODE_ISP:
			MessageSize = 2;
			
			USI_SPIInitMaster();
			ProgrammingFault = ISPCC_NO_FAULT;

			MAIN_SetTargetResetLine(MAIN_RESET_ACTIVE);
			ISPCC_EnterChipProgrammingMode();

			if (InProgrammingMode)
			{
				LCD_PutStr_f(AVRISPModeMessage);
				PacketBytes[1] = AICB_STATUS_CMD_OK;
			}
			else
			{
				LCD_PutStr_f(SyncErrorMessage);
				PacketBytes[1] = AICB_STATUS_CMD_FAILED;
			}

			break;
		case AICB_CMD_LEAVE_PROGMODE_ISP:
			MessageSize = 2;

			if (InProgrammingMode)
			  TG_PlayToneSeq((ProgrammingFault == ISPCC_NO_FAULT)? TONEGEN_SEQ_PROGDONE : TONEGEN_SEQ_PROGFAIL);

			MAIN_Delay1MS(PacketBytes[1]);             // Wait for the "PreDelay" amount specified in the packet
			InProgrammingMode = FALSE;
			MAIN_SetTargetResetLine(MAIN_RESET_INACTIVE); // Release the RESET line and allow the slave AVR to run
			MAIN_Delay1MS(PacketBytes[2]);             // Wait for the "PostDelay" amount specified in the packet
			
			USI_SPIOff();

			MAIN_SETSTATUSLED(MAIN_STATLED_GREEN);     // Non programming mode = green status led
			PacketBytes[1] = AICB_STATUS_CMD_OK;

			break;
		case AICB_CMD_CHIP_ERASE_ISP:
			MessageSize = 2;
			
			for (uint8_t PacketB = 3; PacketB <= 6; PacketB++) // Send the erase commands to the slave AVR
			  USI_SPITransmit(PacketBytes[PacketB]);

			if (PacketBytes[2])                        // Poll mode, value of 1 indicates a busy flag wait
			{
				ProgrammingFault = ISPCC_NO_FAULT;
				PM_WaitWhileTargetBusy();

				PacketBytes[1] = ((ProgrammingFault == ISPCC_NO_FAULT)? AICB_STATUS_CMD_OK : AICB_STATUS_CMD_TOUT);
			}
			else                                       // Poll mode flag of 0 indicates a predefined delay
			{
				MAIN_Delay1MS(PacketBytes[1]);         // Wait the specified interval to ensure erase complete
				PacketBytes[1] = AICB_STATUS_CMD_OK;
			}
						
			break;
		case AICB_CMD_SPI_MULTI:
			MessageSize = (3 + PacketBytes[2]);        // Number of recieved bytes, plus two OKs and the command byte
	
			uint8_t TxBytes      = PacketBytes[1];     // \. The packet data is overwritten during the transfer. Because
			uint8_t RxBytes      = PacketBytes[2];     // |  of this each data byte must be stored into temp variables
			uint8_t RxStartByte  = PacketBytes[3];     // /  so that their values are not lost.
			uint8_t RxByteNum    = 0;
			uint8_t TxByteNum    = 0;
			uint8_t RecievedByte = 0;

			while (TxByteNum < TxBytes)              // Still bytes to transfer
			{
				RecievedByte = USI_SPITransmit(PacketBytes[4 + TxByteNum]); // Transmit the byte, store the answer

				if ((TxByteNum >= RxStartByte) && (RxByteNum < RxBytes))
				  PacketBytes[2 + RxByteNum++] = RecievedByte;
				  
				TxByteNum++;
			}

			while (RxByteNum < RxBytes)                              // Still more bytes to recieve
			  PacketBytes[2 + RxByteNum++] = USI_SPITransmit(0x00);  // Send dummy bytes to fetch the response(s)

			PacketBytes[1]             = AICB_STATUS_CMD_OK;
			PacketBytes[2 + RxByteNum] = AICB_STATUS_CMD_OK;

			break;
		case AICB_CMD_READ_SIGNATURE_ISP:
		case AICB_CMD_READ_FUSE_ISP:
		case AICB_CMD_READ_LOCK_ISP:
		case AICB_CMD_READ_OSCCAL_ISP:
			MessageSize = 4;
	
			for (uint8_t ByteNum = 1; ByteNum <= 4; ByteNum++)
			{
				uint8_t Response = USI_SPITransmit(PacketBytes[1 + ByteNum]); // Transmit the four signature request bytes

				if (ByteNum == PacketBytes[1])         // If the current byte is the requested signature byte, save the reponse in the packet
				  PacketBytes[2] = Response;
			}

			PacketBytes[1] = AICB_STATUS_CMD_OK;       // Data byte is encased in CMD_OKs
			PacketBytes[3] = AICB_STATUS_CMD_OK;

			break;
		case AICB_CMD_PROGRAM_FUSE_ISP:
		case AICB_CMD_PROGRAM_LOCK_ISP:
			MessageSize = 3;
			
			for (uint8_t PacketB = 1; PacketB <= 4; PacketB++) // Send the lock-byte values to the slave AVR
			  USI_SPITransmit(PacketBytes[PacketB]);

			PacketBytes[1] = AICB_STATUS_CMD_OK;       // Two CMD_OKs are always returned
			PacketBytes[2] = AICB_STATUS_CMD_OK;

			break;
		case AICB_CMD_READ_FLASH_ISP:
		case AICB_CMD_READ_EEPROM_ISP:
			MessageSize = 0;                           // Here to prevent compiler from complaining if a var dec appears straight after a case

			uint8_t  ReadCommand = PacketBytes[3];
			uint16_t BytesToRead = (((uint16_t)PacketBytes[1] << 8) | PacketBytes[2]);
			uint8_t  MemoryType  = PacketBytes[0];

			MessageSize = BytesToRead + 3;

			for (uint16_t ReadByte = 0; ReadByte < BytesToRead; ReadByte++)
			{
				if (PacketBytes[0] == AICB_CMD_READ_FLASH_ISP)  // Flash read mode - word addresses so MSB/LSB masking nessesary
				  USI_SPITransmit(ReadCommand | ((ReadByte & 0x01)? ISPCC_HIGH_BYTE_READ : ISPCC_LOW_BYTE_READ));
				else                                   // EEPROM read mode, address is in bytes and so no masking nessesary
				  USI_SPITransmit(ReadCommand);
				
				USI_SPITransmitWord(CurrAddress & 0xFFFF);

				PacketBytes[2 + ReadByte] = USI_SPITransmit(0x00); // Read in the byte stored at the requested location

				if ((ReadByte & 0x01) || (MemoryType == AICB_CMD_READ_EEPROM_ISP)) // Flash addresses are given in words; only increment on the odd byte if reading the flash.
				  V2P_IncrementCurrAddress();
			}
			
			PacketBytes[1]               = AICB_STATUS_CMD_OK; // Return data should be encompassed in STATUS_CMD_OKs
			PacketBytes[2 + BytesToRead] = AICB_STATUS_CMD_OK;

			break;
		case AICB_CMD_PROGRAM_FLASH_ISP:
		case AICB_CMD_PROGRAM_EEPROM_ISP:
			MessageSize = 2;

			ISPCC_ProgramChip();	

			PacketBytes[1] = ((ProgrammingFault == ISPCC_NO_FAULT) ? AICB_STATUS_CMD_OK : AICB_STATUS_CMD_TOUT);
			
			break;
		case AICB_CMD_OSCCAL:
			MessageSize = 2;

			MAIN_SetTargetResetLine(MAIN_RESET_INACTIVE);
			
			USI_SPIOff();                              // Disable USI while calibration sequence runs
			PacketBytes[1] = (AICI_SendCalibrationClocks() ? AICB_STATUS_CMD_OK : AICB_STATUS_CMD_FAILED);
			USI_SPIInitMaster();                       // Re-enable USI subsystem
			
			if (InProgrammingMode)                     // Was previously in programming mode, re-enter in preparation of future commands
			{
				MAIN_SetTargetResetLine(MAIN_RESET_ACTIVE);
				ISPCC_EnterChipProgrammingMode();
			}

			break;
		default:                                       // Unknown command, return error
			MessageSize = 2;
			
			PacketBytes[1] = AICB_STATUS_CMD_UNKNOWN;
	}

	V2P_SendPacket();                                  // Send the response packet
}
