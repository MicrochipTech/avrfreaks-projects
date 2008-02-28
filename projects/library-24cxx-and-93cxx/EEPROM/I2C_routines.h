// Written by Chris Keeser
// copyright Chris Keeser 6/11/2005
// I2C_routines.h
// revision: 6/21/05 added ability to use different
// ports in i2c routines
// revision: 7/16/05 added conditional compilation
// to enable clock stretching

// only operates as an I2C master

// include file guard code
#ifndef I2C_ROUTINES
#define I2C_ROUTINES

#include <avr/io.h>
#include <inttypes.h>

// uncomment to enable clock stretching
//#define CLOCK_STRETCHING

// define the Clock pin information here
#define i2cPORTC PORTD
#define i2cDDRC DDRD
#define i2cPINC PIND
#define i2cSCK 0

// define the Data pin information here
#define i2cPORTD PORTD
#define i2cDDRD DDRD
#define i2cPIND PIND
#define i2cSDA 1


// definition of a boolean value

#define fTrue 255
#define fFalse 0

void i2cInit(void);  // initializes the I2C hardware
void start(void);		// generates the start condition
void sendByte(uint8_t);	// transmits a byte
unsigned char receiveAck(void);	// recives the acknowledge
void stop(void);	// generates the stop condition

void restart(void);	// generates the restart condition
uint8_t receiveByte(void);	// recieves a byte
void sendAck(void);	// sends the acknowledge
void sendNack(void);	// sends the No Acknowledge

#endif
