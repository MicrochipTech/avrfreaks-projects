//**************************************************************************************
///	File Name	: \file zoI2c.h 
///	Description	: \brief I2C/Two wire function library.
//	Created		: 21/01/2010
//	Target MCU	: Mega 48/88/168/328
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation, 
//  version 3 of the License or any later version. This program is distributed in the hope
//  that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
//  License for more details: <http://www.gnu.org/licenses/>
//  
/// \ingroup driver_avr 
/// \defgroup zoI2c zoI2c.h : I2C function library.
/// 
/// \par Overview
/// This is a library that provides high level functionality for the AVR I2C peripheral
/// (or Two wire according to Atmel datasheets). Special thanks to Pascal Stang
/// for implementing a great deal of this code and to Marek Michalkiewicz for providing
/// the I2C state defines.
///
/// \par Usage example: I2C slave operation 
/// \code
/// #include "zoI2c.h"
/// #include "zoTypes.h"
/// #include "zoString.h"
///
/// u08 MyString[13] = "Hello World!";			//12 characters and a null
/// u08 MyIncomingBuffer[ZO_I2C_BUFFER_SIZE];	//storage for incoming I2C messages
/// //the above global buffer should be protected from ISRs in code - see zoMcu.h
///
/// //user function to be called when the MCU is addressed as slave for reception
/// void mySlaveReceiveFunction(u08 receiveDataLength, u08* receiveData);
/// //user function to be called when the MCU is addressed as slave for transmission
/// u08 mySlaveTransmitFunction(u08 transmitDataLengthMax, u08* transmitData);
/// 
/// int main(void)
/// {
/// 	zoI2cInit();						//initialize I2c to 100KHz bitrate
/// 	zoI2cSetLocalDeviceAddr(0x04,1);	//set the MCU slave address to 0x04 and
/// 										//configure acceptance of I2C broadcast msgs.
///
/// 	//initialize the slave receive handler to the user-provided function
/// 	zoI2cSetSlaveReceiveHandler(mySlaveReceiveFunction);
/// 	//initialize the slave transmit handler to the user-provided function
/// 	zoI2cSetSlaveTransmitHandler(mySlaveTransmitFunction);
///
/// 	while(1);	//wait until we are addressed as I2C slaves.
/// }
///
/// // Both user slave functions are executed inside the I2C ISR. There is no need to 
/// // protect global variables here.( as AVR ISRs do not nest by default )
///
/// void mySlaveReceiveFunction(u08 receiveDataLength, u08* receiveData)
/// {
/// 	//we received some data from an I2C master!
/// 	u08 i;
/// 	
/// 	for(i=0;i<receiveDataLength;i++)			//store them for now...
/// 		MyIncomingBuffer[i] = receiveData[i];
/// }
///
/// u08 mySlaveTransmitFunction(u08 transmitDataLengthMax, u08* transmitData)
/// {
/// 	//now that we are addressed by an I2C master as slaves we will say to him
/// 	//"Hello world!". Of course the master can stop our transmission at will...
/// 	//All depend on how many bytes the master asked for.
///
/// 	u08 transmitLength;								//how many bytes to transmit
/// 	transmitData = MyString;						//assign the byte array to send
/// 	
/// 	//check to see if the I2C buffer is big enough
/// 	if( 12 > transmitDataLengthMax) 				//if it isn't send as many bytes 
/// 		transmitLength = transmitDataLengthMax;		//as possible
/// 	else											//if it is: send 12 bytes
/// 		transmitLength = 12;						//we will not send the terminating
/// } 													//null :)
/// \endcode 
//****************************************************************************************
//@{

#ifndef ZO_I2C_H
#define ZO_I2C_H

#include "zoTypes.h"
#include "zoError.h"

//Conditional compilation defines_________________________________________________________
///Define for conditional compilation of the I2C library code. Enable master transmit
///functionality by assigning a 1 to this define. Disable (and do not compile at all)
///master transmit functionality by assigning a zero to this define.
#define ZO_I2C_ENABLE_MASTER_TRANSMIT			1
///Define for conditional compilation of the I2C library code. Enable master receive
///functionality by assigning a 1 to this define. Disable (and do not compile at all)
///master receive functionality by assigning a zero to this define.
#define ZO_I2C_ENABLE_MASTER_RECEIVE			1

//Errors__________________________________________________________________________________
#define ZO_I2C_ERROR_NONE						0x00

///This error indicates that the I2C slave address used in the last communication does not 
///exist. To resolve this error: Double check the slave settings, check cabling.
#define ZO_I2C_ERROR_NODEV						0x01

///This error indicates that the I2C bus is in an error state. To resolve this error: 
///Double check settings of all I2C devices on the bus, check I2C pull-ups, check cabling.
#define ZO_I2C_ERROR_BUS_ERROR					0x02

///This error indicates that we issued a master command on the I2C bus simultaneously with
///another bus master, which won the I2C arbitration. This means that the communication
///transaction we issued did not appear on the I2C bus. If you get this error write user
///code that attempts the last I2C transaction again.
#define ZO_I2C_ERROR_ARBITRATION_LOST			0x03

///This error indicates that I2C got in an indeterminate state. Ignore this for the time
///being. \todo identify when and why this error happens and document it.
#define ZO_I2C_ERROR_INDETERMINATE				0x04

///The MCU waited too long for a space on the I2C bus. ie The voltage on SDA and SCL is 
///almost constantly at zero Volts. To resolve this error: 1)check the I2C pull-up 
///resistors 2)check cabling 3)check for defective devices on the I2C bus 4)check power
#define ZO_I2C_ERROR_WAIT_ON_BUS_READY_TIMEOUT	0x05

///The zoI2cMasterSendBlocking() routine waited too long for the I2C ISR to finish sending
///data over I2C. To resolve this error: 1)double check settings and initialization 2)
///follow the procedure for resolving the ZO_I2C_ERROR_WAIT_ON_BUS_READY_TIMEOUT error.
#define ZO_I2C_ERROR_WAIT_ON_TRANSMIT_TIMEOUT	0x06


///The zoI2cMasterReceive() routine waited too long for the slave addressed to respond.
///To resolve this error follow the ZO_I2C_ERROR_WAIT_ON_BUS_READY_TIMEOUT error resolving
///strategy.
#define ZO_I2C_ERROR_WAIT_ON_RECEIVE_TIMEOUT	0x07

//Defaults________________________________________________________________________________
#define ZO_I2C_DEFAULT_BITRATE_KHZ								100
#define ZO_I2C_DEFAULT_WAIT_ON_BUS_READY_TIMEOUT_MILISECONDS	1000
#define ZO_I2C_DEFAULT_WAIT_ON_TRANSMIT_TIMEOUT_MILISECONDS		1000
#define ZO_I2C_DEFAULT_WAIT_ON_RECEIVE_TIMEOUT_MILISECONDS		1000

///The size of the I2C buffers. Change this define according to the largest byte array
///that is expected to be communicated over I2C.
///\todo Replace fixed size buffers.
#define ZO_I2C_BUFFER_SIZE										32

///The I2C error buffer
extern ZO_ERROR zoI2cError;

/*! \brief Initializes the I2C AVR peripheral. */
void zoI2cInit(void);

/*! \brief Sets the I2C bitrate.
	\param bitrateKHz the bitrate in KHz. Default value is 100
	\return Wether the bitrate given is valid or not. */
bool zoI2cSetBitrate(u16 bitrateKHz);

/*! \brief Sets the local slave device address. A master uses this address to communicate
	to this MCU.
	\param deviceAddr The desired slave address for this MCU.
	\param genCallEn Pass 0 to disable reception of I2C broadcast messages (to slave ID 
	0x00). Write 1 to enable.
	\return Wether the slave address given is valid or not. */
bool zoI2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn);

/*! \brief Sets the time the MCU waits for SDA to go high. If this time is exceeded during
	a master I2C transaction an error is generated.
	\param miliseconds The timeout in miliseconds. Default value 1 sec */
void zoI2cSetWaitOnBusReadyTimeOut(u16 miliseconds);

/*! \brief Sets the time the MCU waits for a master transmission to end.
	\param miliseconds The timeout in miliseconds. Default value 1 sec */
void zoI2cSetWaitOnTransmitTimeOut(u16 miliseconds);

/*! \brief Sets the time the MCU waits for a slave to respond to a master receive I2C 
	transaction.
	\param miliseconds The timeout in miliseconds. Default value 1 sec */
void zoI2cSetWaitOnReceiveTimeOut(u16 miliseconds);

/*! \brief Function to easily assign a user function to the slave receive ISR function 
	handler.
	\param i2cSlaveRx_func The user function (just pass the name of the function) */
void zoI2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(u08 receiveDataLength, u08* receiveData));

/*! \brief Function to easily assign a user function to the slave transmit ISR function 
	handler.
	\param i2cSlaveTx_func The user function (just pass the name of the function) */
void zoI2cSetSlaveTransmitHandler(u08 (*i2cSlaveTx_func)(u08 transmitDataLengthMax, u08* transmitData));

// high-level I2C transaction commands____________________________________________________
/*! \brief Send a byte array over I2C as master.
	\param deviceAddr The slave device addressed.
	\param length How many bytes to send over I2C.
	\param *data Pointer to the byte array to be sent.
	\return Wether the MCU managed to issue the master send transaction.
	This does not necessarily mean that the slave received the command.*/
bool zoI2cMasterSend(u08 deviceAddr, u08 length, u08 *data);

/*! \brief Send a byte array over I2C as master AND wait until the transaction has 
	finished.
	\param deviceAddr The slave device addressed.
	\param length How many bytes to send over I2C.
	\param *data Pointer to the byte array to be sent.
	\return Wether the data were actually sent with no errors.*/
bool zoI2cMasterSendBlocking(u08 deviceAddr, u08 length, u08 *data);

/*! \brief Request data from an I2C slave.
	\param deviceAddr The slave device addressed.
	\param length How many bytes to receive over I2C.
	\param *data Pointer to the byte array where the incoming data will be stored.
	\return Wether the data were actually received with no errors.*/
bool zoI2cMasterReceive(u08 deviceAddr, u08 length, u08* data);

// Low-level I2C transaction commands_____________________________________________________ 
///Send a start on the I2C bus
void zoI2cSendStart(void);

///Send a stop on the I2C bus
void zoI2cSendStop(void);

///Wait for an I2C transaction to finish
void zoI2cWaitForComplete(void);

///Send a byte over I2C
void zoI2cSendByte(u08 data);

///Acknowledge a received byte
void zoI2cAcknowledgeRx(bool acknowledge);

#endif	//ZO_I2C_H
//@}
