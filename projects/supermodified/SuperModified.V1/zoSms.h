#ifndef ZO_SUPERMODIFIED_H
#define ZO_SUPERMODIFIED_H

#include "zoTypes.h"
#include "zoPid.h"
#include <stdLib.h>
#include "zoProtocol.h"
#include "zoEncoder.h"
#include "zoError.h"

#define ZO_SMS_CONTROL_LOOP_HZ_APPROX	1000
#define ZO_SMS_MOTOR_STALLED_TIMEOUT_MS 2000

//Eeprom adresses for settings____________________________________________________________
#define ZO_EEPROM_ADDRESS_NODE_ID					0x0000
#define ZO_EEPROM_ADDRESS_GAIN_P					0x0010
#define ZO_EEPROM_ADDRESS_GAIN_I					0x0020
#define ZO_EEPROM_ADDRESS_GAIN_D					0x0030
#define ZO_EEPROM_ADDRESS_CURRENT_LIMIT				0x0040
#define ZO_EEPROM_ADDRESS_CURRENT_LIMIT_DURATION	0x0050
#define ZO_EEPROM_ADDRESS_PROFILE_ACCELERATION		0x0060
#define ZO_EEPROM_ADDRESS_PROFILE_VELOCITY			0x0070
#define ZO_EEPROM_ADDRESS_DIGITAL_IO_CONFIG			0x0080
#define ZO_EEPROM_ADDRESS_BAUD_I2C					0x0090
#define ZO_EEPROM_ADDRESS_BAUD_UART					0x00A0
#define ZO_EEPROM_ADDRESS_LAM						0x00B0
#define ZO_EEPROM_ADDRESS_ERROR_REPORTING_LVL		0x00C0

//Default settings values
#define ZO_DEFAULT_NODE_ID					0x04
#define ZO_DEFAULT_GAIN_P					8000
#define ZO_DEFAULT_GAIN_I					4000
#define ZO_DEFAULT_GAIN_D					8000
#define ZO_DEFAULT_CURRENT_LIMIT			5000
#define ZO_DEFAULT_CURRENT_LIMIT_DURATION	5000
#define ZO_DEFAULT_PROFILE_ACCELERATION		800
#define ZO_DEFAULT_PROFILE_VELOCITY			800
#define ZO_DEFAULT_DIGITAL_IO_CONFIG		0
#define ZO_DEFAULT_BAUD_I2C					100000
#define ZO_DEFAULT_BAUD_UART				115200
#define ZO_DEFAULT_LAM						ZO_PROTOCOL_DEFAULT_LAM
#define ZO_DEFAULT_ERROR_REPORTING_LVL		0

//Types___________________________________________________________________________________
typedef enum ZO_SMS_CONTROL_STATE{
	ZO_SMS_CONTROL_NO_CONTROL,
	ZO_SMS_CONTROL_POSITION_CONTROL,
	ZO_SMS_CONTROL_VELOCITY_CONTROL,
	ZO_SMS_CONTROL_PROFILED_VELOCITY_CONTROL,
	ZO_SMS_CONTROL_PROFILED_POSITION_CONTROL,
}ZO_SMS_CONTROL_STATE;

typedef enum ZO_SMS_PROFILE_STATE{
	ZO_SMS_PROFILE_ACCELERATING,
	ZO_SMS_PROFILE_MOVING_WITH_VELOCITY,
	ZO_SMS_PROFILE_DECCELERATING
}ZO_SMS_PROFILE_STATE;

typedef struct ZO_SMS_CONTROL {
	ZO_SMS_CONTROL_STATE State;
	double LoopHz;
}ZO_SMS_CONTROL;

typedef struct ZO_SMS_SETTINGS{
	u08 NodeID;
	u08 DigitalIoConfig;
	u16 CurrentLimit;
	u16 CurrentLimitDuration;
	u32 BaudUart;
	u32 BaudI2C;
	u08 localAcceptanceMask;
	u08 errorReportingLevel;
}ZO_SMS_SETTINGS;

typedef enum ZO_SMS_SETPOINT_TYPE{
	ZO_SMS_SETPOINT_NONE = 0,
	ZO_SMS_SETPOINT_VELOCITY,
	ZO_SMS_SETPOINT_ABSOLUTE_POSITION,
	ZO_SMS_SETPOINT_RELATIVE_POSITION,
	ZO_SMS_SETPOINT_PROFILED_VELOCITY,
	ZO_SMS_SETPOINT_PROFILED_ABSOLUTE_POSITION,
	ZO_SMS_SETPOINT_PROFILED_RELATIVE_POSITION
}ZO_SMS_SETPOINT_TYPE;

typedef struct {
	ZO_SMS_SETPOINT_TYPE type;
	s32 value;
}ZO_SMS_BUFFERED_SETPOINT;

typedef struct ZO_SMS_PROFILE{
	double PositionSetpoint;
	double TargetVelDeltaPos;
	double VelocityDeltaPos;
	double LastVelocityDeltaPos;
	double ConstAccelDeltaVel;
	s32 AccelerationKnee;
	s32 DeccelerationKnee;
	s32 TargetPosition;
	u16 DesiredAcceleration;
	u16 DesiredVelocity;
	ZO_SMS_PROFILE_STATE State;
}ZO_SMS_PROFILE;

typedef struct ZO_SUPERMODIFIED{
	ZO_SMS_CONTROL	Control;
	ZO_SMS_SETTINGS	Settings;
	ZO_SMS_PROFILE	Profile;
	struct ZO_PID	Pid;
	ZO_SMS_BUFFERED_SETPOINT BufferdSetPoint;
}ZO_SUPERMODIFIED;

extern struct ZO_SUPERMODIFIED Sms;



//Functions_______________________________________________________________________________
void zoSmsInit(void);
void zoSmsPinsInit(void);
void zoSmsInitSettingsFromEeprom(void);
void zoSmsRestoreDefaults(void);
//bool zoSmsCheckErrors(void);

//control loop function
void zoSmsControlLoop(void);

//inline functions used for code clarity
bool accelerationCompleteCriterion(void);
bool constantVelocityCompletedCriterion(void);
bool positionReachedCriterion(void);
double deccelRecalc(void);



#endif // ZO_SUPERMODIFIED_H