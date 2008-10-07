/*
 *  Module:    pin_numbers.c
 *  Title:     Oak Micros Sample Application - pin_numbers
 *  Author:    Mike Perks (mikep@oakmicros.com)
 *  Tools:     avr-gcc 3.4.6 / avr-libc 1.4.4
 *  Hardware:  om128
 *
 *  Module Description
 *  ------------------
 * 
 *  The module contains an example of how to access the I/O pins of the om128 
 *  device just using the pin number. 
 *
 *  The pin number is encoded in a #define value e.g. OMPin10 refers to pin 10
 *  on the device. If preferred an actual number could be used eg. 10 and a 
 *  lookup done in an array for all 40 pins.
 *
 *  The code makes use of the fact that DDRx is usually at address one less than
 *  the corresponding PORTx and PINx is at address two less than PORTx. Note that
 *  PINF is an exception for the ATMega128.
 *
 *	Acknowlegements
 *  ---------------
 
 * 	Acknowledgment to spamiam for suggesting the idea and to avrfreaks.net 
 *  especially user "cpluscon" for suggesting the idea for this implementation.
 *
 *  Change History
 *  --------------
 * 
 *  1.0  20071116 First version to support om128 
 *
 ****************************************************************************
 */

/* ---- Standard header files --------------------------------------------- */

#include <avr/io.h>
#include <avr/pgmspace.h>


/* ---- Custom header files ----------------------------------------------- */

#include "pin_numbers.h"


/* ---- Private Constants and Types --------------------------------------- */

prog_uint16_t pinarray[] = { 0,
	OMPin1,
	OMPin2,
	OMPin3,
    OMPin4,
	OMPin5,
	OMPin6,
	OMPin7,
	OMPin8,
	OMPin9,
	OMPin10,
	OMPin11,
	OMPin12,
	OMPin13,
    OMPin14,
	OMPin15,
	OMPin16,
	OMPin17,
	OMPin18,
	OMPin19,
	OMPin20,
	OMPin21,
	OMPin22,
	OMPin23,
    OMPin24,
	OMPin25,
	OMPin26,
	OMPin27,
	OMPin28,
	OMPin29,
	OMPin30,
	OMPin31,
	OMPin32,
	OMPin33,
    OMPin34,
	OMPin35,
	OMPin36,
	OMPin37,
	OMPin38,
	OMPin39,
	OMPin40
};

/* ---- Private Variables ------------------------------------------------- */
/* ---- Private Function Prototypes --------------------------------------- */
/* ---- Interrupt Routines ------------------------------------------------ */
/* ---- Private Functions ------------------------------------------------- */
/* ---- Public Functions -------------------------------------------------- */


/**
 *
 * GetPin
 *
 */
uint8_t
GetPin(uint16_t x_pin)
{
	volatile uint8_t *p = (volatile uint8_t *) (x_pin&0xFF);

	// point to PINx
	p = p-2;

	// exception for port F on an ATmega128
	if (p == (volatile uint8_t *)0x60) {
		p = (volatile uint8_t *)0x20;
	}

	// return input value
	if ((*p) & x_pin>>8)
		return 1;
	else
		return 0;
}

/**
 *
 * GetPinByNumber
 *
 */
uint8_t
GetPinByNumber(uint8_t pin)
{
	return GetPin(pgm_read_word_near(pinarray + pin*2));
}


/**
 *
 * PutPin
 *
 */
//void PutPin(uint16_t x_pin, unsigned char code)
//{
//	volatile uint8_t *p = (volatile uint8_t *) (x_pin&0xFF);
//
//	if (code > Toggle) {
//		return;
//	}
//	else if (code == Toggle) {
//        *p ^= x_pin>>8;	 // toggle PORT
//	} else {
//		// set DDR for input or output
//		if (code <= InputHigh) {
//			*(p-1) &= ~ x_pin>>8;
 //		} else {
//			*(p-1) |= x_pin>>8;
//			
//		}
//
//		// set PORT bit to 0 or 1
//		if ((code % 2) == 0) {
//			*p &= ~ x_pin>>8; 
//		} else {
//			*p |= x_pin>>8; 
//		}
//	}
//}

//The sequence of DDR and PORT may well make a difference.
//For OutPut:  set PORT, then DDR
//For Input:  set DDR then PORT

void PutPin(uint16_t x_pin, unsigned char code)
{
	volatile uint8_t *p = (volatile uint8_t *) (x_pin&0xFF);

	if (code > Toggle) return;
	
	else
	{
		switch (code)
		{
			case OutputLow:
				*p &= ~ x_pin>>8; //set PORT  LOW
				*(p-1) |= x_pin>>8;//set DDR OUTPUT
			break;

			case OutputHigh:
				*p |= x_pin>>8;  //set PORT HIGH
				*(p-1) |= x_pin>>8;//set DDR OUTPUT
			break;

			case InputTristate:
				*(p-1) &= ~x_pin>>8;  //set DDR INPUT
				*p &= ~ x_pin>>8; //set PORT LOW
			break;

			case InputHigh:
				*(p-1) &= ~x_pin>>8;  //set DDR INPUT
				*p |= x_pin>>8;  //set PORT HIGH
			break;
		}
	}
}

/**
 *
 * PutPinByNumber
 *
 */
void
PutPinByNumber(uint8_t pin, unsigned char code)
{
	PutPin(pgm_read_word_near(pinarray + pin*2), code);
}



