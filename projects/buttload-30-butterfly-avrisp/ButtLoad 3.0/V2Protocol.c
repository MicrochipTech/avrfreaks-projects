/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#define  INC_FROM_V2P
#include "V2Protocol.h"

// PROGMEM CONSTANTS:
const  uint8_t SignonResponse[]        PROGMEM = {AICB_CMD_SIGN_ON, AICB_STATUS_CMD_OK, 8, 'A', 'V', 'R', 'I', 'S', 'P', '_', '2', 0x00};

// GLOBAL VARIABLES:
       uint8_t  PacketBytes[V2P_MAXBUFFSIZE]   = {};
static uint8_t  SequenceNum                    = 0;
       uint16_t MessageSize                    = 0;

       uint8_t  InProgrammingMode              = FALSE;
       uint32_t CurrAddress                    = 0;

static uint8_t  Param_ControllerInit           = 0; // This is set to zero on reset, and can be written to or read by the computer

// ======================================================================================

/*
 NAME:      | V2P_RunStateMachine
 PURPOSE:   | Runs the master state machine for recieving and interpreting V2Protocol packets
 ARGUMENTS: | Pointer to secondary interpreter function for mode-specific commands
 RETURNS:   | None
*/
void V2P_RunStateMachine(const FuncPtr PacketDecodeFunction)
{
	uint8_t  V2PState           = V2P_STATE_IDLE;
	uint16_t CurrentMessageByte = 0;

	BUFF_InitializeBuffer();	
	TIMEOUT_SLEEP_TIMER_OFF();
	
	InProgrammingMode = FALSE;
	V2P_ClearCurrAddress();

	for (;;)
	{
		if (PacketTimeOut == TRUE)                 // Packet has timed out waiting for data
		  V2PState = V2P_STATE_TIMEOUT;
		else if (V2PState != V2P_STATE_IDLE)
		  TIMEOUT_PACKET_TIMER_ON();               // Reset the timer on each loop if not in idle mode
		
		switch (V2PState)
		{
			case V2P_STATE_IDLE:
				if (BuffElements)                  // Serial data recieved in FIFO buffer
				  V2PState = V2P_STATE_START;
				
				if ((JoyStatus & JOY_LEFT) && !(InProgrammingMode))
				{
					USART_OFF();
					TOUT_SetupSleepTimer();        // Re-setup and start the auto-sleep timer
					return;
				}
								
				SLEEPCPU(SLEEP_IDLE);              // Can idle here, since USART or Joystick interrupt will cause execution to continue

				break;
			case V2P_STATE_START:
				if (USART_Rx() == AICB_MESSAGE_START) // Start bit is always 0x1B
				  V2PState = V2P_STATE_GETSEQUENCENUM;
				else
				  V2PState = V2P_STATE_PACKERR;
				
				break;
			case V2P_STATE_GETSEQUENCENUM:
				SequenceNum = USART_Rx();
				V2PState    = V2P_STATE_GETMESSAGESIZE1;

				break;
			case V2P_STATE_GETMESSAGESIZE1:
				MessageSize = ((uint16_t)USART_Rx() << 8); // Message size is MSB first				
				V2PState    = V2P_STATE_GETMESSAGESIZE2;
				
				break;
			case V2P_STATE_GETMESSAGESIZE2:
				MessageSize |= USART_Rx();         // Get the second byte of the message size				

				CurrentMessageByte = 0;

				if (MessageSize < V2P_MAXBUFFSIZE) // Safety; only continue if packet size is less than buffer size
				  V2PState = V2P_STATE_GETTOKEN;
				else
				  V2PState = V2P_STATE_PACKERR;

				break;
			case V2P_STATE_GETTOKEN:
				if (USART_Rx() == AICB_TOKEN)      // Token bit is always 0x0E
				  V2PState = V2P_STATE_GETDATA;
				else                               // Incorrect token bit
				  V2PState = V2P_STATE_PACKERR;

				break;
			case V2P_STATE_GETDATA:
				if (CurrentMessageByte == MessageSize) // Packet reception complete
				  V2PState = V2P_STATE_GETCHECKSUM;
				else
				  PacketBytes[CurrentMessageByte++] = USART_Rx();

				break;
			case V2P_STATE_GETCHECKSUM:
				if (!(PacketTimeOut))             // Only try to process the packet if there is no timeout
				{
					if (V2P_GetChecksum() == USART_Rx()) // If checksum is ok, process the packet
					{
						V2P_ProcessPacketData(PacketDecodeFunction);
						V2PState = V2P_STATE_PACKOK;
					}
					else
					{					
						V2PState = V2P_STATE_BADCHKSUM;
					}
				}

				break;
			case V2P_STATE_BADCHKSUM:
			case V2P_STATE_PACKERR:
			case V2P_STATE_TIMEOUT:
				if (V2PState == V2P_STATE_BADCHKSUM)
				  PacketBytes[1] = AICB_STATUS_CKSUM_ERROR;
				else if (V2PState == V2P_STATE_PACKERR)
				  PacketBytes[1] = AICB_STATUS_CMD_FAILED;
				else
				  PacketBytes[1] = AICB_STATUS_CMD_TOUT;

				MessageSize = 2;
				V2P_SendPacket();

				V2PState = V2P_STATE_PACKOK;
				break;
			case V2P_STATE_PACKOK:
				PacketTimeOut = FALSE;
				BUFF_InitializeBuffer();
				TIMEOUT_PACKET_TIMER_OFF();

				V2PState = V2P_STATE_IDLE;
		}
	}	
}

/*
 NAME:      | V2P_SendPacket
 PURPOSE:   | Transmits the V2Protocol packet stored in PacketBytes global, complete with header and checksum
 ARGUMENTS: | None
 RETURNS:   | None
*/
void V2P_SendPacket(void)
{
	USART_Tx(AICB_MESSAGE_START);
	USART_Tx(SequenceNum);
	USART_Tx(MessageSize >> 8);
	USART_Tx(MessageSize & 0xFF);
	USART_Tx(AICB_TOKEN);

	for (uint16_t SentBytes = 0; SentBytes < MessageSize; SentBytes++)
	  USART_Tx(PacketBytes[SentBytes]);

	USART_Tx(V2P_GetChecksum());
}

/*
 NAME:      | V2P_CheckForExtendedAddress
 PURPOSE:   | Checks the current address to see if an extended address command needs to be sent, and if so sends it
 ARGUMENTS: | None
 RETURNS:   | None
*/
void V2P_CheckForExtendedAddress(void)
{
	if (BYTE(CurrAddress, 3))                 // MSb in the long indicates the need for sending a Load Extended Address command
	{
		USI_SPITransmit(V2P_LOAD_EXTENDED_ADDR_CMD);
		USI_SPITransmit(0x00);
		USI_SPITransmit(BYTE(CurrAddress, 2));
		USI_SPITransmit(0x00);
		
		BYTE(CurrAddress, 3) = 0;             // Clear the flag
	}
}

/*
 NAME:      | V2P_ClearCurrAddress
 PURPOSE:   | Clears the Current Address variable (saves flash by placing it into its own function)
 ARGUMENTS: | None
 RETURNS:   | None
*/
void V2P_ClearCurrAddress(void)
{
	CurrAddress = 0;
}

/*
 NAME:      | V2P_GetChecksum (static)
 PURPOSE:   | Calculates the V2Protocol checksum for the packet stored in the PacketBytes global
 ARGUMENTS: | None
 RETURNS:   | Checksum for the packet
*/
static uint8_t V2P_GetChecksum(void)
{
	uint8_t CheckSumByte;
	
	/* Checksum for the V2 protocol is comprised of an XOR of all the packet 
       bytes, including the start, sequence number, size and token bytes.    */
	
	CheckSumByte  = AICB_MESSAGE_START;
	CheckSumByte ^= SequenceNum;
	CheckSumByte ^= (uint8_t)(MessageSize >> 8);
	CheckSumByte ^= (uint8_t)(MessageSize & 0xFF);
	CheckSumByte ^= AICB_TOKEN;
	
	for (uint16_t CByteIndex = 0; CByteIndex < MessageSize; CByteIndex++)
	   CheckSumByte ^= PacketBytes[CByteIndex];

	return CheckSumByte;
}

/*
 NAME:      | V2P_ProcessPacketData (static)
 PURPOSE:   | Processes generic V2Protocol commands, and passes along mode-specific commands to the second decoding routine
 ARGUMENTS: | Pointer to the second (mode-specific) packet decoding routine
 RETURNS:   | None
*/
static void V2P_ProcessPacketData(const FuncPtr PacketDecodeFunction)
{
	switch (PacketBytes[0])                   // Look for generic commands which can be interpreted here, otherwise run the custom interpret routine
	{ 
		case AICB_CMD_SIGN_ON:
			MessageSize = 11;

			for (uint8_t SOByte = 0; SOByte < 11; SOByte++) // Load the sign-on sequence from program memory
			  PacketBytes[SOByte] = pgm_read_byte(&SignonResponse[SOByte]);
								  
			V2P_SendPacket();
			
			break;
		case AICB_CMD_FIRMWARE_UPGRADE:
			MessageSize = 2;
			
			PacketBytes[1] = AICB_STATUS_CMD_FAILED;  // Return failed (no automatic firmware upgrades)
			
			V2P_SendPacket();
			
			break;				
		case AICB_CMD_LOAD_ADDRESS:
			MessageSize = 2;

			/* The new address is stored in the packet most-significant byte first. Using standard bitshifts
			   to ensure portability wastes a LOT of code, so I've opted for the voodoo here. The CurrAddress
			   variable is cast to a byte array structure, allowing the loading of the bytes to be controlled
			   by a preprocessor define. This ensures optimal code is produced, saving time and flash space.  */
			
			for (uint8_t ByteNum = 0; ByteNum < 4; ByteNum++)
			  BYTE(CurrAddress, ByteNum) = PacketBytes[4 - ByteNum];

			if (PacketDecodeFunction == AICI_InterpretPacket)
			  V2P_CheckForExtendedAddress();
			else
			  VAMM_SetAddress();
								
			PacketBytes[1] = AICB_STATUS_CMD_OK;

			V2P_SendPacket();
			
			break;			
		case AICB_CMD_GET_PARAMETER:
		case AICB_CMD_SET_PARAMETER:						
			V2P_GetSetParameter();
			
			break;
		default:
			((FuncPtr)PacketDecodeFunction)(); // Run the interpret packet routine as set by the pointer
	}
}

/*
 NAME:      | V2P_GetSetParameter (static)
 PURPOSE:   | Manages get/set parameter V2Protocol packets
 ARGUMENTS: | None
 RETURNS:   | None
*/
static void V2P_GetSetParameter(void)
{
	uint8_t Param_Name = PacketBytes[1];      // Save the parameter number

	MessageSize = 3;                          // Set the default response message size to 3 bytes     
	PacketBytes[1] = AICB_STATUS_CMD_OK;      // Set the default response to OK

	switch (Param_Name)
	{
		case AICB_PARAM_BUILD_NUMBER_LOW:
			PacketBytes[2] = VERSION_MINOR;

			break;
		case AICB_PARAM_BUILD_NUMBER_HIGH:
			PacketBytes[2] = VERSION_MAJOR;

			break;
		case AICB_PARAM_HARDWARE_VERSION:
			PacketBytes[2] = V2P_HW_VERSION;

			break;
		case AICB_PARAM_SW_MAJOR:
			PacketBytes[2] = V2P_SW_VERSION_MAJOR;

			break;
		case AICB_PARAM_SW_MINOR:
			PacketBytes[2] = eeprom_read_byte(&EEPROMVars.FirmVerMinor);
			
			if (PacketBytes[2] == 0xFF)
			  PacketBytes[2] = V2P_SW_VERSION_MINOR_DEFAULT;

			break;
		case AICB_PARAM_SCK_DURATION:
			if (PacketBytes[0] == AICB_CMD_GET_PARAMETER)
			{
				PacketBytes[2] = pgm_read_byte(&USISpeedVals[eeprom_read_byte(&EEPROMVars.SCKDuration)]);
				if (PacketBytes[2] > USI_PRESET_SPEEDS)
				  PacketBytes[2] = 0;
			}
			else
			{
				MessageSize = 2;
				
				if (PacketBytes[2] >= USI_STUDIO_SPEEDS) // Bounds check
				  PacketBytes[2] = (USI_STUDIO_SPEEDS - 1);

				eeprom_write_byte(&EEPROMVars.SCKDuration, pgm_read_byte(&USISpeedIndex[PacketBytes[2]]));

				USI_SPIInitMaster();          // Re-Initialise the USI system with the new frequency
			}
					
			break;
		case AICB_PARAM_RESET_POLARITY:
			if (PacketBytes[0] == AICB_CMD_GET_PARAMETER)
			{
				PacketBytes[2] = eeprom_read_byte(&EEPROMVars.ResetPolarity);		
			}
			else
			{
				MessageSize = 2;
				eeprom_write_byte(&EEPROMVars.ResetPolarity, PacketBytes[2]);
				MAIN_SetTargetResetLine(MAIN_RESET_INACTIVE); // Change takes effect immediatly
			}
			
			break;
		case AICB_PARAM_CONTROLLER_INIT:
			if (PacketBytes[0] == AICB_CMD_GET_PARAMETER)
			{
				PacketBytes[2] = Param_ControllerInit;
			}
			else
			{
				MessageSize = 2;
				Param_ControllerInit = PacketBytes[2];
			}
			
			break;
		default:
			/* Despite not supporting other parameters (STK500 only), the AVR Studio programmer
			   seems to send them occasionally. A OK must be returned or the sequence will fail.
			   Also send back dummy values in response to any invalid parameter requests.        */
		
			if (PacketBytes[0] == AICB_CMD_GET_PARAMETER)
			  PacketBytes[2] = 0;             // If the command is a read, return a 0 for both parameters
			else
			  MessageSize = 2;                // Otherwise just send back an OK if the command is a set		
			
			break;
	}
	
	V2P_SendPacket();
}
