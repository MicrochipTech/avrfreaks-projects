/*
 *  Module:    1-wire_bus.c
 *  Title:     Low Level Dallas 1-Wire hardware interface
 *  Author:    Anthony Rhodes
 *  Tools:     avr-gcc 3.4.5 / avr-libc
 *  Hardware:  om128
 *
 *  Module Description
 *  ------------------
 * 
 *  The module contains the low level interface software to access
 *  Dallas 1-Wire devices.  OWReset, OWWriteBit, and OWReadBit are the
 *  primary interface routines.  The rest are derived.
 *
 *  The timing of delays is critical.  While the Dallas references vary
 *  somewhat on the timing specifications, these were followed as per
 *  Dallas DS18S20 temperature sensor datasheet
 *
 *  Direct access to ports is utilized instead of the generic PutPut()
 *  function beause speed is critical.
 *
 *  The delays are likely to need to be modified to suit different processoe speeds
 *
 *
 *	Acknowlegements
 *  ---------------
 *
 * 	Various Dallas Semiconductor/Maxim references on 1-Wire interfacing were used,
 *  Most useful was the DS18S20 datasheet, followed by Dallas AN-216
 *
 *
 *  Change History
 *  --------------
 * 
 *  1.0  Nov 21, 2007
 *
 ****************************************************************************
 */

/* ---- Standard header files --------------------------------------------- */
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>


/* ---- Custom header files ----------------------------------------------- */
#include "pin_numbers.h"


/* ---- Private Constants and Types --------------------------------------- */




/* ---- Private Variables ------------------------------------------------- */
/* ---- Private Function Prototypes --------------------------------------- */
/* ---- Interrupt Routines ------------------------------------------------ */
/* ---- Private Functions ------------------------------------------------- */
/* ---- Public Functions -------------------------------------------------- */



//***************************************************************************
//*                                                                         *
//*    One Wire Reset:  Resets all the 1-wire devices on a specific         *
//*    bus (processor pin).                                                 *
//*                                                                         *
//*    Timing is:  Pull Low for 480us, Release the bus, Delay for 70us      *
//*    Read the bus, Delay 410us, Read the bus again.                       *
//*                                                                         *
//*                                                                         *
//*    1-Wire devices will wait 15-60us after the rising edge of the Low    *
//*    signal, then will transmit the Low "presence" pulse for 60-240us.    *
//*    This wide range of waits and pulse widths means that the guaranteed  *
//*    window for valid data is only 61-75us after the rising edge          * 
//*    The presence pulse should be completed by 300us after the            *
//*    rising edge.                                                         *
//*                                                                         *
//*    Bus should be pulled LOW by 1-Wire devices on the first read,        *
//*    and should be released to HIGH on the second read.  If it is high    *
//*    on both reads, then there are no 1-wire devices present.  If         *
//*    it is LOW on both reads then it is shorted.                          *
//*                                                                         *
//*    RETURNS:                                                             *
//*             ZERO if a 1-Wire device is detected                         *
//*             1    if no 1-Wire device is detected                        *
//*             2    if the bus is shorted                                  *
//*                                                                         *
//***************************************************************************

unsigned char OWReset(unsigned int OWPin)
{
	#define RESET_PULSE		0.485	//0.485ms = 485uS
	#define READ_DELAY		0.066	// plus about 2us overhead =0.068ms = 68us
	#define END_RX_WINDOW	0.417	//0.485 - READ_DELAY

	PutPin(OWPin,OutputLow);
		_delay_ms(RESET_PULSE);
	PutPin(OWPin, InputTristate);
	_delay_ms(READ_DELAY);
	if (~GetPin(OWPin))	//pin should be low if there is a ds present (or short circuit)
	{
		_delay_ms(END_RX_WINDOW);	//at the end of the 480us Rx window
		if (GetPin(OWPin)) return(0);	//high = DS present
		else return(2);					//low = Bus Shorted
	}
	else  //no presence pulse detected, and bus is correctly pulled up
	{
		_delay_ms(END_RX_WINDOW);	//at the end of the 480us Rx window
		return(1);
	}

}





//***************************************************************************
//*                                                                         *
//*    One Wire Write Bit:  Transmits a bit over the 1-wire bus as          *   
//*    specified by the bus (processor pin).                                *
//*                                                                         *
//*    Timing is:  Pull Low for more than 1us.  (sources vary on the        *
//*    duration.  Some say as short as possible, but more than 1us          *
//*    others suggest at least 5us.  This uses 8us).  Following this,       *
//*    either release the bus to transmit a ZERO or keep it high to  t      *
//*    transmit a ONE.  The window for the bit ends at 60us and there       *
//*    should be a 10us (other sources say 1+us) recovery time.             *
//*                                                                         *
//*    The 1-Wire devices will read the bit 15-60us (typ. 30us) after the   *
//*    falling edge at the start of the transmission.  The bus must         *
//*    be released by 15us after the falling edge for a ZEro bit to be      *
//*    reliably registered by the 1-Wire devices                            *
//*                                                                         *
//*    RETURNS: VOID                                                        *
//*                                                                         *
//***************************************************************************


void OWWriteBit(unsigned int OWPin, unsigned char BitVal)
{
	cli();							//can not allow interrupts:timing is critical
	if(BitVal)						//write a "1"
	{

		PutPin(OWPin,OutputLow);	//takes about 43 clocks (2.03us @14.7MHz)

									//this delay uses ms because it may be more accurate than us due to
									//the floating math used to calc the result
									//hand tuned to give  8us at 14.7MHz.
		_delay_ms(0.0052);			//needs total of less than 15us, DS samples soon after 15us: use 8

		PutPin(OWPin,InputTristate);//let DS sample High value
		_delay_ms(0.057 + 0.006);	//let rest of 65us + 10us  expire: 
									//use delay_ms because the longest delay_us is 52 at 14.7MHz
	}
	else							//write a "0"
	{
		PutPin(OWPin,OutputLow);	//takes about 43 clocks (2.03us @14.7MHz)
		_delay_ms(0.0052 + 0.057);	//leave bus Low:  DS samples soon after 15us
		PutPin(OWPin,InputTristate);//use delay_ms because the longest delay_us is 52 at 14.7MHz
		_delay_ms(0.006);			//wait the recovery time this setting + loop overhead gives 10us
	}
	sei();							//allow interrupts again
}








//***************************************************************************
//*                                                                         *
//*    One Wire Read Bit:  Reads a bit over the 1-wire bus as               *   
//*    specified by the bus (processor pin).                                *
//*                                                                         *
//*    Timing is:  Pull Low for more than 1us.  (sources vary on the        *
//*    duration.  Some say as short as possible, but more than 1us          *
//*    others suggest at least 5us.  This uses 2-3 us).  Then release       *
//*    the bus.                                                             *
//*                                                                         *
//*    The 1-Wire device will either pull the bus low or leave it high.     *
//*    Data is valid only to the end of 15us after the falling edge         *
//*    at the start of the transmission.  The bus will be read at 11us      *
//*    after the falling edge                                               *
//*                                                                         *
//*    RETURNS: 1 or Zero                                                   *
//*                                                                         *
//***************************************************************************


unsigned char OWReadBit(unsigned int OWPin)
{
	unsigned char value;
	cli();						//do not allow interrupts: timing is critical
	PutPin(OWPin,OutputLow);	//pull bus low
								//the 2 PutPin calls back to back give a Low pulse length
								//of about 2.7us
	_delay_ms(0.0023);			//add more delay to get to 5us
	PutPin(OWPin,InputTristate);
	_delay_ms(0.0029);			//delay to put the sampling at 11us after the first falling edge

	value = GetPin(OWPin);		//sample the bus
	_delay_ms(0.0542 + 0.006);	//wait to end of 65us window, add 10us recovery time
	sei();
	return(value);

}



//************************************************************************************
//*
//*                       Derived functions
//*
//************************************************************************************


//************************************************************************************
void OWWriteByte(unsigned int OWPin, unsigned char ByteVal)
{
	unsigned char bits;
	for (bits =0; bits<7; bits++)
	{
		OWWriteBit(OWPin, ByteVal & 0x01);
		ByteVal >>= 1;
	}
}
//************************************************************************************



//************************************************************************************
void OWWriteData(unsigned int OWPin, unsigned char* data, unsigned char numbytes)
{
	unsigned char pointer;
	for (pointer = 0; pointer < numbytes; pointer++)
	{
		OWWriteByte(OWPin, data[pointer]);
	}
}
//************************************************************************************




//************************************************************************************
unsigned char OWReadByte( unsigned int OWPin)
{
	unsigned char bitnum;
	unsigned char result;

	result = 0;
	for (bitnum=0;bitnum<7;bitnum++)
	{
		result <<= 1;
		result |= OWReadBit(OWPin);
	}

	return result;
}
//************************************************************************************



//************************************************************************************
void OWReadData(unsigned int OWPin, unsigned char* data, unsigned char numbytes)
{
	unsigned char pointer;
	for (pointer=0;pointer<numbytes;pointer++)
	{
		data[pointer] = OWReadByte(OWPin);
	}
}
//************************************************************************************
