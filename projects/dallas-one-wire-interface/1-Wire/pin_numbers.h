/*
 *  Module:    pin_numbers.h
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

#ifndef PIN_NUMBERS_H_
#define PIN_NUMBERS_H_  

   
/* ---- Standard header files --------------------------------------------- */

#include <avr/io.h>  //only used for the test code, not the final code

/* ---- Custom header files ----------------------------------------------- */

/* ---- Constants and Types ----------------------------------------------- */

/* Pin Table */ 
#define OMPin1       0x0000	// illegal
#define OMPin2       0x0000
#define OMPin3       0x0000 
#define OMPin4       0x0000

#define OMPin5       0x8032 //PortD address 0x32
#define OMPin6       0x4032 
#define OMPin7       0x2032 
#define OMPin8       0x1032
#define OMPin9       0x0832
#define OMPin10      0x0432
#define OMPin11      0x0232
#define OMPin12      0x0132

#define OMPin13      0x8023	// PortE address 0x23
#define OMPin14      0x4023
#define OMPin15      0x2023
#define OMPin16      0x1023
#define OMPin17      0x0823
#define OMPin18      0x0423
#define OMPin19      0x0223	// illegal?
#define OMPin20      0x0123 // illegal?

#define OMPin21      0x8038	// PortB address 0x38
#define OMPin22      0x4038
#define OMPin23      0x2038
#define OMPin24      0x1038
#define OMPin25      0x0838
#define OMPin26      0x0438
#define OMPin27      0x0238
#define OMPin28      0x0138

#define OMPin29      0x8062 // PortF address 0x62
#define OMPin30      0x4062
#define OMPin31      0x2062
#define OMPin32      0x1062
#define OMPin33      0x0862
#define OMPin34      0x0462
#define OMPin35      0x0262
#define OMPin36      0x0162

#define OMPin37      0x0000 // illegal
#define OMPin38      0x0000
#define OMPin39      0x0000
#define OMPin40      0x0000


/* defines for PutPin */
#define OutputLow		0
#define OutputHigh		1
#define InputTristate		2
#define InputHigh		3
#define Toggle		4

/* ---- Public Variables -------------------------------------------------- */

/* ---- Public Functions -------------------------------------------------- */

extern uint8_t GetPin(uint16_t x_pin);
extern uint8_t GetPinByNumber(uint8_t pin);
extern void PutPin(uint16_t x_pin, unsigned char code);
extern void PutPinByNumber(uint8_t pin, unsigned char code);


#endif /*PIN_NUMBERS_H_*/
