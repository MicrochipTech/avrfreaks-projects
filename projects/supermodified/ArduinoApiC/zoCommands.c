//****************************************************************************************
//	File Name	: zoCommands.c
//	Description	: valid commands the supermodified servo
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

#include "zoCommands.h"
#include "zoMcu.h"
#include "zoString.h"
#include "zoProtocol.h"
#include "zoString.h"
#include "zoUart.h"
#include "zoI2c.h"

ZO_ERROR zoCommandsError = ZO_ERROR_DEFAULTS;
static ZO_PROTOCOL_HAL *Hal;

bool zoCommunicatePacket(ZO_PROTOCOL_PACKET *packet)
{
	bool success = TRUE;
	u08 err,i,cmdId;

	//store the command id of the command to send;
	cmdId=packet->commandID;

	//if there was a problem with communications: report it
	if(!zoProtocolCommand(Hal,packet))
	{
		success = FALSE;
		while(!zoErrorIsEmpty(Hal->error))
		{
			//add error offset here to distinguish communication errors
			//from errors received through an error response
			err = zoErrorGet(Hal->error);

			if(err > ZO_PROTOCOL_ERROR_NONE)
				zoErrorPut(&zoCommandsError,err + ZO_SMS_ERROR_PROTOCOL_ERRORS_OFFSET);
			else
			{
				if( (Hal->hw == ZO_PROTOCOL_HW_RS232) || 
					(Hal->hw == ZO_PROTOCOL_HW_UART_5V) ||
					(Hal->hw == ZO_PROTOCOL_HW_HALF_DUPLEX_RS485) )
					zoErrorPut(&zoCommandsError,err + ZO_SMS_ERROR_UART_ERRORS_OFFSET);
				else
					zoErrorPut(&zoCommandsError,err + ZO_SMS_ERROR_I2C_ERRORS_OFFSET);
			}
		}
	}
	else
	{
		//a success response should have the same command id
		if(packet->commandID == cmdId)
			success = TRUE;
		else
		{
			success = FALSE;
			//is it an error response
			if(packet->commandID == ZO_PROTOCOL_ERROR_ID)
			{
				for(i=0;i<packet->byteCount;i++)
					zoErrorPut(&zoCommandsError,packet->data[packet->byteCount]);
			}
			else
				zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_SMS_RESPONSE);
		}
	}

	return success;
}

void zoCommandsInit(ZO_PROTOCOL_HAL *hal)
{
	zoErrorInit(&zoCommandsError);
	
	enterCritical();
	Hal = hal;		//assign the Hardware abstraction layer
	exitCritical();
}



//command function prototypes_____________________________________________________________
/*00*/	bool zoCommandSetGainP(u08 nodeId, u16 gainP)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_GAIN_P;
	packet.byteCount = 2;
	u16ToStr(gainP,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*01*/	bool zoCommandSetGainI(u08 nodeId, u16 gainI)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_GAIN_I;
	packet.byteCount = 2;
	u16ToStr(gainI,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*02*/	bool zoCommandSetGainD(u08 nodeId, u16 gainD)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_GAIN_D;
	packet.byteCount = 2;
	u16ToStr(gainD,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*03*/	bool zoCommandSetProfileAcceleration(u08 nodeId, u16 ticksPerSec2)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_PROFILE_ACCELERATION;
	packet.byteCount = 2;
	u16ToStr(ticksPerSec2,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*04*/	bool zoCommandSetProfileVelocity(u08 nodeId, u16 ticksPerSec)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_PROFILE_VELOCITY;
	packet.byteCount = 2;
	u16ToStr(ticksPerSec,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*05*/	bool zoCommandSetCurrentLimit(u08 nodeId, u16 miliAmps)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_CURRENT_LIMIT;
	packet.byteCount = 2;
	u16ToStr(miliAmps,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*06*/	bool zoCommandSetCurrentLimitDuration(u08 nodeId, u16 miliSecs)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_CURRENT_LIMIT_DURATION;
	packet.byteCount = 2;
	u16ToStr(miliSecs,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*07*/	bool zoCommandVelocityMove(u08 nodeId, s16 ticksPerSec)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_VELOCITY_MOVE;
	packet.byteCount = 2;
	s16ToStr(ticksPerSec,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*08*/	bool zoCommandAbsolutePositionMove(u08 nodeId, s32 ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_ABSOLUTE_POSITION_MOVE;
	packet.byteCount = 4;
	s32ToStr(ticks,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*09*/	bool zoCommandRelativePositionMove(u08 nodeId, s32 ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_RELATIVE_POSITION_MOVE;
	packet.byteCount = 4;
	s32ToStr(ticks,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*0A*/  bool zoCommandProfiledVelocityMove(u08 nodeId, s16 ticksPerSec)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_PROFILED_VELOCITY_MOVE;
	packet.byteCount = 2;
	s16ToStr(ticksPerSec,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*0B*/  bool zoCommandProfiledAbsolutePositionMove(u08 nodeId, s32 ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_PROFILED_ABSOLUTE_POSITION_MOVE;
	packet.byteCount = 4;
	s32ToStr(ticks,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*0C*/	bool zoCommandProfiledRelativePositionMove(u08 nodeId, s32 ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_PROFILED_RELATIVE_POSITION_MOVE;
	packet.byteCount = 4;
	s32ToStr(ticks,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*0D*/	bool zoCommandSetVelocitySetpoint(u08 nodeId, s16 ticksPerSec)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_VELOCITY_SETPOINT;
	packet.byteCount = 2;
	s16ToStr(ticksPerSec,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*0E*/	bool zoCommandSetAbsolutePositionSetpoint(u08 nodeId, s32 ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_ABSOLUTE_POSITION_SETPOINT;
	packet.byteCount = 4;
	s32ToStr(ticks,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*0F*/	bool zoCommandSetRelativePositionSetpoint(u08 nodeId, s32 ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_RELATIVE_POSITION_SETPOINT;
	packet.byteCount = 4;
	s32ToStr(ticks,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*10*/	bool zoCommandSetProfiledVelocitySetpoint(u08 nodeId, s16 ticksPerSec)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_PROFILED_VELOCITY_SETPOINT;
	packet.byteCount = 2;
	s16ToStr(ticksPerSec,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*11*/	bool zoCommandSetProfiledAbsolutePositionSetpoint(u08 nodeId, s32 ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_PROFILED_ABSOLUTE_POSITION_SETPOINT;
	packet.byteCount = 4;
	s32ToStr(ticks,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*12*/	bool zoCommandSetProfiledRelativePositionSetpoint(u08 nodeId, s32 ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_PROFILED_RELATIVE_POSITION_SETPOINT;
	packet.byteCount = 4;
	s32ToStr(ticks,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*13*/	bool zoCommandConfigureDigitalIO(u08 nodeId, u08 portDirection)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_CONFIGURE_DIGITAL_IO;
	packet.byteCount = 1;
	packet.data[0] = portDirection;
	
	return (zoCommunicatePacket(&packet));
}

/*14*/	bool zoCommandSetDigitalOut(u08 nodeId, u08 portPins)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_DIGITAL_OUT;
	packet.byteCount = 1;
	packet.data[0] = portPins;
	
	return (zoCommunicatePacket(&packet));
}

/*15*/	bool zoCommandSetNodeID(u08 nodeId, u08 newNodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_NODE_ID;
	packet.byteCount = 1;
	packet.data[0] = newNodeId;
	
	return (zoCommunicatePacket(&packet));
}

/*16*/  bool zoCommandSetLocalAcceptanceMask(u08 nodeId, u08 lam)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_LOCAL_ACCEPTANCE_MASK;
	packet.byteCount = 1;
	packet.data[0] = lam;
	
	return (zoCommunicatePacket(&packet));
}

/*17*/  bool zoCommandSetBaudUart(u08 nodeId, u32 bps)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_BAUD_UART;
	packet.byteCount = 4;
	s32ToStr(bps,packet.data);
	
	return (zoCommunicatePacket(&packet));
}

/*18*/	bool zoCommandResetPosition(u08 nodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_RESET_POSITION;
	packet.byteCount = 0;
	
	return (zoCommunicatePacket(&packet));
}

/*19*/	bool zoCommandStart(u08 nodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_START;
	packet.byteCount = 0;
	
	return (zoCommunicatePacket(&packet));
}

/*1A*/	bool zoCommandHalt(u08 nodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_HALT;
	packet.byteCount = 0;
	
	return (zoCommunicatePacket(&packet));
}

/*1B*/	bool zoCommandStop(u08 nodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_STOP;
	packet.byteCount = 0;
	
	return (zoCommunicatePacket(&packet));
}

/*1C*/  bool zoCommandSetErrorReportingLevel(u08 nodeId, u08 errReportLvl)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_SET_ERROR_REPORTING_LEVEL;
	packet.byteCount = 1;
	packet.data[0] = errReportLvl;
	
	return (zoCommunicatePacket(&packet));
}


/*64*/	bool zoCommandGetGainP(u08 nodeId, u16 *gainP)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_GAIN_P;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*gainP = strToU16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*65*/	bool zoCommandGetGainI(u08 nodeId, u16 *gainI)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_GAIN_I;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*gainI = strToU16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*66*/	bool zoCommandGetGainD(u08 nodeId, u16 *gainD)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_GAIN_D;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*gainD = strToU16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*67*/	bool zoCommandGetProfileAcceleration(u08 nodeId, u16 *ticksPerSec2)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_PROFILE_ACCELERATION;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*ticksPerSec2 = strToU16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}
/*68*/	bool zoCommandGetProfileVelocity(u08 nodeId, u16 *ticksPerSec)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_PROFILE_VELOCITY;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*ticksPerSec = strToU16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*69*/	bool zoCommandGetCurrentLimit(u08 nodeId, u16 *miliAmps)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_CURRENT_LIMIT;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*miliAmps = strToU16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*6A*/	bool zoCommandGetCurrentLimitDuration(u08 nodeId, u16 *miliSecs)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_CURRENT_LIMIT_DURATION;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*miliSecs = strToU16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*6B*/	bool zoCommandGetDigitalIoConfiguration(u08 nodeId, u08 *portDir)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_DIO_CONFIGURATION;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*portDir = packet.data[0];
		return TRUE;
	}
	else
		return FALSE;
}

/*6C*/	bool zoCommandGetLocalAcceptanceMask(u08 nodeId, u08 *lam)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_LOCAL_ACCEPTANCE_MASK;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*lam = packet.data[0];
		return TRUE;
	}
	else
		return FALSE;
}

/*6D*/	bool zoCommandGetDigitalIn(u08 nodeId, u08 *pinState)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_DIGITAL_IN;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*pinState = packet.data[0];
		return TRUE;
	}
	else
		return FALSE;
}

/*6E*/  bool zoCommandGetAnalogIn(u08 nodeId, u16 *fourElementResultArray)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_ANALOG_IN;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		fourElementResultArray[0] = strToU16(packet.data);
		fourElementResultArray[1] = strToU16(&packet.data[2]);
		fourElementResultArray[2] =	strToU16(&packet.data[4]);
		fourElementResultArray[3] =	strToU16(&packet.data[6]);
		return TRUE;
	}
	else
		return FALSE;
}

/*6F*/  bool zoCommandGetPosition(u08 nodeId, s32 *ticks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_POSITION;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*ticks = strToS32(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*70*/	bool zoCommandGetAbsolutePosition(u08 nodeId, u16 *absTicks)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_ABSOLUTE_POSITION;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*absTicks = strToU16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*71*/	bool zoCommandGetVelocity(u08 nodeId, s16 *ticksPerSec)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_VELOCITY;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*ticksPerSec = strToS16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*72*/	bool zoCommandGetCurrent(u08 nodeId, s16 *miliAmp)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_CURRENT;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*miliAmp = strToS16(packet.data);
		return TRUE;
	}
	else
		return FALSE;
}

/*73*/	bool zoCommandGetWarning(u08 nodeId, u08 *warningBuffer,u08 *warningsCount)
{
	ZO_PROTOCOL_PACKET packet;
	u08 i;

	packet.AddressedNodeID = nodeId;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_GET_WARNING;
	packet.byteCount = 0;
	
	if(zoCommunicatePacket(&packet))
	{
		*warningsCount = packet.byteCount;
		for(i=0;i<packet.byteCount;i++)
			warningBuffer[i] = packet.data[i];
		return TRUE;
	}
	else
		return FALSE;
}


/*C8*/	bool zoCommandDoMove(u08 nodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = ZO_PROTOCOL_BROADCAST_ID;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_BRC_DO_MOVE;
	packet.byteCount = 0;
	
	return (zoCommunicatePacket(&packet));
}
/*C9*/	bool zoCommandGlobalStart(u08 nodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = ZO_PROTOCOL_BROADCAST_ID;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_BRC_START;
	packet.byteCount = 0;
	
	return (zoCommunicatePacket(&packet));
}

/*CA*/	bool zoCommandGlobalHalt(u08 nodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = ZO_PROTOCOL_BROADCAST_ID;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_BRC_HALT;
	packet.byteCount = 0;
	
	return (zoCommunicatePacket(&packet));
}

/*CB*/	bool zoCommandGlobalStop(u08 nodeId)
{
	ZO_PROTOCOL_PACKET packet;

	packet.AddressedNodeID = ZO_PROTOCOL_BROADCAST_ID;
	packet.OwnNodeID = Hal->localNodeID;
	packet.commandID = ZO_COMMAND_BRC_STOP;
	packet.byteCount = 0;
	
	return (zoCommunicatePacket(&packet));
}





