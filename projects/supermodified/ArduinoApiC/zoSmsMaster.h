#ifndef ZO_SMS_MASTER_H
#define ZO_SMS_MASTER_H

#include "zoTypes.h"
#include "zoError.h"
#include "zoProtocol.h"

//SuperModified Errors____________________________________________________________________
//System errors
#define ZO_SMS_ERROR_NONE									0x00
#define ZO_SMS_ERROR_MOTOR_STALLED							0x01		//error
#define ZO_SMS_ERROR_ENCODER_OVERFLOW						0x02		//error
#define ZO_SMS_ERROR_ENCODER_UNDERFLOW						0x03		//error

//command related errors
#define ZO_SMS_ERROR_COMMAND_INVALID_ID						0x11	//error
#define ZO_SMS_ERROR_COMMAND_INVALID_SET_COMMAND_BYTECOUNT	0x12	//error
#define ZO_SMS_ERROR_COMMAND_INVALID_ARGUMENT				0x13	//error
#define ZO_SMS_ERROR_COMMAND_INVALID_FOR_MOTOR_STATE		0x14	//error
#define ZO_SMS_ERROR_COMMAND_INVALID_BYTECOUNT				0x15	//warning

//i2c related errors
#define ZO_SMS_ERROR_I2C_ARBITRATION_LOST					0x23	//warning
#define ZO_SMS_ERROR_I2C_PACKET_OVERWRITTEN					0x31	//error
#define ZO_SMS_ERROR_I2C_INVALID_RX_BYTECOUNT				0x32	//error

//uart related errors
#define ZO_SMS_ERROR_UART_MEMORY_ALLOCATION_ERROR			0x41	//error
#define ZO_SMS_ERROR_UART_RX_FRAME_ERROR					0x46	//error
#define ZO_SMS_ERROR_UART_PARITY_ERROR						0x47	//error
#define ZO_SMS_ERROR_UART_RX_BUFFER_OVERFLOW				0x48	//error
#define ZO_SMS_ERROR_UART_RX_DATA_OVERWRITTEN				0x49	//error

//protocol related errors
#define ZO_SMS_ERROR_PROTOCOL_WRONG_LRC						0x52	//error

//Master Errors___________________________________________________________________________
#define ZO_SMS_MASTER_ERROR_INVALID_SMS_RESPONSE			0x61	//means supermodified replied tottally wrong

//i2c master errors
#define ZO_SMS_MASTER_ERROR_I2C_NODEV						0x61
#define ZO_SMS_MASTER_ERROR_I2C_BUS_ERROR					0x92
#define ZO_SMS_MASTER_ERROR_I2C_ARBITRATION_LOST			0x93
#define ZO_SMS_MASTER_ERROR_I2C_INDETERMINATE				0x94
#define ZO_SMS_MASTER_ERROR_I2C_WAIT_ON_BUS_READY_TIMEOUT	0x95
#define ZO_SMS_MASTER_ERROR_I2C_WAIT_ON_TRANSMIT_TIMEOUT	0x96
#define ZO_SMS_MASTER_ERROR_I2C_WAIT_ON_RECEIVE_TIMEOUT		0x97

//protocol master errors
#define ZO_SMS_MASTER_ERROR_RESPONSE_TIMEOUT				0xC1	//supermodified did not issue a command response within timeout
#define ZO_SMS_MASTER_ERROR_WRONG_LRC						0xC2	//supermodified replied with a package with wrong lrc



extern ZO_ERROR *zoSmsMasterError;

void zoSmsMasterInit(ZO_PROTOCOL_HW_TYPE hw,ZO_PROTOCOL_HAL *hal, u08 localNodeID);

#endif //ZO_SMS_MASTER_H