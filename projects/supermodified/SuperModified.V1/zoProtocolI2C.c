//****************************************************************************************
//	File Name	: zoProtocolI2C.c
//	Description	: proprietary Zero One Mechatronics protocol, specifics for I2C 
//				  implementation
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

#include <avr/io.h>
#include "zoProtocolI2C.h"
#include "zoMcu.h"
#include "zoI2c.h"

static void i2cSlaveRx(u08 rxLength,u08* rxData);
static volatile ZO_PROTOCOL_PACKET RxPacket;
static volatile bool zoRxPacketReceived = FALSE;

//use the i2c error buffer for error handling
ZO_ERROR *zoProtocolI2cError = &zoI2cError;

//ZO_ERROR zoProtocolI2cError = ZO_ERROR_DEFAULTS;

void zoProtocolI2cInit(u08 ownNodeID, u16 kBps)
{
	//initialize with default baud rate of 100KHz
	zoI2cInit();
	zoI2cSetLocalDeviceAddr(ownNodeID,1);
	zoI2cSetBitrate(kBps);
	//assign slave receive handler
	zoI2cSetSlaveReceiveHandler(i2cSlaveRx);
}

bool zoProtocolI2cPutPacket(const ZO_PROTOCOL_PACKET* packet)
{
	u08 str[ZO_PROTOCOL_PACKET_SIZE];
	u08 i;

	str[0] = packet->AddressedNodeID;
	str[1] = packet->OwnNodeID;
	str[2] = packet->commandID;
	str[3] = packet->byteCount;
	
	for( i=0; i< packet->byteCount; i++ )
		str[4+i] = packet->data[i];

	str[(packet->byteCount + 4)] = packet->lrc;
	
	return ( zoI2cMasterSendBlocking(packet->AddressedNodeID,
									  (packet->byteCount + 5),
									  str));
}

bool zoProtocolI2cGetPacket(ZO_PROTOCOL_PACKET* packet)
{
	u08 i;

	if( !zoErrorIsEmpty(zoProtocolI2cError) )
		return FALSE;

	if( zoRxPacketReceived )
	{
		enterCritical();
		//copy buffered packet
		packet->AddressedNodeID = RxPacket.AddressedNodeID;		
		packet->OwnNodeID = RxPacket.OwnNodeID;
		packet->commandID = RxPacket.commandID;
		packet->byteCount = RxPacket.byteCount;
		packet->lrc = RxPacket.lrc;
		
		for(i=0;i<packet->byteCount;i++)
			packet->data[i] = RxPacket.data[i];
		
		//indicate received packet was used
		zoRxPacketReceived = FALSE;
		exitCritical();
		return TRUE;
	}
	else
		return FALSE;
}

void i2cSlaveRx(u08 rxLength,u08* rxData)
{
	u08 i;
	
	if(zoRxPacketReceived == TRUE)
		zoErrorPutIsr(zoProtocolI2cError,ZO_PROTOCOL_I2C_ERROR_RX_PACKET_OVERWRITTEN);

	RxPacket.AddressedNodeID = *rxData++;
	RxPacket.OwnNodeID = *rxData++;
	RxPacket.commandID = *rxData++;
	RxPacket.byteCount = *rxData++;
	
	if( (RxPacket.byteCount + ZO_PROTOCOL_INFO_SIZE) != rxLength)
	{
		zoErrorPutIsr(zoProtocolI2cError,ZO_PROTOCOL_I2C_ERROR_INVALID_RX_BYTECOUNT);
		zoRxPacketReceived = TRUE;
		return;
	}

	for( i=0; i < RxPacket.byteCount ; i++ )
		RxPacket.data[i] = *rxData++;

	RxPacket.lrc = *rxData;

	//indicate we received a packet
	zoRxPacketReceived = TRUE;
}

inline void zoProtocolI2cSetLAM(u08 localAcceptanceMask)
{
	TWAMR = (~localAcceptanceMask)<<1;
}

inline bool zoProtocolI2cSetBitrate(u32 bitsPerSecond)
{
	return (zoI2cSetBitrate(bitsPerSecond/1000));
}

