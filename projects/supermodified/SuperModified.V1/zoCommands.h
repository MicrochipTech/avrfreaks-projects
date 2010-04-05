#ifndef ZO_COMMANDS_H
#define ZO_COMMANDS_H

#include "zoTypes.h"
#include "zoProtocol.h"


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
/*1D*/	ZO_COMMANDS_SET_COMMANDS_END
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
/*C8*/	ZO_COMMAND_DO_MOVE = ZO_PROTOCOL_BRC_COMMANDS_START,
/*C9*/	ZO_COMMAND_GLOBAL_START,
/*CA*/	ZO_COMMAND_GLOBAL_HALT,
/*CB*/	ZO_COMMAND_GLOBAL_STOP,
/*CC*/	ZO_COMMANDS_BRC_COMMANDS_END
}ZO_BRC_COMMANDS;

typedef void (*ZO_COMMAND_FUNCTOR)(ZO_PROTOCOL_PACKET *packet);

//error codes and handling
#define ZO_COMMANDS_ERROR_NONE							0x00
#define ZO_COMMANDS_ERROR_INVALID_COMMAND_ID			0x01
#define ZO_COMMANDS_ERROR_INVALID_SET_BYTECOUNT			0x02
#define ZO_COMMANDS_ERROR_INVALID_ARGUMENT				0x03
#define ZO_COMMANDS_ERROR_INVALID_COMMAND				0x04
#define ZO_COMMANDS_ERROR_INVALID_BYTECOUNT				0x05


extern ZO_ERROR zoCommandsError;

//utility functions_______________________________________________________________________
void zoCommandsInit(void);
bool zoCommandsServiceCommand(ZO_PROTOCOL_PACKET *packet);

//command function prototypes_____________________________________________________________
/*00*/	void zoCommandSetGainP(ZO_PROTOCOL_PACKET *packet);
/*01*/	void zoCommandSetGainI(ZO_PROTOCOL_PACKET *packet);
/*02*/	void zoCommandSetGainD(ZO_PROTOCOL_PACKET *packet);
/*03*/	void zoCommandSetProfileAcceleration(ZO_PROTOCOL_PACKET *packet);
/*04*/	void zoCommandSetProfileVelocity(ZO_PROTOCOL_PACKET *packet);
/*05*/	void zoCommandSetCurrentLimit(ZO_PROTOCOL_PACKET *packet);
/*06*/	void zoCommandSetCurrentLimitDuration(ZO_PROTOCOL_PACKET *packet);
/*07*/	void zoCommandVelocityMove(ZO_PROTOCOL_PACKET *packet);
/*08*/	void zoCommandAbsolutePositionMove(ZO_PROTOCOL_PACKET *packet);
/*09*/	void zoCommandRelativePositionMove(ZO_PROTOCOL_PACKET *packet);
/*0A*/  void zoCommandProfiledVelocityMove(ZO_PROTOCOL_PACKET *packet);
/*0B*/  void zoCommandProfiledAbsolutePositionMove(ZO_PROTOCOL_PACKET *packet);
/*0C*/	void zoCommandProfiledRelativePositionMove(ZO_PROTOCOL_PACKET *packet);
/*0D*/	void zoCommandSetVelocitySetpoint(ZO_PROTOCOL_PACKET *packet);
/*0E*/	void zoCommandSetAbsolutePositionSetpoint(ZO_PROTOCOL_PACKET *packet);
/*0F*/	void zoCommandSetRelativePositionSetpoint(ZO_PROTOCOL_PACKET *packet);
/*10*/	void zoCommandSetProfiledVelocitySetpoint(ZO_PROTOCOL_PACKET *packet);
/*11*/	void zoCommandSetProfiledAbsolutePositionSetpoint(ZO_PROTOCOL_PACKET *packet);
/*12*/	void zoCommandSetProfiledRelativePositionSetpoint(ZO_PROTOCOL_PACKET *packet);
/*13*/	void zoCommandConfigureDigitalIO(ZO_PROTOCOL_PACKET *packet);
/*14*/	void zoCommandSetDigitalOut(ZO_PROTOCOL_PACKET *packet);
/*15*/	void zoCommandSetNodeID(ZO_PROTOCOL_PACKET *packet);
/*16*/  void zoCommandSetLocalAcceptanceMask(ZO_PROTOCOL_PACKET *packet);
/*17*/  void zoCommandSetBaudUart(ZO_PROTOCOL_PACKET *packet);
/*18*/	void zoCommandResetPosition(ZO_PROTOCOL_PACKET *packet);
/*19*/	void zoCommandStart(ZO_PROTOCOL_PACKET *packet);
/*1A*/	void zoCommandHalt(ZO_PROTOCOL_PACKET *packet);
/*1B*/	void zoCommandStop(ZO_PROTOCOL_PACKET *packet);
/*1C*/  void zoCommandSetErrorReportingLevel(ZO_PROTOCOL_PACKET *packet);

/*64*/	void zoCommandGetGainP(ZO_PROTOCOL_PACKET *packet);
/*65*/	void zoCommandGetGainI(ZO_PROTOCOL_PACKET *packet);
/*66*/	void zoCommandGetGainD(ZO_PROTOCOL_PACKET *packet);
/*67*/	void zoCommandGetProfileAcceleration(ZO_PROTOCOL_PACKET *packet);
/*68*/	void zoCommandGetProfileVelocity(ZO_PROTOCOL_PACKET *packet);
/*69*/	void zoCommandGetCurrentLimit(ZO_PROTOCOL_PACKET *packet);
/*6A*/	void zoCommandGetCurrentLimitDuration(ZO_PROTOCOL_PACKET *packet);
/*6B*/	void zoCommandGetDigitalIoConfiguration(ZO_PROTOCOL_PACKET *packet);
/*6C*/	void zoCommandGetLocalAcceptanceMask(ZO_PROTOCOL_PACKET *packet);
/*6D*/	void zoCommandGetDigitalIn(ZO_PROTOCOL_PACKET *packet);
/*6E*/  void zoCommandGetAnalogIn(ZO_PROTOCOL_PACKET *packet);
/*6F*/  void zoCommandGetPosition(ZO_PROTOCOL_PACKET *packet);
/*70*/	void zoCommandGetAbsolutePosition(ZO_PROTOCOL_PACKET *packet);
/*71*/	void zoCommandGetVelocity(ZO_PROTOCOL_PACKET *packet);
/*72*/	void zoCommandGetCurrent(ZO_PROTOCOL_PACKET *packet);
/*73*/	void zoCommandGetWarning(ZO_PROTOCOL_PACKET *packet);

/*C8*/	void zoCommandDoMove(ZO_PROTOCOL_PACKET *packet);
/*C9*/	//zoCommandGlobalStart(ZO_PROTOCOL_PACKET *packet);
/*CA*/	//zoCommandGlobalHalt(ZO_PROTOCOL_PACKET *packet);
/*CB*/	//zoCommandGlobalStop(ZO_PROTOCOL_PACKET *packet);

void zoProfiledVelocity(s16 velocity);
void zoProfiledMoveAbsolute(s32 setpoint);
























#endif	//ZO_COMMANDS_H