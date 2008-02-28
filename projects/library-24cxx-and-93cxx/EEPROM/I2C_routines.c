// Written by Chris Keeser
// copyright Chris Keeser 6/11/2005
// I2C_routines.c
// revision: 6/21/05 added ability to use different
// ports in i2c routines
// revision: 7/16/05 added conditional compilation
// to enable clock stretching


// only operates as an I2C master

#include "I2C_routines.h"
#include "global.h"
/*
defined in I2C_routines.h:

i2cPORTC
i2cDDRC
i2cPINC

i2cPORTD
i2cDDRD
i2cPIND

i2cSDA
i2cSCK
*/


/* i2cInit

entering:

leaving:
sck will be output high
sda will be input high
*/


#define	delay		_delay_us(25)
void i2cInit(void)
{
	// make sda pin an input to allow it to be pulled high
	i2cDDRD &= ~( 1 << i2cSDA );
	i2cPORTD |= ( 1 << i2cSDA );
	
	// make sck pin high and output
	i2cPORTC |= ( 1 << i2cSCK );
	i2cDDRC |= ( 1 << i2cSCK );
	

	return;
}

/* START
i2c start condition generator

entering:
expects the sda pin to be an input and high
expects the sck line to be output high

leaving:
sck will be output low
sda will be output low
*/

void start(void)
{	
	// sda value written as 0
	i2cPORTD &= ~( 1 << i2cSDA );
	
	// sda pin made an output
	i2cDDRD |= ( 1 << i2cSDA );
	delay;
	// clock low

	// sck driven low
	i2cPORTC &= ~( 1 << i2cSCK );
	delay;
	return;
}

/* sendByte
shifts out a byte, msb first
data is latched in on the rising edge
and must be stable through the entire
high clock pulse

entering:
expects the sck line to be output low

leaving:
sck will be output low
sda will be unknown
*/

void sendByte(uint8_t data)
{
	uint8_t	i;
	
	for(i = 0 ; i < 8 ; i++)
	{
		if( ( data << i & 0b10000000 ) != 0 )
		{
			// make sda pin an input to allow it to be pulled high
			i2cDDRD &= ~( 1 << i2cSDA );
			i2cPORTD |= ( 1 << i2cSDA );
			delay;
			// clock high

			// bring the clock high
			i2cPORTC |= ( 1 << i2cSCK );
		}
		else
		{
			// sda value written as 0
			i2cPORTD &= ~( 1 << i2cSDA );
			// sda pin made an output
			i2cDDRD |= ( 1 << i2cSDA );
			delay;
			// clock high

			// bring the clock high
			i2cPORTC |= ( 1 << i2cSCK );

		}
		delay;
		// clock low

		// sck driven low
		i2cPORTC &= ~( 1 << i2cSCK );
		delay;
	}
	
	return;
}

/* receiveAck
will generate the next clock and receive
the acknowledge bit, It will return fFalse
if no ack is recieved, fTrue is it does
recieve an ACK
entering:
expects sck to be output low

leaving:
sck will be output low
sda will be an input

*/

unsigned char receiveAck(void)
{
	// make the sda an input
	i2cDDRD &= ~( 1 << i2cSDA );
	i2cPORTD |= ( 1 << i2cSDA );
	delay;
	// clock high

	// bring the clock high
	i2cPORTC |= ( 1 << i2cSCK );
	delay;
	// read the acknowledge bit
	if( ( i2cPIND & ( 1 << i2cSDA ) ) == 0 )
	{
		// clock low

		// sck driven low
		i2cPORTC &= ~( 1 << i2cSCK );

		// return a "true"
		return fTrue;
	}
	else
	{
	delay;
		// clock low

		// sck driven low
		i2cPORTC &= ~( 1 << i2cSCK );
	// return a "false"
		return fFalse;
	}
}

/* STOP
i2c STOP condition generator

entering:
expects the sda pin to be an input
expects the sck line to be output low

leaving:
sck will be input
sda will be output high
*/

void stop(void)
{
	// make sda output low
	i2cPORTD &= ~( 1 << i2cSDA );
	i2cDDRD |= ( 1 << i2cSDA );
	delay;
	// clock high

	// bring the clock high
	i2cPORTC |= ( 1 << i2cSCK );
	delay;
	// bring SDA high
	i2cDDRD &= ~( 1 << i2cSDA );
	i2cPORTD |= ( 1 << i2cSDA );
	
	return;
}

// restart
// expecting sck output low
// ends with sda and sck output low
void restart(void)
{
	// bring sda high
	i2cDDRD &= ~( 1 << i2cSDA );
	i2cPORTD |= ( 1 << i2cSDA );
	delay;
	// clock high

	// bring the clock high
	i2cPORTC |= ( 1 << i2cSCK );
	delay;
	//bring SDA low
	i2cPORTD &= ~( 1 << i2cSDA );
	i2cDDRD |= ( 1 << i2cSDA );
	
	// clock low
	delay;
	// sck driven low
	i2cPORTC &= ~( 1 << i2cSCK );
	return;
}

// receive byte
// expects sck output low
// exits with sck low, and sda an input

uint8_t receiveByte(void)
{
	uint8_t i, received = 0;
	
	// make SDA an input
	i2cDDRD &= ~( 1 << i2cSDA );
	i2cPORTD |= ( 1 << i2cSDA );
	delay;
	for( i =0 ; i < 8 ; i++ )
	{
	delay;
		// clock high

		// bring the clock high
		i2cPORTC |= ( 1 << i2cSCK );

		delay;
		if( ( i2cPIND & 1 << i2cSDA ) != 0 )
		{
			received |= 1 << ( 7 - i );
		}
		
		// clock low
		delay;
		// sck driven low
		i2cPORTC &= ~( 1 << i2cSCK );
	}
	
	return received;
}

// generate ack
// enter:  clock low sda input
// exit : clock low, sda output low

void sendAck(void)
{
	//bring SDA low
	i2cPORTD &= ~( 1 << i2cSDA );
	i2cDDRD |= ( 1 << i2cSDA );
	delay;
	// clock high

	// bring the clock high
	i2cPORTC |= ( 1 << i2cSCK );
	delay;
	// clock low

	// sck driven low
	i2cPORTC &= ~( 1 << i2cSCK );
	return;
}

// generate NACK
// enter:  clock low sda input
// exit : clock low, sda input high

void sendNack(void)
{
	// make SDA an input
	i2cDDRD &= ~( 1 << i2cSDA );
	i2cPORTD |= ( 1 << i2cSDA );
	delay;
	// clock high

	// bring the clock high
	i2cPORTC |= ( 1 << i2cSCK );


	delay;
	// clock low

	// sck driven low
	i2cPORTC &= ~( 1 << i2cSCK );
	return;
}
