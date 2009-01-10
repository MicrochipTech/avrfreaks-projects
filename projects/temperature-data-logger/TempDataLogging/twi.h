//**********************************************************************//
//FILE: twi.h 
//AUTHOR: Adam Kadolph
//DATE:	10-6-08
//DESCRIPTION: I2C interface using AVR Two-Wire Interface (TWI) hardware
//**********************************************************************//

#ifndef _TWI_H_
#define _TWI_H_

// TWSR values
// Master
#define START				0x08
#define RESTART				0x10

// Master Transmitter
#define MT_SLA_ACK			0x18
#define MT_SLA_NACK			0x20
#define MT_DATA_ACK			0x28
#define MT_DATA_NACK			0x30
#define MT_ARB_LOST			0x38

// Master Receiver
#define MR_ARB_LOST			0x38
#define MR_SLA_ACK			0x40
#define MR_SLA_NACK			0x48
#define MR_DATA_ACK			0x50
#define MR_DATA_NACK			0x58

// Slave Transmitter
#define ST_SLA_ACK			0xA8
#define ST_ARB_LOST_SLA_ACK		0xB0
#define ST_DATA_ACK			0xB8
#define ST_DATA_NACK			0xC0
#define ST_LAST_DATA			0xC8

// Slave Receiver
#define SR_SLA_ACK			0x60
#define SR_ARB_LOST_SLA_ACK		0x68
#define SR_GCALL_ACK			0x70
#define SR_ARB_LOST_GCALL_ACK		0x78
#define SR_DATA_ACK			0x80
#define SR_DATA_NACK			0x88
#define SR_GCALL_DATA_ACK		0x90
#define SR_GCALL_DATA_NACK		0x98
#define SR_STOP				0xA0

// Misc
#define NO_INFO				0xF8
#define BUS_ERROR			0x00

// defines and constants
#define CMD_MASK			0x0F
#define STATUS_MASK			0xF8

// return values
#define I2C_OK				0x00
#define I2C_ERROR_NODEV		0x01

#define  ERROR_CODE			0x7E

//TWI internal functions to expand TWI/I2C for other devices like EEPROM
void I2Cinit(void);
unsigned char I2C_start(void);
unsigned char I2C_repeatStart(void);
unsigned char I2C_sendAddr(unsigned char addr);
unsigned char I2C_sendData(unsigned char data);
unsigned char I2C_receiveData_ACK(void);
unsigned char I2C_receiveData_NACK(void);
void I2C_stop(void);

#endif
