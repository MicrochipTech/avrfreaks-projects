//****************************************************************************************
//	File Name	: zoProtocol.c
//	Description	: proprietary Zero One Mechatronics protocol
//	Created		: 25.1.2010
//	Target MCU	: Atmel AVR
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************************************

#include "zoProtocol.h"
#include "zoProtocolI2c.h"
#include "zoProtocolUart.h"
#include "zoSystemTimer.h"

static inline bool checkLRC(ZO_PROTOCOL_PACKET* packet)
{
	return ( ( packet->lrc == 
		      zoProtocolLRC((u08*)&(packet->commandID),packet->byteCount + 2)) ? 
			  TRUE:FALSE );
}


//initialization
bool zoProtocolInit(ZO_PROTOCOL_HW_TYPE hw,ZO_PROTOCOL_HAL *hal, u08 localNodeID)
{
	bool success = TRUE;

	switch (hw)
	{
	case ZO_PROTOCOL_HW_I2C:
		hal->putPacket = zoProtocolI2cPutPacket;
		hal->getPacket = zoProtocolI2cGetPacket;
		hal->setLAM = zoProtocolI2cSetLAM;
		hal->setBitRate = zoProtocolI2cSetBitrate;
		hal->error = zoProtocolI2cError;
		zoProtocolI2cInit(localNodeID,100);
		break;

	case ZO_PROTOCOL_HW_RS232:
	case ZO_PROTOCOL_HW_UART_5V:
	case ZO_PROTOCOL_HW_HALF_DUPLEX_RS485:
		hal->putPacket = zoProtocolUartPutPacket;
		hal->getPacket = zoProtocolUartGetPacket;
		hal->setLAM = zoProtocolUartSetLAM;
		hal->setBitRate = zoProtocolUartSetBitrate;
		hal->error = zoProtocolUartError;
		if( ! zoProtocolUartInit(hal->hw,localNodeID,9600) )
			success = FALSE;
		break;
	
	case ZO_PROTOCOL_HW_CAN:
		//TODO (for processors that support it)
		break;
	}

	hal->hw = hw;
	hal->localNodeID = localNodeID;
	hal->setLAM(ZO_PROTOCOL_DEFAULT_LAM);

	return success;
}

//master operations
bool zoProtocolCommand(ZO_PROTOCOL_HAL* hal, ZO_PROTOCOL_PACKET* packet)
{
	bool success = TRUE;
	u16 t;
	
	//compute lrc
	packet->lrc = zoProtocolLRC((u08*)&(packet->commandID),packet->byteCount + 2);

	//is it a broadcast command? Then send it and do not wait on response
	if( packet->commandID >= ZO_PROTOCOL_BRC_COMMANDS_START )
		return (hal->putPacket(packet));

	//if it is a set or get command: Send it and wait with timeout on response
	if( hal->putPacket(packet) )
	{
		zoSystemTimerTimeOutInit(&t);
		while( !(hal->getPacket(packet)) )
		{
			if( zoSystemTimerTimeOutExpired(&t,ZO_PROTOCOL_COMMAND_RESPONSE_TIMEOUT_MS) )
			{
				zoErrorPut(hal->error,ZO_PROTOCOL_ERROR_RESPONSE_TIMEOUT);
				success = FALSE;
				break;
			}
		}

		if( success )
		{
			if( ! checkLRC(packet)  )
			{
				zoErrorPut(hal->error,ZO_PROTOCOL_ERROR_WRONG_LRC);
				success = FALSE;
			}
		}
	}
	else
		success = FALSE;

	return success;
}

//slave operations
inline bool zoProtocolParse(ZO_PROTOCOL_HAL *hal, ZO_PROTOCOL_PACKET *packet )
{
	if( hal->getPacket(packet) )
	{
		if ( checkLRC(packet) )
			return TRUE;
		else
			zoErrorPut(hal->error,ZO_PROTOCOL_ERROR_WRONG_LRC);
	}

	return FALSE;
}


bool zoProtocolCommandResponse(ZO_PROTOCOL_HAL *hal,
							   ZO_PROTOCOL_PACKET* packet,
							   ZO_ERROR *error)
{
	//were we addressed directly? ie not with broadcast or through local acceptance
	//mechanism? no -> do not respond at all
	if( packet->AddressedNodeID  !=  hal->localNodeID )
		return TRUE;

	//respond to the node that issued the command
	packet->AddressedNodeID =  packet->OwnNodeID;
	packet->OwnNodeID = hal->localNodeID;

	//if it was a set command set bytecount to zero
	if ( packet->commandID < ZO_PROTOCOL_GET_COMMANDS_START )
		packet->byteCount = 0;

	if(error != 0) //if the error struct pointer exists
	{
		if( !zoErrorIsEmpty(error) )		//are there any errors in the system?
		{
			//yes there are, prepare the error response
			packet->commandID = ZO_PROTOCOL_ERROR_ID;
			packet->byteCount = 0;
			do {
				packet->data[packet->byteCount] = zoErrorGet(error);
				packet->byteCount++;
			}while(!zoErrorIsEmpty(error));
		}
	}

	//compute lrc
	packet->lrc = zoProtocolLRC((u08*)&(packet->commandID),packet->byteCount + 2);

	return (hal->putPacket(packet));
}

u08 zoProtocolLRC(const u08* crcBytes, u08 crcByteCount)
{
	u08 i;
	u08 lrc = 0;

	for( i=0; i<crcByteCount; i++)
		lrc ^= crcBytes[i];

   return lrc;
}
