#ifndef ZO_PROTOCOL_H
#define ZO_PROTOCOL_H

//****************************************************************************************
//	File Name	: zoProtocol.h
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

#include "zoTypes.h"
#include "zoError.h"

#define ZO_PROTOCOL_SET_COMMANDS_START	0x00
#define ZO_PROTOCOL_GET_COMMANDS_START	0x64
#define ZO_PROTOCOL_BRC_COMMANDS_START	0xC8
#define ZO_PROTOCOL_ERROR_ID			0xFA

#define ZO_PROTOCOL_HEADER_0	0x55
#define ZO_PROTOCOL_HEADER_1	0xAA

#define ZO_PROTOCOL_BROADCAST_ID 0x00

#define ZO_PROTOCOL_DATA_SIZE	0x10
#define ZO_PROTOCOL_INFO_SIZE	0x05
#define ZO_PROTOCOL_PACKET_SIZE (ZO_PROTOCOL_DATA_SIZE + ZO_PROTOCOL_INFO_SIZE)

#define ZO_PROTOCOL_COMMAND_RESPONSE_TIMEOUT_MS	200

#define ZO_PROTOCOL_DEFAULT_LAM	0xFF

//error codes
#define ZO_PROTOCOL_ERROR_NONE				0x20
#define ZO_PROTOCOL_ERROR_RESPONSE_TIMEOUT	0x21
#define ZO_PROTOCOL_ERROR_WRONG_LRC			0x22

typedef enum {
	ZO_PROTOCOL_HW_I2C,
	ZO_PROTOCOL_HW_RS232,
	ZO_PROTOCOL_HW_HALF_DUPLEX_RS485,
	ZO_PROTOCOL_HW_UART_5V,
	ZO_PROTOCOL_HW_CAN,
}ZO_PROTOCOL_HW_TYPE;

typedef struct {
	u08 AddressedNodeID;
	u08 OwnNodeID;
	u08 commandID;
	u08 byteCount;
	u08 data[ZO_PROTOCOL_DATA_SIZE];
	u08 lrc;
}ZO_PROTOCOL_PACKET;

//hardware abstraction layer
typedef struct {
	u08 localNodeID;
	ZO_PROTOCOL_HW_TYPE hw;
	ZO_ERROR *error;
	bool ( *putPacket )(const ZO_PROTOCOL_PACKET* packet);
	bool ( *getPacket )(ZO_PROTOCOL_PACKET* packet);
	void ( *setLAM )(u08 localAcceptanceMask);
	bool ( *setBitRate )(u32 bitsPerSecond); 
}ZO_PROTOCOL_HAL;

//initialization
bool zoProtocolInit(ZO_PROTOCOL_HW_TYPE hw,ZO_PROTOCOL_HAL *hal, u08 localNodeID);

//master operations
bool zoProtocolCommand(ZO_PROTOCOL_HAL *hal, ZO_PROTOCOL_PACKET *packet);

//slave operations
bool zoProtocolParse(ZO_PROTOCOL_HAL *hal, ZO_PROTOCOL_PACKET *packet );
bool zoProtocolCommandResponse(ZO_PROTOCOL_HAL *hal,
							   ZO_PROTOCOL_PACKET* packet,
							   ZO_ERROR *error);

u08 zoProtocolLRC(const u08* crcBytes, u08 crcByteCount);

inline void zoProtocolSetLAM(ZO_PROTOCOL_HAL *hal,u08 lam)
{
	hal->setLAM(lam);
}

inline void zoProtocolSetBitrate(ZO_PROTOCOL_HAL *hal,u32 bitrate)
{
	hal->setBitRate(bitrate);
}

#endif //ZO_PROTOCOL_H