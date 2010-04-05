/*! \file i2c.c \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "zoI2c.h"
#include "zoMcu.h"
#include "zoSystemTimer.h"

ZO_ERROR zoI2cError = ZO_ERROR_DEFAULTS;

// types
typedef enum
{
	I2C_IDLE = 0, 
	I2C_BUSY,
	I2C_MASTER_TX,
	I2C_MASTER_RX,
	I2C_SLAVE_TX, 
	I2C_SLAVE_RX
} ZO_I2C_STATE;

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

// I2C state and address variables
static volatile ZO_I2C_STATE I2cState;
static u08 I2cDeviceAddrRW;
// send/transmit buffer (outgoing data)
static u08 I2cSendData[ZO_I2C_BUFFER_SIZE];
static u08 I2cSendDataIndex;
static u08 I2cSendDataLength;
// receive buffer (incoming data)
static u08 I2cReceiveData[ZO_I2C_BUFFER_SIZE];
static u08 I2cReceiveDataIndex;
static u08 I2cReceiveDataLength;

//timeout values
static u16 BusReadyTimeOut = ZO_I2C_DEFAULT_WAIT_ON_BUS_READY_TIMEOUT_MILISECONDS;
static u16 TransmitTimeOut = ZO_I2C_DEFAULT_WAIT_ON_TRANSMIT_TIMEOUT_MILISECONDS;
static u16 ReceiveTimeOut = ZO_I2C_DEFAULT_WAIT_ON_RECEIVE_TIMEOUT_MILISECONDS;

// function pointer to i2c receive routine
//! I2cSlaveReceive is called when this processor
// is addressed as a slave for writing
static void (*zoI2cSlaveReceive)(u08 receiveDataLength, u08* receiveData);
//! I2cSlaveTransmit is called when this processor
// is addressed as a slave for reading
static u08 (*zoI2cSlaveTransmit)(u08 transmitDataLengthMax, u08* transmitData);

//ISR functor implementation types and routines
typedef void (*ZO_I2C_FUNCTOR)(void);
static ZO_I2C_FUNCTOR zoI2cFunctor[32];
void zoI2cFunctorInit(void);

// functions
void zoI2cInit(void)
{
	// set pull-up resistors on I2C bus pins
	// TODO: should #ifdef these to support other processors
	PORTC |= _BV(4);	// i2c SCL on ATmega8,48,88,168
	PORTC |= _BV(5);	// i2c SDA on ATmega8,48,88,168

	// clear SlaveReceive and SlaveTransmit handler to null
	zoI2cSlaveReceive = 0;
	zoI2cSlaveTransmit = 0;

	//initialize functors for isr
	zoI2cFunctorInit();
	
	//initialize error buffer
	zoErrorInit(&zoI2cError);

	//initialize system timer
	zoSystemTimerInit();
		
	// set i2c bit rate to 100KHz
	zoI2cSetBitrate(ZO_I2C_DEFAULT_BITRATE_KHZ);
	// enable TWI (two-wire interface)
	TWCR |= _BV(TWEN);
	// set state
	I2cState = I2C_IDLE;
	// enable TWI interrupt and slave address ACK
	TWCR |= _BV(TWIE);
	TWCR |= _BV(TWEA);
	
	//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	// enable interrupts
	sei();
}

bool zoI2cSetBitrate(u16 bitrateKHz)
{
	u08 bitrate_div;
	
	if( (bitrateKHz > 400)||(bitrateKHz==0) )
		return FALSE;
	
	// calculate bitrate division
	bitrate_div = ((F_CPU/1000l)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div-16)/2;
	TWBR = bitrate_div;
	
	return TRUE;
}

inline bool zoI2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn)
{
	if((deviceAddr>0x7F)||(deviceAddr==0))
		return FALSE;
	
	TWAR = ((deviceAddr<<1) | (genCallEn?1:0));
	return TRUE;
}

inline void zoI2cSetWaitOnBusReadyTimeOut(u16 miliseconds)
{
	BusReadyTimeOut = miliseconds;
}

inline void zoI2cSetWaitOnTransmitTimeOut(u16 miliseconds)
{
	TransmitTimeOut = miliseconds;
}

inline void zoI2cSetWaitOnReceiveTimeOut(u16 miliseconds)
{
	ReceiveTimeOut = miliseconds;
}

inline void zoI2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(u08 receiveDataLength, u08* receiveData))
{
	zoI2cSlaveReceive = i2cSlaveRx_func;
}

inline void zoI2cSetSlaveTransmitHandler(u08 (*i2cSlaveTx_func)(u08 transmitDataLengthMax, u08* transmitData))
{
	zoI2cSlaveTransmit = i2cSlaveTx_func;
}

inline void zoI2cSendStart(void)
{
	// send start condition
	TWCR = (TWCR & TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWSTA);
}

inline void zoI2cSendStop(void)
{
	// transmit stop condition
	// leave with TWEA on for slave receiving
	TWCR = ( TWCR & TWCR_CMD_MASK ) | _BV(TWINT)|_BV(TWEA)|_BV(TWSTO);
}

inline void zoI2cWaitForComplete(void)
{
	// wait for i2c interface to complete operation
	while( !(TWCR & _BV(TWINT)) );
}

inline void zoI2cSendByte(u08 data)
{
	TWDR = data;
	TWCR = (TWCR & TWCR_CMD_MASK)|_BV(TWINT);	//send
}

inline void zoI2cAcknowledgeRx(bool acknowledge)
{
	if( acknowledge )
		TWCR = (TWCR & TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA);
	else
		TWCR = (TWCR & TWCR_CMD_MASK)|_BV(TWINT);
}

#if ZO_I2C_ENABLE_MASTER_TRANSMIT == 1
bool zoI2cMasterSend(u08 deviceAddr, u08 length, u08 *data)
{
	u08 i;
	u16 timer;
	// wait for interface to be ready
	
	zoSystemTimerTimeOutInit(&timer);
	while(I2cState)
	{
		if(zoSystemTimerTimeOutExpired(&timer,BusReadyTimeOut))
		{
			zoErrorPut(&zoI2cError,ZO_I2C_ERROR_WAIT_ON_BUS_READY_TIMEOUT);
			return FALSE;
		}
	}

	// set state
	I2cState = I2C_MASTER_TX;
	// save data
	I2cDeviceAddrRW = ((deviceAddr<<1) & 0xFE);	// RW cleared: write operation
	for(i=0; i<length; i++)
		I2cSendData[i] = *data++;
	I2cSendDataIndex = 0;
	I2cSendDataLength = length;
	// send start condition
	zoI2cSendStart();

	return TRUE;
}

bool zoI2cMasterSendBlocking(u08 deviceAddr, u08 length, u08 *data)
{
	u16 timer;

	if( !zoI2cMasterSend(deviceAddr,length,data) )
		return FALSE;
		
	//wait on transmission of data
	zoSystemTimerTimeOutInit(&timer);
	while(I2cState != I2C_IDLE)
	{
		if(zoSystemTimerTimeOutExpired(&timer,TransmitTimeOut))
		{
			zoErrorPut(&zoI2cError,ZO_I2C_ERROR_WAIT_ON_TRANSMIT_TIMEOUT);
			return FALSE;
		}
	}

	return(zoErrorIsEmpty(&zoI2cError));
}
#endif //ZO_I2C_ENABLE_MASTER_TRANSMIT

#if ZO_I2C_ENABLE_MASTER_RECEIVE==1
bool zoI2cMasterReceive(u08 deviceAddr, u08 length, u08* data)
{
	u08 i;
	u16 timer;

	// wait for interface to be ready
	zoSystemTimerTimeOutInit(&timer);
	while(I2cState)
	{
		if(zoSystemTimerTimeOutExpired(&timer,BusReadyTimeOut))
		{
			zoErrorPut(&zoI2cError,ZO_I2C_ERROR_WAIT_ON_BUS_READY_TIMEOUT);
			return FALSE;
		}
	}

	// set state
	I2cState = I2C_MASTER_RX;
	// save data
	I2cDeviceAddrRW = ((deviceAddr<<1)|0x01);	// RW set: read operation
	I2cReceiveDataIndex = 0;
	I2cReceiveDataLength = length;
	// send start condition
	zoI2cSendStart();
	
	// wait for data
	zoSystemTimerTimeOutInit(&timer);
	while(I2cState)
	{
		if(zoSystemTimerTimeOutExpired(&timer,ReceiveTimeOut))
		{
			zoErrorPut(&zoI2cError,ZO_I2C_ERROR_WAIT_ON_RECEIVE_TIMEOUT);
			return FALSE;
		}
	}

	// return data
	for(i=0; i<length; i++)
		*data++ = I2cReceiveData[i];

	return TRUE;
}
#endif //ZO_I2C_ENABLE_MASTER_RECEIVE


//! I2C (TWI) interrupt service routine
//SIGNAL(SIG_2WIRE_SERIAL)
ISR(TWI_vect)
{
	// read status bits	and shift right by 3. Execute corresponding functor
	zoI2cFunctor[( TWSR >> 3 )]();
}

// 0x00: Bus error due to illegal start or stop condition
void zoI2cFunctorBusError(void)
{
	// reset internal hardware and release bus
	TWCR =  ( TWCR & TWCR_CMD_MASK )|_BV(TWINT)|_BV(TWSTO)|_BV(TWEA);
	// set state
	zoErrorPut(&zoI2cError,ZO_I2C_ERROR_BUS_ERROR);
	I2cState = I2C_IDLE;
	
}

// 0x01, 0x02 : Start or repeated start
void zoI2cFunctorStart(void)		
{
	// send device address
	zoI2cSendByte(I2cDeviceAddrRW);	
}

//0x03: Slave address acknowledged, 0x05: Data acknowledged 
void zoI2cFunctorMtAck(void)
{
	if(I2cSendDataIndex < I2cSendDataLength)
		zoI2cSendByte( I2cSendData[I2cSendDataIndex++] );
	else
	{
		zoI2cSendStop();			// transmit stop condition, enable SLA ACK
		I2cState = I2C_IDLE;		// set state
	}
}

// 0x0B: Data received, NACK reply issued
void zoI2cFunctorMrDataNAck(void)
{
	I2cReceiveData[I2cReceiveDataIndex++] = TWDR;
	zoI2cSendStop();
	I2cState = I2C_IDLE;
}

// 0x09, 0x04: Slave address not acknowledged, 0x06: Data not acknowledged
void zoI2cFunctorMasterNAck(void)
{
	zoI2cSendStop();
	zoErrorPut(&zoI2cError,ZO_I2C_ERROR_NODEV);
	I2cState = I2C_IDLE;
}

//0x07: Bus arbitration lost
void zoI2cFunctorArbitrationLost(void)
{
	// release bus
	TWCR = (TWCR&TWCR_CMD_MASK)|_BV(TWINT);
	// set state
	zoErrorPut(&zoI2cError,ZO_I2C_ERROR_ARBITRATION_LOST);
	I2cState = I2C_IDLE;
}

//0x0A: Data acknowledged
void zoI2cFunctorMrDataAck(void)
{
	// store received data byte
	I2cReceiveData[I2cReceiveDataIndex++] = TWDR;

	if(I2cReceiveDataIndex < (I2cReceiveDataLength-1))
		// data byte will be received, reply with ACK (more bytes in transfer)
		zoI2cAcknowledgeRx(TRUE);
	else
		// data byte will be received, reply with NACK (final byte in transfer)
		zoI2cAcknowledgeRx(FALSE);
}

//0x08: Slave address acknowledged
void zoI2cFunctorMrSlaveAck(void)
{
	if(I2cReceiveDataIndex < (I2cReceiveDataLength-1))
		// data byte will be received, reply with ACK (more bytes in transfer)
		zoI2cAcknowledgeRx(TRUE);
	else
		// data byte will be received, reply with NACK (final byte in transfer)
		zoI2cAcknowledgeRx(FALSE);
}

// 0x0C, 0x0D: own SLA+W has been received, ACK has been returned
// 0x0E, 0x0F: GCA+W has been received, ACK has been returned
void zoI2cFunctorSrAck(void)
{
	// we are being addressed as slave for writing (data will be received from master)
	// set state
	I2cState = I2C_SLAVE_RX;
	// prepare buffer
	I2cReceiveDataIndex = 0;
	// receive data byte and return ACK
	TWCR = (TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA);
}

// 0x10, 0x12: data byte has been received, ACK has been returned 
void zoI2cFunctorSrDataAck(void)
{
	// get previously received data byte
	I2cReceiveData[I2cReceiveDataIndex++] = TWDR;
	// check receive buffer status
	if(I2cReceiveDataIndex < ZO_I2C_BUFFER_SIZE)
		// receive data byte and return ACK
		zoI2cAcknowledgeRx(TRUE);
	else
		// receive data byte and return NACK
		zoI2cAcknowledgeRx(FALSE);
}

//0x11, 0x13: data byte has been received, NACK has been returned 
void zoI2cFunctorSrDataNack(void)
{
	// receive data byte and return NACK
	zoI2cAcknowledgeRx(FALSE);
}

//0x14: STOP or REPEATED START has been received while addressed as slave
void zoI2cFunctorStop(void)
{
	// switch to SR mode with SLA ACK
	TWCR = (TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA);
	// i2c receive is complete, call i2cSlaveReceive
	if(zoI2cSlaveReceive) zoI2cSlaveReceive(I2cReceiveDataIndex, I2cReceiveData);
	// set state
	I2cState = I2C_IDLE;
}

//0x15: own SLA+R has been received, ACK has been returned
//0x16: GCA+R has been received, ACK has been returned 
void zoI2cFunctorStAck(void)
{
	// we are being addressed as slave for reading (data must be transmitted back to master)
	// set state
	I2cState = I2C_SLAVE_TX;
	// request data from application
	if(zoI2cSlaveTransmit) I2cSendDataLength = zoI2cSlaveTransmit(ZO_I2C_BUFFER_SIZE, I2cSendData);
	// reset data index
	I2cSendDataIndex = 0;
	// transmit data byte
	TWDR = I2cSendData[I2cSendDataIndex++];
	if(I2cSendDataIndex < I2cSendDataLength)
		// expect ACK to data byte
		TWCR = (TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA);
	else
		// expect NACK to data byte
		TWCR = (TWCR&TWCR_CMD_MASK)|_BV(TWINT);
}

//0x17: data byte has been transmitted, ACK has been received
void zoI2cFunctorStDataAck(void)
{
	// transmit data byte
	TWDR = I2cSendData[I2cSendDataIndex++];
	if(I2cSendDataIndex < I2cSendDataLength)
		// expect ACK to data byte
		TWCR = (TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA);
	else
		// expect NACK to data byte
		TWCR = (TWCR&TWCR_CMD_MASK)|_BV(TWINT);
}

// 0x18, 0x19: data byte has been transmitted, NACK has been received 
void zoI2cFunctorStDataNAck(void)
{
	// switch to open slave
	TWCR = (TWCR&TWCR_CMD_MASK)|_BV(TWINT)|_BV(TWEA);
	// set state
	I2cState = I2C_IDLE;
}

//0x1F: No relevant state information
void zoI2cFunctorNoInfo(void)
{
	zoErrorPut(&zoI2cError,ZO_I2C_ERROR_INDETERMINATE);
}

//Null functor: all other indexes
void zoI2cFunctorNull(void) {}

void zoI2cFunctorInit(void)
{
	zoI2cFunctor[TW_BUS_ERROR]				= zoI2cFunctorBusError;
	zoI2cFunctor[TW_START]					= zoI2cFunctorStart;
	zoI2cFunctor[TW_REP_START]				= zoI2cFunctorStart;
	zoI2cFunctor[TW_MT_SLA_ACK]				= zoI2cFunctorMtAck;
	zoI2cFunctor[TW_MT_SLA_NACK]			= zoI2cFunctorMasterNAck;
	zoI2cFunctor[TW_MT_DATA_ACK]			= zoI2cFunctorMtAck;
	zoI2cFunctor[TW_MT_DATA_NACK]			= zoI2cFunctorMasterNAck;
	zoI2cFunctor[TW_MT_ARB_LOST]			= zoI2cFunctorArbitrationLost;
	zoI2cFunctor[TW_MR_SLA_ACK]				= zoI2cFunctorMrSlaveAck;
	zoI2cFunctor[TW_MR_SLA_NACK]			= zoI2cFunctorMasterNAck;
	zoI2cFunctor[TW_MR_DATA_ACK]			= zoI2cFunctorMrDataAck;
	zoI2cFunctor[TW_MR_DATA_NACK]			= zoI2cFunctorMrDataNAck;
	zoI2cFunctor[TW_SR_SLA_ACK]				= zoI2cFunctorSrAck;
	zoI2cFunctor[TW_SR_ARB_LOST_SLA_ACK]	= zoI2cFunctorSrAck;
	zoI2cFunctor[TW_SR_GCALL_ACK]			= zoI2cFunctorSrAck;
	zoI2cFunctor[TW_SR_ARB_LOST_GCALL_ACK]	= zoI2cFunctorSrAck;
	zoI2cFunctor[TW_SR_DATA_ACK]			= zoI2cFunctorSrDataAck;
	zoI2cFunctor[TW_SR_DATA_NACK]			= zoI2cFunctorSrDataNack;
	zoI2cFunctor[TW_SR_GCALL_DATA_ACK]		= zoI2cFunctorSrDataAck;
	zoI2cFunctor[TW_SR_GCALL_DATA_NACK]		= zoI2cFunctorSrDataNack;
	zoI2cFunctor[TW_SR_STOP]				= zoI2cFunctorStop;
	zoI2cFunctor[TW_ST_SLA_ACK]				= zoI2cFunctorStAck;
	zoI2cFunctor[TW_ST_ARB_LOST_SLA_ACK]	= zoI2cFunctorStAck;
	zoI2cFunctor[TW_ST_DATA_ACK]			= zoI2cFunctorStDataAck;
	zoI2cFunctor[TW_ST_DATA_NACK]			= zoI2cFunctorStDataNAck;
	zoI2cFunctor[TW_ST_LAST_DATA]			= zoI2cFunctorStDataNAck;
	zoI2cFunctor[0x1A]						= zoI2cFunctorNull;
	zoI2cFunctor[0x1B]						= zoI2cFunctorNull;
	zoI2cFunctor[0x1C]						= zoI2cFunctorNull;
	zoI2cFunctor[0x1D]						= zoI2cFunctorNull;
	zoI2cFunctor[0x1E]						= zoI2cFunctorNull;
	zoI2cFunctor[TW_NO_INFO]				= zoI2cFunctorNoInfo;
}

