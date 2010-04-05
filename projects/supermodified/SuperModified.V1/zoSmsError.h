#ifndef ZO_SMS_ERROR_H
#define ZO_SMS_ERROR_H

#include "zoTypes.h"
#include "zoError.h"
#include "zoProtocol.h"

//Error offsets
#define ZO_SMS_ERROR_SYSTEM_ERRORS_OFFSET					0x00
#define ZO_SMS_ERROR_COMMAND_ERRORS_OFFSET					0x10
#define ZO_SMS_ERROR_I2C_ERRORS_OFFSET						0x20
#define ZO_SMS_ERROR_UART_ERRORS_OFFSET						0x40
#define ZO_SMS_ERROR_PROTOCOL_ERRORS_OFFSET					0x30

//Errors__________________________________________________________________________________
//System errors
#define ZO_SMS_ERROR_NONE									0x00 + 0x00
#define ZO_SMS_ERROR_MOTOR_STALLED							0x00 + 0x01		//error
#define ZO_SMS_ERROR_ENCODER_OVERFLOW						0x00 + 0x02		//error
#define ZO_SMS_ERROR_ENCODER_UNDERFLOW						0x00 + 0x03		//error

//command related errors
#define ZO_SMS_ERROR_COMMAND_INVALID_ID						(0x10 + 0x01)	//error
#define ZO_SMS_ERROR_COMMAND_INVALID_SET_COMMAND_BYTECOUNT	(0x10 + 0x02)	//error
#define ZO_SMS_ERROR_COMMAND_INVALID_ARGUMENT				(0x10 + 0x03)	//error
#define ZO_SMS_ERROR_COMMAND_INVALID_FOR_MOTOR_STATE		(0x10 + 0x04)	//error
#define ZO_SMS_ERROR_COMMAND_INVALID_BYTECOUNT				(0x10 + 0x05)	//warning

//i2c related errors
#define ZO_SMS_ERROR_I2C_ARBITRATION_LOST					(0x20 + 0x03)	//warning
#define ZO_SMS_ERROR_I2C_PACKET_OVERWRITTEN					(0x20 + 0x11)	//error
#define ZO_SMS_ERROR_I2C_INVALID_RX_BYTECOUNT				(0x20 + 0x12)	//error

//uart related errors
#define ZO_SMS_ERROR_UART_MEMORY_ALLOCATION_ERROR			(0x40 + 0x01)	//error
#define ZO_SMS_ERROR_UART_RX_FRAME_ERROR					(0x40 + 0x06)	//error
#define ZO_SMS_ERROR_UART_PARITY_ERROR						(0x40 + 0x07)	//error
#define ZO_SMS_ERROR_UART_RX_BUFFER_OVERFLOW				(0x40 + 0x08)	//error
#define ZO_SMS_ERROR_UART_RX_DATA_OVERWRITTEN				(0x40 + 0x09)	//error

//protocol related errors
#define ZO_SMS_ERROR_PROTOCOL_WRONG_LRC						(0x30 + 0x22)	//error

extern ZO_ERROR zoSmsError;
extern ZO_ERROR zoSmsWarning;

void zoSmsErrorInit(void);

void zoSmsErrorPoll(void);

void zoSmsErrorHandleParseFailure(ZO_PROTOCOL_HAL *hal,ZO_PROTOCOL_PACKET *p);
void zoSmsErrorHandleServiceCommandFailure(void);
void zoSmsErrorHandleCommandResponseFailure(ZO_PROTOCOL_HAL *hal,ZO_PROTOCOL_PACKET *p);

bool zoSmsMotorStalled(void);




#endif //ZO_SMS_ERROR_H