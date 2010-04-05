#ifndef ZO_PROTOCOL_I2C
#define	ZO_PROTOCOL_I2C

//****************************************************************************************
//	File Name	: zoProtocolI2C.h
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

#include "zoTypes.h"
#include "zoProtocol.h"
#include "zoError.h"

//error codes beyond already existing I2C error codes
#define ZO_PROTOCOL_I2C_ERROR_NONE					0x10
#define ZO_PROTOCOL_I2C_ERROR_RX_PACKET_OVERWRITTEN	0x11
#define ZO_PROTOCOL_I2C_ERROR_INVALID_RX_BYTECOUNT	0x12

extern ZO_ERROR *zoProtocolI2cError;

void zoProtocolI2cInit(u08 ownNodeID,u16 kBps);
bool zoProtocolI2cPutPacket(const ZO_PROTOCOL_PACKET* packet);
bool zoProtocolI2cGetPacket(ZO_PROTOCOL_PACKET* packet);
void zoProtocolI2cSetLAM(u08 localAcceptanceMask);
bool zoProtocolI2cSetBitrate(u32 bitsPerSecond);

#endif // ZO_PROTOCOL_I2C