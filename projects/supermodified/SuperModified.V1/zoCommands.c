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
#include <avr/eeprom.h>
#include <math.h>
#include "zoMcu.h"
#include "zoString.h"
#include "zoSms.h"
#include "zoSmsError.h"
#include "zoEncoder.h"
#include "zoMotor.h"
#include "zoAdc.h"

#define	ZO_COMMANDS_SET_COMMANDS_COUNT \
	( ZO_COMMANDS_SET_COMMANDS_END - ZO_PROTOCOL_SET_COMMANDS_START )
#define ZO_COMMANDS_GET_COMMANDS_COUNT \
	( ZO_COMMANDS_GET_COMMANDS_END - ZO_PROTOCOL_GET_COMMANDS_START )
#define ZO_COMMANDS_BRC_COMMANDS_COUNT \
	( ZO_COMMANDS_BRC_COMMANDS_END - ZO_PROTOCOL_BRC_COMMANDS_START )

static ZO_COMMAND_FUNCTOR zoCommandSet[ZO_COMMANDS_SET_COMMANDS_COUNT];
static u08 zoCommandsSetByteCount[ZO_COMMANDS_SET_COMMANDS_COUNT];

static ZO_COMMAND_FUNCTOR zoCommandGet[ZO_COMMANDS_GET_COMMANDS_COUNT]; 

static ZO_COMMAND_FUNCTOR zoCommandBrc[ZO_COMMANDS_BRC_COMMANDS_COUNT]; 

ZO_ERROR zoCommandsError = ZO_ERROR_DEFAULTS;


//utility functions_______________________________________________________________________
void zoCommandsInit(void)
{
		zoErrorInit(&zoCommandsError);
	
/*00*/	zoCommandSet[ZO_COMMAND_SET_GAIN_P] = zoCommandSetGainP;
		zoCommandsSetByteCount[ZO_COMMAND_SET_GAIN_P] = 2;
/*01*/	zoCommandSet[ZO_COMMAND_SET_GAIN_I] = zoCommandSetGainI;
		zoCommandsSetByteCount[ZO_COMMAND_SET_GAIN_I] = 2;
/*02*/	zoCommandSet[ZO_COMMAND_SET_GAIN_D] = zoCommandSetGainD;
		zoCommandsSetByteCount[ZO_COMMAND_SET_GAIN_D] = 2;		
/*03*/	zoCommandSet[ZO_COMMAND_SET_PROFILE_ACCELERATION] =
		zoCommandSetProfileAcceleration;
		zoCommandsSetByteCount[ZO_COMMAND_SET_PROFILE_ACCELERATION] = 2;		
/*04*/	zoCommandSet[ZO_COMMAND_SET_PROFILE_VELOCITY] =	zoCommandSetProfileVelocity;
		zoCommandsSetByteCount[ZO_COMMAND_SET_PROFILE_VELOCITY] = 2;		
/*05*/	zoCommandSet[ZO_COMMAND_SET_CURRENT_LIMIT] = zoCommandSetCurrentLimit;
		zoCommandsSetByteCount[ZO_COMMAND_SET_CURRENT_LIMIT] = 2;		
/*06*/	zoCommandSet[ZO_COMMAND_SET_CURRENT_LIMIT_DURATION] =
		zoCommandSetCurrentLimitDuration;
		zoCommandsSetByteCount[ZO_COMMAND_SET_CURRENT_LIMIT_DURATION] = 2;				
/*07*/	zoCommandSet[ZO_COMMAND_VELOCITY_MOVE] = zoCommandVelocityMove;
		zoCommandsSetByteCount[ZO_COMMAND_VELOCITY_MOVE] = 2;						
/*08*/	zoCommandSet[ZO_COMMAND_ABSOLUTE_POSITION_MOVE] = zoCommandAbsolutePositionMove;
		zoCommandsSetByteCount[ZO_COMMAND_ABSOLUTE_POSITION_MOVE] = 4;						
/*09*/	zoCommandSet[ZO_COMMAND_RELATIVE_POSITION_MOVE] = zoCommandRelativePositionMove;
		zoCommandsSetByteCount[ZO_COMMAND_RELATIVE_POSITION_MOVE] = 4;						
/*0A*/  zoCommandSet[ZO_COMMAND_PROFILED_VELOCITY_MOVE] = zoCommandProfiledVelocityMove;
		zoCommandsSetByteCount[ZO_COMMAND_PROFILED_VELOCITY_MOVE] = 2;
/*0B*/  zoCommandSet[ZO_COMMAND_PROFILED_ABSOLUTE_POSITION_MOVE] =
		zoCommandProfiledAbsolutePositionMove;
		zoCommandsSetByteCount[ZO_COMMAND_PROFILED_ABSOLUTE_POSITION_MOVE] = 4;
/*0C*/	zoCommandSet[ZO_COMMAND_PROFILED_RELATIVE_POSITION_MOVE] =
		zoCommandProfiledRelativePositionMove;
		zoCommandsSetByteCount[ZO_COMMAND_PROFILED_RELATIVE_POSITION_MOVE] = 4;
/*0D*/	zoCommandSet[ZO_COMMAND_SET_VELOCITY_SETPOINT] = zoCommandSetVelocitySetpoint;
		zoCommandsSetByteCount[ZO_COMMAND_SET_VELOCITY_SETPOINT] = 2;
/*0E*/	zoCommandSet[ZO_COMMAND_SET_ABSOLUTE_POSITION_SETPOINT] =
		zoCommandSetAbsolutePositionSetpoint;
		zoCommandsSetByteCount[ZO_COMMAND_SET_ABSOLUTE_POSITION_SETPOINT] = 4;
/*0F*/	zoCommandSet[ZO_COMMAND_SET_RELATIVE_POSITION_SETPOINT] =
		zoCommandSetRelativePositionSetpoint;
		zoCommandsSetByteCount[ZO_COMMAND_SET_RELATIVE_POSITION_SETPOINT] = 4;
/*10*/	zoCommandSet[ZO_COMMAND_SET_PROFILED_VELOCITY_SETPOINT] =
		zoCommandSetProfiledVelocitySetpoint;
		zoCommandsSetByteCount[ZO_COMMAND_SET_PROFILED_VELOCITY_SETPOINT] = 2;
/*11*/	zoCommandSet[ZO_COMMAND_SET_PROFILED_ABSOLUTE_POSITION_SETPOINT] =
		zoCommandSetProfiledAbsolutePositionSetpoint;
		zoCommandsSetByteCount[ZO_COMMAND_SET_PROFILED_ABSOLUTE_POSITION_SETPOINT] = 4;
/*12*/	zoCommandSet[ZO_COMMAND_SET_PROFILED_RELATIVE_POSITION_SETPOINT] =	
		zoCommandSetProfiledRelativePositionSetpoint;
		zoCommandsSetByteCount[ZO_COMMAND_SET_PROFILED_RELATIVE_POSITION_SETPOINT] = 4;
/*13*/	zoCommandSet[ZO_COMMAND_CONFIGURE_DIGITAL_IO] =	zoCommandConfigureDigitalIO;
		zoCommandsSetByteCount[ZO_COMMAND_CONFIGURE_DIGITAL_IO] = 1;
/*14*/	zoCommandSet[ZO_COMMAND_SET_DIGITAL_OUT] = zoCommandSetDigitalOut;
		zoCommandsSetByteCount[ZO_COMMAND_SET_DIGITAL_OUT] = 1;
/*15*/	zoCommandSet[ZO_COMMAND_SET_NODE_ID] = zoCommandSetNodeID;
		zoCommandsSetByteCount[ZO_COMMAND_SET_NODE_ID] = 1;
/*16*/  zoCommandSet[ZO_COMMAND_SET_LOCAL_ACCEPTANCE_MASK] = 
		zoCommandSetLocalAcceptanceMask;
		zoCommandsSetByteCount[ZO_COMMAND_SET_LOCAL_ACCEPTANCE_MASK] = 1;
/*17*/  zoCommandSet[ZO_COMMAND_SET_BAUD_UART] = zoCommandSetBaudUart;
		zoCommandsSetByteCount[ZO_COMMAND_SET_BAUD_UART] = 4;
/*18*/	zoCommandSet[ZO_COMMAND_RESET_POSITION] = zoCommandResetPosition;
		zoCommandsSetByteCount[ZO_COMMAND_RESET_POSITION] = 0;		
/*19*/	zoCommandSet[ZO_COMMAND_START] = zoCommandStart;
		zoCommandsSetByteCount[ZO_COMMAND_START] = 0;		
/*1A*/	zoCommandSet[ZO_COMMAND_HALT] =	zoCommandHalt;
		zoCommandsSetByteCount[ZO_COMMAND_HALT] = 0;
/*1B*/	zoCommandSet[ZO_COMMAND_STOP] =	zoCommandStop;
		zoCommandsSetByteCount[ZO_COMMAND_STOP] = 0;
/*1C*/  zoCommandSet[ZO_COMMAND_SET_ERROR_REPORTING_LEVEL] =
		zoCommandSetErrorReportingLevel;
		zoCommandsSetByteCount[ZO_COMMAND_SET_ERROR_REPORTING_LEVEL] = 1;

/*64*/	zoCommandGet[ZO_COMMAND_GET_GAIN_P-ZO_PROTOCOL_GET_COMMANDS_START] =	
		zoCommandGetGainP;
/*65*/	zoCommandGet[ZO_COMMAND_GET_GAIN_I-ZO_PROTOCOL_GET_COMMANDS_START] =
		zoCommandGetGainI;
/*66*/	zoCommandGet[ZO_COMMAND_GET_GAIN_D-ZO_PROTOCOL_GET_COMMANDS_START] =	
		zoCommandGetGainD;
/*67*/	zoCommandGet[ZO_COMMAND_GET_PROFILE_ACCELERATION-ZO_PROTOCOL_GET_COMMANDS_START] =
		zoCommandGetProfileAcceleration;
/*68*/	zoCommandGet[ZO_COMMAND_GET_PROFILE_VELOCITY-ZO_PROTOCOL_GET_COMMANDS_START] =	
		zoCommandGetProfileVelocity;
/*69*/	zoCommandGet[ZO_COMMAND_GET_CURRENT_LIMIT-ZO_PROTOCOL_GET_COMMANDS_START] =
		zoCommandGetCurrentLimit;
/*6A*/	zoCommandGet[ZO_COMMAND_GET_CURRENT_LIMIT_DURATION-ZO_PROTOCOL_GET_COMMANDS_START]= 
		zoCommandGetCurrentLimitDuration;
/*6B*/	zoCommandGet[ZO_COMMAND_GET_DIO_CONFIGURATION-ZO_PROTOCOL_GET_COMMANDS_START] =	
		zoCommandGetDigitalIoConfiguration;
/*6C*/	zoCommandGet[ZO_COMMAND_GET_LOCAL_ACCEPTANCE_MASK-ZO_PROTOCOL_GET_COMMANDS_START]= 
		zoCommandGetLocalAcceptanceMask;
/*6D*/	zoCommandGet[ZO_COMMAND_GET_DIGITAL_IN-ZO_PROTOCOL_GET_COMMANDS_START] =	
		zoCommandGetDigitalIn;
/*6E*/  zoCommandGet[ZO_COMMAND_GET_ANALOG_IN-ZO_PROTOCOL_GET_COMMANDS_START] =
		zoCommandGetAnalogIn;
/*6F*/  zoCommandGet[ZO_COMMAND_GET_POSITION-ZO_PROTOCOL_GET_COMMANDS_START] =	
		zoCommandGetPosition;
/*70*/	zoCommandGet[ZO_COMMAND_GET_ABSOLUTE_POSITION-ZO_PROTOCOL_GET_COMMANDS_START]=
		zoCommandGetAbsolutePosition;
/*71*/	zoCommandGet[ZO_COMMAND_GET_VELOCITY-ZO_PROTOCOL_GET_COMMANDS_START]=	
		zoCommandGetVelocity;
/*72*/	zoCommandGet[ZO_COMMAND_GET_CURRENT-ZO_PROTOCOL_GET_COMMANDS_START]=	
		zoCommandGetCurrent;
/*73*/	zoCommandGet[ZO_COMMAND_GET_WARNING-ZO_PROTOCOL_GET_COMMANDS_START]=
		zoCommandGetWarning;

/*C8*/	zoCommandBrc[ZO_COMMAND_DO_MOVE-ZO_PROTOCOL_BRC_COMMANDS_START]
		= zoCommandDoMove;
/*C9*/	zoCommandBrc[ZO_COMMAND_GLOBAL_START-ZO_PROTOCOL_BRC_COMMANDS_START]
		= zoCommandStart; 
/*CA*/	zoCommandBrc[ZO_COMMAND_GLOBAL_HALT-ZO_PROTOCOL_BRC_COMMANDS_START]
		= zoCommandHalt;
/*CB*/	zoCommandBrc[ZO_COMMAND_GLOBAL_STOP-ZO_PROTOCOL_BRC_COMMANDS_START]
		= zoCommandStop;
}

bool zoCommandsServiceCommand(ZO_PROTOCOL_PACKET *packet)
{
	if( packet->commandID >= ZO_COMMANDS_BRC_COMMANDS_END )
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_COMMAND_ID);
		return FALSE;
	}
	
	if( packet->commandID >= ZO_PROTOCOL_BRC_COMMANDS_START )
	{
		//all broadcast commands of the supermodified have bytecount = zero
		if(packet->byteCount != 0)
			zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_BYTECOUNT);

		//check if a command that needs the motor initialized is being called and the 
		//motor is not initialized
		if( !zoMotorGetState() && ( (packet->commandID == ZO_COMMAND_DO_MOVE) || 
			(packet->commandID == ZO_COMMAND_GLOBAL_HALT) ) )
		{
			zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_COMMAND);
		}

		//above are warnings so try to execute the command
		zoCommandBrc[packet->commandID - ZO_PROTOCOL_BRC_COMMANDS_START](packet);

		if(!zoErrorIsEmpty(&zoCommandsError))
			return FALSE;

		return (zoErrorIsEmpty(&zoCommandsError));
	}
	
	if( packet->commandID >= ZO_COMMANDS_GET_COMMANDS_END)
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_COMMAND_ID);
		return FALSE;
	}

	if( packet->commandID >= ZO_PROTOCOL_GET_COMMANDS_START )
	{
		//execute the command even with wrong bytecount. Then issue the warning
		zoCommandGet[packet->commandID - ZO_PROTOCOL_GET_COMMANDS_START](packet);

		//all get commands should have a bytecount of zero
		if(packet->byteCount != 0)
		{
			zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_BYTECOUNT);
			return FALSE;
		}

		return (zoErrorIsEmpty(&zoCommandsError));
	}

	if( packet->commandID >= ZO_COMMANDS_SET_COMMANDS_END)
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_COMMAND_ID);
		return FALSE;
	}

	if( packet->commandID >= ZO_PROTOCOL_SET_COMMANDS_START )
	{
		//check received bytecount. In this case it is a serious error because if motor 
		//receives wrong data we might damage something. So if a wrong bytecount is 
		//received do not execute the command.
		if(packet->byteCount != zoCommandsSetByteCount[packet->commandID])
		{
			zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_SET_BYTECOUNT);
			return FALSE;
		}
		
		//check if a command that needs the motor initialized is being called and the 
		//motor is not initialized
		if( !zoMotorGetState() && ( ((packet->commandID >= ZO_COMMAND_VELOCITY_MOVE) && 
			(packet->commandID <= ZO_COMMAND_PROFILED_RELATIVE_POSITION_MOVE)) || 
			(packet->commandID == ZO_COMMAND_HALT) ) )
		{
			zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_COMMAND);
			return FALSE;
		}		
		
		zoCommandSet[packet->commandID](packet);
		return (zoErrorIsEmpty(&zoCommandsError));
	}
}

//command function definitions____________________________________________________________
/*00*/	void zoCommandSetGainP(ZO_PROTOCOL_PACKET *packet)
{
	u16 gainP;

	gainP = strToU16(packet->data);
	eeprom_write_word((u16*)ZO_EEPROM_ADDRESS_GAIN_P, gainP);
	
	enterCritical();
	Sms.Pid.GainP = gainP;
	exitCritical();	
}

/*01*/	void zoCommandSetGainI(ZO_PROTOCOL_PACKET *packet)
{
	u16 gainI;

	gainI = strToU16(packet->data);
	eeprom_write_word((u16*)ZO_EEPROM_ADDRESS_GAIN_I, gainI);
	
	enterCritical();
	Sms.Pid.GainI = gainI;
	exitCritical();
}

/*02*/	void zoCommandSetGainD(ZO_PROTOCOL_PACKET *packet)
{
	u16 gainD;

	gainD = strToU16(packet->data);
	eeprom_write_word((u16*)ZO_EEPROM_ADDRESS_GAIN_D, gainD);
	
	enterCritical();
	Sms.Pid.GainD = gainD;
	exitCritical();
}

/*03*/	void zoCommandSetProfileAcceleration(ZO_PROTOCOL_PACKET *packet)
{
	u16 pAccel;

	pAccel = strToU16(packet->data);

	//Pointless to allow a profile acceleration of 0.
	if(pAccel == 0)
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_ARGUMENT);
		return;
	}

	eeprom_write_word((u16*)ZO_EEPROM_ADDRESS_PROFILE_ACCELERATION, pAccel);
	
	enterCritical();
	Sms.Profile.DesiredAcceleration = pAccel;
	exitCritical();
}

/*04*/	void zoCommandSetProfileVelocity(ZO_PROTOCOL_PACKET *packet)
{
	u16 pVel;

	pVel = strToU16(packet->data);

	//Pointless to allow a profile velocity of zero
	if(pVel == 0)
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_ARGUMENT);
		return;
	}

	eeprom_write_word((u16*)ZO_EEPROM_ADDRESS_PROFILE_VELOCITY, pVel);
	
	enterCritical();
	Sms.Profile.DesiredVelocity = pVel;
	exitCritical();
}

/*05*/	void zoCommandSetCurrentLimit(ZO_PROTOCOL_PACKET *packet)
{
	u16 currLim;

	currLim = strToU16(packet->data);

	//Pointless to allow a current limit of zero
	if(currLim == 0)
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_ARGUMENT);
		return;
	}

	eeprom_write_word((u16*)ZO_EEPROM_ADDRESS_CURRENT_LIMIT, currLim);
	
	enterCritical();
	Sms.Settings.CurrentLimit = currLim;
	exitCritical();
}

/*06*/	void zoCommandSetCurrentLimitDuration(ZO_PROTOCOL_PACKET *packet)
{
	u16 currLimDurMiliSecs;

	currLimDurMiliSecs = strToU16(packet->data);
	eeprom_write_word((u16*)ZO_EEPROM_ADDRESS_CURRENT_LIMIT_DURATION, currLimDurMiliSecs);
	
	enterCritical();
	Sms.Settings.CurrentLimit = currLimDurMiliSecs;
	exitCritical();
}

/*07*/	void zoCommandVelocityMove(ZO_PROTOCOL_PACKET *packet)
{
	s16 vel;

	vel = strToS16(packet->data);

	enterCritical();
	if(Sms.Control.State == ZO_SMS_CONTROL_POSITION_CONTROL)
		Sms.Profile.PositionSetpoint = (double)zoEncoderGetPos();
	Sms.Profile.VelocityDeltaPos = (double)vel/Sms.Control.LoopHz;
	Sms.Control.State = ZO_SMS_CONTROL_VELOCITY_CONTROL;
	exitCritical();
}

/*08*/	void zoCommandAbsolutePositionMove(ZO_PROTOCOL_PACKET *packet)
{
	s32 pos;
	pos = strToS32(packet->data);

	enterCritical();
	Sms.Control.State = ZO_SMS_CONTROL_POSITION_CONTROL;
	Sms.Pid.SetPoint = pos;
	exitCritical();
}

/*09*/	void zoCommandRelativePositionMove(ZO_PROTOCOL_PACKET *packet)
{
	s32 pos;

	pos = strToS32(packet->data);

	enterCritical();
	Sms.Control.State = ZO_SMS_CONTROL_POSITION_CONTROL;
	Sms.Pid.SetPoint += pos;
	exitCritical();
}

/*0A*/  void zoCommandProfiledVelocityMove(ZO_PROTOCOL_PACKET *packet)
{
	s16 vel;

	vel = strToS16(packet->data);
	zoProfiledVelocity(vel);
}

/*0B*/  void zoCommandProfiledAbsolutePositionMove(ZO_PROTOCOL_PACKET *packet)
{
	s32 pos;

	pos = strToS32(packet->data);
	zoProfiledMoveAbsolute(pos);
}

/*0C*/	void zoCommandProfiledRelativePositionMove(ZO_PROTOCOL_PACKET *packet)
{
	s32 pos;
	
	pos = strToS32(packet->data) + zoEncoderGetPos();
	zoProfiledMoveAbsolute(pos);
}

/*0D*/	void zoCommandSetVelocitySetpoint(ZO_PROTOCOL_PACKET *packet)
{
	s16 vel;

	vel = strToS16(packet->data);

	enterCritical();
	Sms.BufferdSetPoint.type = ZO_SMS_SETPOINT_VELOCITY;
	Sms.BufferdSetPoint.value = (s32)vel;
	exitCritical();
}

/*0E*/	void zoCommandSetAbsolutePositionSetpoint(ZO_PROTOCOL_PACKET *packet)
{
	s32 pos;

	pos = strToS32(packet->data);

	enterCritical();
	Sms.BufferdSetPoint.type = ZO_SMS_SETPOINT_ABSOLUTE_POSITION;
	Sms.BufferdSetPoint.value = pos;
	exitCritical();
}

/*0F*/	void zoCommandSetRelativePositionSetpoint(ZO_PROTOCOL_PACKET *packet)
{
	s32 pos;

	pos = strToS32(packet->data);

	enterCritical();
	Sms.BufferdSetPoint.type = ZO_SMS_SETPOINT_RELATIVE_POSITION;
	Sms.BufferdSetPoint.value = pos;
	exitCritical();
}

/*10*/	void zoCommandSetProfiledVelocitySetpoint(ZO_PROTOCOL_PACKET *packet)
{
	s16 vel;

	vel = strToS16(packet->data);

	enterCritical();
	Sms.BufferdSetPoint.type = ZO_SMS_SETPOINT_PROFILED_VELOCITY;
	Sms.BufferdSetPoint.value = (s32)vel;
	exitCritical();
}
/*11*/	void zoCommandSetProfiledAbsolutePositionSetpoint(ZO_PROTOCOL_PACKET *packet)
{
	s32 pos;

	pos = strToS32(packet->data);

	enterCritical();
	Sms.BufferdSetPoint.type = ZO_SMS_SETPOINT_PROFILED_ABSOLUTE_POSITION;
	Sms.BufferdSetPoint.value = pos;
	exitCritical();
}

/*12*/	void zoCommandSetProfiledRelativePositionSetpoint(ZO_PROTOCOL_PACKET *packet)
{
	s32 pos;

	pos = strToS32(packet->data);

	enterCritical();
	Sms.BufferdSetPoint.type = ZO_SMS_SETPOINT_PROFILED_RELATIVE_POSITION;
	Sms.BufferdSetPoint.value = pos;
	exitCritical();
}

/*13*/	void zoCommandConfigureDigitalIO(ZO_PROTOCOL_PACKET *packet)
{
	u08 dir;

	dir = packet->data[0];
	
	eeprom_write_byte((u08*)ZO_EEPROM_ADDRESS_DIGITAL_IO_CONFIG, dir & 0x0F);

	enterCritical();
	//set digital IO 1 direction -> portD7
	DDRD = (DDRD & (~0x80)) | ( (dir<<7) & 0x80 );
	//set digital IO 2,3,4 direction -> portB3,4,5 respectively
	DDRB = (DDRB & (~0x38)) | ( (dir<<2) & 0x38 );
	
	Sms.Settings.DigitalIoConfig = dir & 0x0F;
	exitCritical();
}

/*14*/	void zoCommandSetDigitalOut(ZO_PROTOCOL_PACKET *packet)
{
	u08 out;

	enterCritical();
	//if a pin is input leave respective bit zero
	out = packet->data[0] & Sms.Settings.DigitalIoConfig;
	//set digital out 1:if it is input leave it as it is (pull up enabled)
	PORTD = (PORTD & (~0x80)) | ( (out<<7) & 0x80 );
	//set digital IO 2,3,4 direction -> portB3,4,5 respectively
	PORTB = (PORTB & (~0x38)) | ( (out<<3) & 0x38 );
	exitCritical();
}

/*15*/	void zoCommandSetNodeID(ZO_PROTOCOL_PACKET *packet)
{
	//node id of 0x00 is forbidden by the protocol
	if( (packet->data[0] == 0x00) || (packet->data[0] > 0x7F) )
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_ARGUMENT);
		return;
	}
	
	//only write to eeprom. A restart is required for setting to take effect
	eeprom_write_byte((u08*)ZO_EEPROM_ADDRESS_NODE_ID, packet->data[0]);
}
/*16*/  void zoCommandSetLocalAcceptanceMask(ZO_PROTOCOL_PACKET *packet)
{
	if( (packet->data[0] == 0x00) )
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_ARGUMENT);
		return;
	}

	eeprom_write_byte((u08*)ZO_EEPROM_ADDRESS_LAM, packet->data[0]);
}
/*17*/  void zoCommandSetBaudUart(ZO_PROTOCOL_PACKET *packet)
{
	u32 baud;

	baud = strToS32(packet->data);

	if( ( baud > 115200 ) || (baud == 0) )
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_ARGUMENT);
		return;
	}
	
	eeprom_write_dword((u32*)ZO_EEPROM_ADDRESS_BAUD_UART, baud);
}
/*18*/	void zoCommandResetPosition(ZO_PROTOCOL_PACKET *packet)
{
	zoEncoderReset();
}
/*19*/	void zoCommandStart(ZO_PROTOCOL_PACKET *packet)
{
	enterCritical();
	Sms.BufferdSetPoint.type = ZO_SMS_SETPOINT_NONE;
	zoEncoderReset();
	Sms.Pid.PrevError = 0;
	Sms.Pid.Integral = 0;
	Sms.Control.State = ZO_SMS_CONTROL_POSITION_CONTROL;
	Sms.Pid.SetPoint = 0;
	zoMotorSetState(TRUE);
	exitCritical();
}
/*1A*/	void zoCommandHalt(ZO_PROTOCOL_PACKET *packet)
{
	enterCritical();
	Sms.Control.State = ZO_SMS_CONTROL_POSITION_CONTROL;
	Sms.Pid.SetPoint = zoEncoderGetPos();
	exitCritical();
}
/*1B*/	void zoCommandStop(ZO_PROTOCOL_PACKET *packet)
{
	enterCritical();
	Sms.Control.State = ZO_SMS_CONTROL_NO_CONTROL;
	exitCritical();
	zoMotorSetState(FALSE);
}
/*1C*/  void zoCommandSetErrorReportingLevel(ZO_PROTOCOL_PACKET *packet)
{
	//only error reporting levels 0 and 1 exist.
	if(packet->data[0]>1)
	{
		zoErrorPut(&zoCommandsError,ZO_COMMANDS_ERROR_INVALID_ARGUMENT);
		return;
	}
	
	eeprom_write_byte((u08*)ZO_EEPROM_ADDRESS_ERROR_REPORTING_LVL, packet->data[0]);
	
	enterCritical();
	Sms.Settings.errorReportingLevel = packet->data[0];
	exitCritical();	
}

/*64*/	void zoCommandGetGainP(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;
	
	enterCritical();
	u16ToStr(Sms.Pid.GainP,packet->data);
	exitCritical();
}

/*65*/	void zoCommandGetGainI(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;

	enterCritical();
	u16ToStr(Sms.Pid.GainI,packet->data);
	exitCritical();
}
/*66*/	void zoCommandGetGainD(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;

	enterCritical();
	u16ToStr(Sms.Pid.GainD,packet->data);
	exitCritical();
}

/*67*/	void zoCommandGetProfileAcceleration(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;

	enterCritical();
	u16ToStr(Sms.Profile.DesiredAcceleration,packet->data);
	exitCritical();
}

/*68*/	void zoCommandGetProfileVelocity(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;
	
	enterCritical();
	u16ToStr(Sms.Profile.DesiredVelocity,packet->data);
	exitCritical();
}

/*69*/	void zoCommandGetCurrentLimit(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;
	
	enterCritical();
	u16ToStr(Sms.Settings.CurrentLimit,packet->data);
	exitCritical();
}

/*6A*/	void zoCommandGetCurrentLimitDuration(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;
	
	enterCritical();
	u16ToStr(Sms.Settings.CurrentLimitDuration,packet->data);
	exitCritical();
}
/*6B*/	void zoCommandGetDigitalIoConfiguration(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 1;
	packet->data[0] = Sms.Settings.DigitalIoConfig;
}
/*6C*/	void zoCommandGetLocalAcceptanceMask(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 1;
	packet->data[0]	= Sms.Settings.localAcceptanceMask;
}

/*6D*/	void zoCommandGetDigitalIn(ZO_PROTOCOL_PACKET *packet)
{
	u08 in=0;

	packet->byteCount = 1;

	enterCritical();
	in = (PIND>>7) & 0x01;
	in |= (PINB & 0x38) >> 2;
	exitCritical();

	packet->data[0] = in;
}

/*6E*/  void zoCommandGetAnalogIn(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 8;
	u16ToStr(zoAdcRead(ZO_ADC_CHANNEL_0),packet->data);
	u16ToStr(zoAdcRead(ZO_ADC_CHANNEL_1),&packet->data[2]);
	u16ToStr(zoAdcRead(ZO_ADC_CHANNEL_2),&packet->data[4]);
	u16ToStr(zoAdcRead(ZO_ADC_CHANNEL_3),&packet->data[6]);
}

/*6F*/  void zoCommandGetPosition(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 4;

	s32ToStr(zoEncoderGetPos(),packet->data);
}

/*70*/	void zoCommandGetAbsolutePosition(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;

	u16ToStr(zoEncoderGetPosAbsolute(),packet->data);
}
/*71*/	void zoCommandGetVelocity(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;

	s16ToStr(zoEncoderGetVel(),packet->data);	
}

/*72*/	void zoCommandGetCurrent(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 2;

	u16ToStr(zoAdcRead(ZO_ADC_CHANNEL_7),packet->data);
}

/*73*/	void zoCommandGetWarning(ZO_PROTOCOL_PACKET *packet)
{
	packet->byteCount = 0;
	while(!zoErrorIsEmpty(&zoSmsWarning))
	{
		packet->data[packet->byteCount] = zoErrorGet(&zoSmsWarning);
		packet->byteCount++;
	}
}

/*C8*/	void zoCommandDoMove(ZO_PROTOCOL_PACKET *packet)
{
	enterCritical();
	ZO_SMS_SETPOINT_TYPE spType = Sms.BufferdSetPoint.type;
	exitCritical();

	switch(spType)
	{
	case ZO_SMS_SETPOINT_NONE:
		return;
		break;

	case ZO_SMS_SETPOINT_VELOCITY:
		s16ToStr((s16)Sms.BufferdSetPoint.value,packet->data);
		zoCommandVelocityMove(packet);
		break;

	case ZO_SMS_SETPOINT_ABSOLUTE_POSITION:
		s32ToStr(Sms.BufferdSetPoint.value,packet->data);
		zoCommandAbsolutePositionMove(packet);
		break;

	case ZO_SMS_SETPOINT_RELATIVE_POSITION:
		s32ToStr(Sms.BufferdSetPoint.value,packet->data);
		zoCommandRelativePositionMove(packet);
		break;

	case ZO_SMS_SETPOINT_PROFILED_VELOCITY:
		s16ToStr(Sms.BufferdSetPoint.value,packet->data);
		zoCommandProfiledVelocityMove(packet);
		break;

	case ZO_SMS_SETPOINT_PROFILED_ABSOLUTE_POSITION:
		zoProfiledMoveAbsolute(Sms.BufferdSetPoint.value);
		break;

	case ZO_SMS_SETPOINT_PROFILED_RELATIVE_POSITION:
		zoProfiledMoveAbsolute(Sms.BufferdSetPoint.value + zoEncoderGetPos());
		break;
	}
	
	//indicate execution of buffered setpoint
	enterCritical();
	Sms.BufferdSetPoint.type = ZO_SMS_SETPOINT_NONE;
	exitCritical();
}
/*C9*/	//zoCommandGlobalStart(ZO_PROTOCOL_PACKET *packet){}
/*CA*/	//zoCommandGlobalHalt(ZO_PROTOCOL_PACKET *packet){}
/*CB*/	//zoCommandGlobalStop(ZO_PROTOCOL_PACKET *packet){}

void zoProfiledVelocity(s16 velocity)
{
	s32 currentPos;
	s32 accelKnee;
	s32 currentVel;
	s32 accel;
	s32 vel;
		
	s08 s=1;
	vel = (s32)velocity;

	enterCritical();
	if(Sms.Control.State == ZO_SMS_CONTROL_POSITION_CONTROL)					//if previous state was again profiled
	{
		currentPos = zoEncoderGetPos();
		currentVel = zoEncoderGetVel();
	}
	else
	{
		currentVel = (s32)(Sms.Profile.VelocityDeltaPos * Sms.Control.LoopHz);
		currentPos = (s32)Sms.Profile.PositionSetpoint;
	}

	s = ( vel >= currentVel ) ? 1 : -1;
	accel = ( s == 1 ) ? (s32)Sms.Profile.DesiredAcceleration : 
							   -((s32)Sms.Profile.DesiredAcceleration);
	exitCritical();

	accelKnee = ( vel*vel - currentVel*currentVel ) / accel / 2 + currentPos;
	
	enterCritical();
	Sms.Profile.PositionSetpoint = currentPos;
	Sms.Profile.AccelerationKnee = accelKnee;
	Sms.Control.State = ZO_SMS_CONTROL_PROFILED_VELOCITY_CONTROL;
	Sms.Profile.TargetVelDeltaPos = (double)vel/Sms.Control.LoopHz;
	Sms.Profile.VelocityDeltaPos = (double)currentVel/Sms.Control.LoopHz;
	Sms.Profile.LastVelocityDeltaPos = Sms.Profile.VelocityDeltaPos;
	Sms.Profile.ConstAccelDeltaVel = (double)accel/Sms.Control.LoopHz/Sms.Control.LoopHz;
	exitCritical();
}

void zoProfiledMoveAbsolute(s32 finalPos)
{
	s32	currentPos = 0;
	s32 accel = 0;
	s32 deccel = 0;
	s32 vel = 0;
	s32 currentVel = 0;
	s32 accelKnee = 0;
	s32 deccelKnee = 0;
	s32 velInt = 0;
	s32 velCalc = 0;
	s32 posCalc = 0;
	bool zeroCross = FALSE;
	s08 s = 1;

	enterCritical();
	if(Sms.Control.State == ZO_SMS_CONTROL_POSITION_CONTROL)					
	{
		currentVel = zoEncoderGetVel();
		currentPos = zoEncoderGetPos();
	}
	else
	{																			//if previous state was again profiled
		currentVel = (s32)(Sms.Profile.VelocityDeltaPos * Sms.Control.LoopHz);	//do not requery velocity
		currentPos = (s32)(Sms.Profile.PositionSetpoint);						//and position
	}

	s = ( finalPos >= currentPos ) ? 1 : -1;	//calculate sign
	accel = ( s == 1 ) ? (s32)Sms.Profile.DesiredAcceleration : 
							   -((s32)Sms.Profile.DesiredAcceleration);			//get acceleration value
	deccel = -accel ;					
	vel = ( s == 1 ) ? (s32)Sms.Profile.DesiredVelocity : 
							  -((s32)Sms.Profile.DesiredVelocity);				//get target velocity
	exitCritical();

	if ( (currentVel * vel) >= 0 )	//does the velocity change sign ?
	{								//if not keep clculations as they are
		velCalc = currentVel;
		posCalc = currentPos;
		zeroCross = FALSE;
	}
	else
	{								//else calculate as if we started from zero velocity
		zeroCross = TRUE;
		velCalc = 0;
		posCalc = currentPos - ( currentVel * currentVel ) / 2 / accel ;
	}

	//calculate the square of the velocity 
	//at the intersection of the acceleration and decceleration curves
	velInt = s * (s32)sqrt( (double)(accel * ( finalPos - posCalc ) + ( velCalc * velCalc ) / 2) );
	
	//we must deccelerate now with given decceleration but we will overshoot and
	//then we will need to go back. Make necessary changes.
	if(  ( (velCalc -  velInt) * s > 0 ) && (!zeroCross) ) 
	{
		accel = -accel;
		deccel = -deccel;
		velCalc = 0;
		posCalc = currentPos - ( currentVel * currentVel ) / 2 / accel ;		
		velInt =  (-s) * (s32)sqrt( (double)(accel * ( finalPos - posCalc ) + ( velCalc * velCalc ) / 2 ) );
	}

	if ( (velInt - vel ) * s >= 0 ) //we can do full profile
	{								
		accel = ( (vel - velCalc ) * s >= 0 ) ? accel : -accel;	 //check wether we must initialy accelerate or deccelerate
		accelKnee = ( vel * vel - velCalc * velCalc ) / 2 / accel + posCalc;	
		deccelKnee = finalPos + vel * vel / 2 / deccel;
	}
	else 	//we cannot reach the desired velocity but we can go with desired accel, deccel
	{		//accelKnee = deccelKnee = intersection
		accelKnee = (finalPos + posCalc) / 2 - ( velCalc * velCalc ) / 4 / accel;
		deccelKnee = accelKnee;
		vel = velInt;
	}

	//initialise profile and calculate delta pos and delta u
	//to achieve velocity and acceleration at control loop
	enterCritical();
	Sms.Profile.PositionSetpoint = currentPos;
	Sms.Profile.AccelerationKnee = accelKnee;
	Sms.Profile.DeccelerationKnee = deccelKnee;
	Sms.Profile.TargetPosition = finalPos;
	Sms.Profile.State = ZO_SMS_PROFILE_ACCELERATING;
	Sms.Control.State = ZO_SMS_CONTROL_PROFILED_POSITION_CONTROL;
	Sms.Profile.TargetVelDeltaPos = (double)vel/Sms.Control.LoopHz;
	Sms.Profile.VelocityDeltaPos =(double)currentVel/Sms.Control.LoopHz;
	Sms.Profile.LastVelocityDeltaPos = Sms.Profile.VelocityDeltaPos;
	Sms.Profile.ConstAccelDeltaVel = (double)accel/Sms.Control.LoopHz/Sms.Control.LoopHz;
	exitCritical();
}
