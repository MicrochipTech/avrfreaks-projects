#ifndef ZO_I2C_H
#define ZO_I2C_H

#include "zoTypes.h"
#include "zoError.h"

//Conditional compile configuration: Select i2c functionalities needed
//Slave transmit and receive are always compiled.
#define ZO_I2C_ENABLE_MASTER_TRANSMIT			1
#define ZO_I2C_ENABLE_MASTER_RECEIVE			0

//Errors
#define ZO_I2C_ERROR_NONE						0x00
#define ZO_I2C_ERROR_NODEV						0x01
#define ZO_I2C_ERROR_BUS_ERROR					0x02
#define ZO_I2C_ERROR_ARBITRATION_LOST			0x03
#define ZO_I2C_ERROR_INDETERMINATE				0x04
#define ZO_I2C_ERROR_WAIT_ON_BUS_READY_TIMEOUT	0x05
#define ZO_I2C_ERROR_WAIT_ON_TRANSMIT_TIMEOUT	0x06
#define ZO_I2C_ERROR_WAIT_ON_RECEIVE_TIMEOUT	0x07

//Defaults
#define ZO_I2C_DEFAULT_BITRATE_KHZ								100
#define ZO_I2C_DEFAULT_WAIT_ON_BUS_READY_TIMEOUT_MILISECONDS	1000
#define ZO_I2C_DEFAULT_WAIT_ON_TRANSMIT_TIMEOUT_MILISECONDS		1000
#define ZO_I2C_DEFAULT_WAIT_ON_RECEIVE_TIMEOUT_MILISECONDS		1000
#define ZO_I2C_BUFFER_SIZE										32		//TODO:replace fixed buffers


extern ZO_ERROR zoI2cError;

void zoI2cInit(void);
bool zoI2cSetBitrate(u16 bitrateKHz);
bool zoI2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn);

void zoI2cSetWaitOnBusReadyTimeOut(u16 miliseconds);
void zoI2cSetWaitOnTransmitTimeOut(u16 miliseconds);
void zoI2cSetWaitOnReceiveTimeOut(u16 miliseconds);

void zoI2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(u08 receiveDataLength, u08* receiveData));
void zoI2cSetSlaveTransmitHandler(u08 (*i2cSlaveTx_func)(u08 transmitDataLengthMax, u08* transmitData));

// high-level I2C transaction commands
bool zoI2cMasterSend(u08 deviceAddr, u08 length, u08 *data);
bool zoI2cMasterSendBlocking(u08 deviceAddr, u08 length, u08 *data);
bool zoI2cMasterReceive(u08 deviceAddr, u08 length, u08* data);

// Low-level I2C transaction commands 
void zoI2cSendStart(void);
void zoI2cSendStop(void);
void zoI2cWaitForComplete(void);
void zoI2cSendByte(u08 data);
void zoI2cAcknowledgeRx(bool acknowledge);

// TWSR values (not bits) right shifted by 3
// (taken from avr-libc twi.h - thank you Marek Michalkiewicz)
// Master
#define TW_START					0x01
#define TW_REP_START				0x02
// Master Transmitter
#define TW_MT_SLA_ACK				0x03
#define TW_MT_SLA_NACK				0x04
#define TW_MT_DATA_ACK				0x05
#define TW_MT_DATA_NACK				0x06
#define TW_MT_ARB_LOST				0x07
// Master Receiver
#define TW_MR_ARB_LOST				0x07
#define TW_MR_SLA_ACK				0x08
#define TW_MR_SLA_NACK				0x09
#define TW_MR_DATA_ACK				0x0A
#define TW_MR_DATA_NACK				0x0B
// Slave Receiver
#define TW_SR_SLA_ACK				0x0C
#define TW_SR_ARB_LOST_SLA_ACK		0x0D
#define TW_SR_GCALL_ACK				0x0E
#define TW_SR_ARB_LOST_GCALL_ACK	0x0F
#define TW_SR_DATA_ACK				0x10
#define TW_SR_DATA_NACK				0x11
#define TW_SR_GCALL_DATA_ACK		0x12
#define TW_SR_GCALL_DATA_NACK		0x13
#define TW_SR_STOP					0x14
// Slave Transmitter
#define TW_ST_SLA_ACK				0x15
#define TW_ST_ARB_LOST_SLA_ACK		0x16
#define TW_ST_DATA_ACK				0x17
#define TW_ST_DATA_NACK				0x18
#define TW_ST_LAST_DATA				0x19
// Misc
#define TW_NO_INFO					0x1F
#define TW_BUS_ERROR				0x00

// defines and constants
#define TWCR_CMD_MASK		0x0F
#define TWSR_STATUS_MASK	0xF8

#endif	//ZO_I2C_H
