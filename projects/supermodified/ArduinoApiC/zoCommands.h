#ifndef ZO_COMMANDS_H
#define ZO_COMMANDS_H

#include "zoTypes.h"
#include "zoProtocol.h"

#define	ZO_COMMANDS_ERROR_NONE								0x60
#define ZO_COMMANDS_ERROR_INVALID_SMS_RESPONSE				0x61


#define ZO_SMS_ERROR_I2C_ERRORS_OFFSET						0x90
#define ZO_SMS_ERROR_UART_ERRORS_OFFSET						0xB0
#define ZO_SMS_ERROR_PROTOCOL_ERRORS_OFFSET					0xA0

extern ZO_ERROR zoCommandsError;

//Command set_____________________________________________________________________________
typedef enum {
/*00*/	ZO_COMMAND_SET_GAIN_P = ZO_PROTOCOL_SET_COMMANDS_START,
/*01*/	ZO_COMMAND_SET_GAIN_I,
/*02*/	ZO_COMMAND_SET_GAIN_D,
/*03*/	ZO_COMMAND_SET_PROFILE_ACCELERATION,
/*04*/	ZO_COMMAND_SET_PROFILE_VELOCITY,
/*05*/	ZO_COMMAND_SET_CURRENT_LIMIT,
/*06*/	ZO_COMMAND_SET_CURRENT_LIMIT_DURATION,
/*07*/	ZO_COMMAND_VELOCITY_MOVE,
/*08*/	ZO_COMMAND_ABSOLUTE_POSITION_MOVE,
/*09*/	ZO_COMMAND_RELATIVE_POSITION_MOVE,
/*0A*/  ZO_COMMAND_PROFILED_VELOCITY_MOVE,
/*0B*/  ZO_COMMAND_PROFILED_ABSOLUTE_POSITION_MOVE,
/*0C*/	ZO_COMMAND_PROFILED_RELATIVE_POSITION_MOVE,
/*0D*/	ZO_COMMAND_SET_VELOCITY_SETPOINT,
/*1E*/	ZO_COMMAND_SET_ABSOLUTE_POSITION_SETPOINT,
/*1F*/	ZO_COMMAND_SET_RELATIVE_POSITION_SETPOINT,
/*10*/	ZO_COMMAND_SET_PROFILED_VELOCITY_SETPOINT,
/*11*/	ZO_COMMAND_SET_PROFILED_ABSOLUTE_POSITION_SETPOINT,
/*12*/	ZO_COMMAND_SET_PROFILED_RELATIVE_POSITION_SETPOINT,
/*13*/	ZO_COMMAND_CONFIGURE_DIGITAL_IO,
/*14*/	ZO_COMMAND_SET_DIGITAL_OUT,
/*15*/	ZO_COMMAND_SET_NODE_ID,
/*16*/  ZO_COMMAND_SET_LOCAL_ACCEPTANCE_MASK,
/*17*/  ZO_COMMAND_SET_BAUD_UART,
/*18*/	ZO_COMMAND_RESET_POSITION,
/*19*/	ZO_COMMAND_START,
/*1A*/	ZO_COMMAND_HALT,
/*1B*/	ZO_COMMAND_STOP,
/*1C*/  ZO_COMMAND_SET_ERROR_REPORTING_LEVEL,
/*1D*/	ZO_COMMAND_SET_COMMANDS_END
}ZO_SET_COMMANDS;

typedef enum {
/*64*/	ZO_COMMAND_GET_GAIN_P = ZO_PROTOCOL_GET_COMMANDS_START,
/*65*/	ZO_COMMAND_GET_GAIN_I,
/*66*/	ZO_COMMAND_GET_GAIN_D,
/*67*/	ZO_COMMAND_GET_PROFILE_ACCELERATION,
/*68*/	ZO_COMMAND_GET_PROFILE_VELOCITY,
/*69*/	ZO_COMMAND_GET_CURRENT_LIMIT,
/*6A*/	ZO_COMMAND_GET_CURRENT_LIMIT_DURATION,
/*6B*/	ZO_COMMAND_GET_DIO_CONFIGURATION,
/*6C*/	ZO_COMMAND_GET_LOCAL_ACCEPTANCE_MASK,
/*6D*/	ZO_COMMAND_GET_DIGITAL_IN,
/*6E*/  ZO_COMMAND_GET_ANALOG_IN,
/*6F*/  ZO_COMMAND_GET_POSITION,
/*70*/	ZO_COMMAND_GET_ABSOLUTE_POSITION,
/*71*/	ZO_COMMAND_GET_VELOCITY,
/*72*/	ZO_COMMAND_GET_CURRENT,
/*73*/	ZO_COMMAND_GET_WARNING,
/*74*/	ZO_COMMANDS_GET_COMMANDS_END
}ZO_GET_COMMANDS;

typedef enum {
/*C8*/	ZO_COMMAND_BRC_DO_MOVE = ZO_PROTOCOL_BRC_COMMANDS_START,
/*C9*/	ZO_COMMAND_BRC_START,
/*CA*/	ZO_COMMAND_BRC_HALT,
/*CB*/	ZO_COMMAND_BRC_STOP,
/*CC*/	ZO_COMMAND_BRC_COMMANDS_END
}ZO_BRC_COMMANDS;


void zoCommandsInit(ZO_PROTOCOL_HAL *hal);
bool zoCommunicatePacket(ZO_PROTOCOL_PACKET *packet);

//command function prototypes_____________________________________________________________
/*00*/	bool zoCommandSetGainP(u08 nodeId, u16 gainP);
/*01*/	bool zoCommandSetGainI(u08 nodeId, u16 gainI);
/*02*/	bool zoCommandSetGainD(u08 nodeId, u16 gainD);
/*03*/	bool zoCommandSetProfileAcceleration(u08 nodeId, u16 ticksPerSec2);
/*04*/	bool zoCommandSetProfileVelocity(u08 nodeId, u16 ticksPerSec);
/*05*/	bool zoCommandSetCurrentLimit(u08 nodeId, u16 miliAmps);
/*06*/	bool zoCommandSetCurrentLimitDuration(u08 nodeId, u16 miliSecs);
/*07*/	bool zoCommandVelocityMove(u08 nodeId, s16 ticksPerSec);
/*08*/	bool zoCommandAbsolutePositionMove(u08 nodeId, s32 ticks);
/*09*/	bool zoCommandRelativePositionMove(u08 nodeId, s32 ticks);
/*0A*/  bool zoCommandProfiledVelocityMove(u08 nodeId, s16 ticksPerSec);
/*0B*/  bool zoCommandProfiledAbsolutePositionMove(u08 nodeId, s32 ticks);
/*0C*/	bool zoCommandProfiledRelativePositionMove(u08 nodeId, s32 ticks);
/*0D*/	bool zoCommandSetVelocitySetpoint(u08 nodeId, s16 ticksPerSec);
/*0E*/	bool zoCommandSetAbsolutePositionSetpoint(u08 nodeId, s32 ticks);
/*0F*/	bool zoCommandSetRelativePositionSetpoint(u08 nodeId, s32 ticks);
/*10*/	bool zoCommandSetProfiledVelocitySetpoint(u08 nodeId, s16 ticksPerSec);
/*11*/	bool zoCommandSetProfiledAbsolutePositionSetpoint(u08 nodeId, s32 ticks);
/*12*/	bool zoCommandSetProfiledRelativePositionSetpoint(u08 nodeId, s32 ticks);
/*13*/	bool zoCommandConfigureDigitalIO(u08 nodeId, u08 portDirection);
/*14*/	bool zoCommandSetDigitalOut(u08 nodeId, u08 portPins);
/*15*/	bool zoCommandSetNodeID(u08 nodeId, u08 newNodeId);
/*16*/  bool zoCommandSetLocalAcceptanceMask(u08 nodeId, u08 lam);
/*17*/  bool zoCommandSetBaudUart(u08 nodeId, u32 bps);
/*18*/	bool zoCommandResetPosition(u08 nodeId);
/*19*/	bool zoCommandStart(u08 nodeId);
/*1A*/	bool zoCommandHalt(u08 nodeId);
/*1B*/	bool zoCommandStop(u08 nodeId);
/*1C*/  bool zoCommandSetErrorReportingLevel(u08 nodeId, u08 errReportLvl);

/*64*/	bool zoCommandGetGainP(u08 nodeId, u16 *gainP);
/*65*/	bool zoCommandGetGainI(u08 nodeId, u16 *gainI);
/*66*/	bool zoCommandGetGainD(u08 nodeId, u16 *gainD);
/*67*/	bool zoCommandGetProfileAcceleration(u08 nodeId, u16 *ticksPerSec2);
/*68*/	bool zoCommandGetProfileVelocity(u08 nodeId, u16 *ticksPerSec);
/*69*/	bool zoCommandGetCurrentLimit(u08 nodeId, u16 *miliAmps);
/*6A*/	bool zoCommandGetCurrentLimitDuration(u08 nodeId, u16 *miliSecs);
/*6B*/	bool zoCommandGetDigitalIoConfiguration(u08 nodeId, u08 *portDir);
/*6C*/	bool zoCommandGetLocalAcceptanceMask(u08 nodeId, u08 *lam);
/*6D*/	bool zoCommandGetDigitalIn(u08 nodeId, u08 *pinState);
/*6E*/  bool zoCommandGetAnalogIn(u08 nodeId, u16 *fourElementResultArray);
/*6F*/  bool zoCommandGetPosition(u08 nodeId, s32 *ticks);
/*70*/	bool zoCommandGetAbsolutePosition(u08 nodeId, u16 *absTicks);
/*71*/	bool zoCommandGetVelocity(u08 nodeId, s16 *ticksPerSec);
/*72*/	bool zoCommandGetCurrent(u08 nodeId, s16 *miliAmp);
/*73*/	bool zoCommandGetWarning(u08 nodeId, u08 *warningBuffer,u08 *warningsCount);

/*C8*/	bool zoCommandDoMove(u08 nodeId);
/*C9*/	bool zoCommandGlobalStart(u08 nodeId);
/*CA*/	bool zoCommandGlobalHalt(u08 nodeId);
/*CB*/	bool zoCommandGlobalStop(u08 nodeId);

























#endif	//ZO_COMMANDS_H