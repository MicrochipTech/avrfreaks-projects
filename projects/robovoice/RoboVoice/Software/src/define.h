/*! \file define.h
 * Contains declaration of routines
 */
 
#ifndef DEFINE_H
#define DEFINE_H

//!Uncomment to perform ringModulation.
//#define RINGMOD			

//!Uncomment to perform vibrato with a sawtooth as delay function.
//#define SAWTOOTHVIBRATO

//!Uncomment to perform vibrato with a squarewave as delay function
//#define SQUAREVIBRATO

//!Uncomment to perform vibrato with a sinewave as delay function
//#define SINEWAVEVIBRATO

//!Uncomment to perform reverberation
#define REVERB

/*!Uncomment to perform no sound processing and leave the soundsamples through
 *unchanged.
 */
//#define NO_EFFECT

//////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>				//definitions of registers in AVR
#include <avr/interrupt.h>		//use for interrupts

typedef unsigned char 	byte;//!<Unsigned 8bit type
typedef unsigned int		word;//!<Unsigned 16bit type

//!Boolean type (doesn't exist in C)
typedef enum{
	false,			//!<0
	true			//!<1
} bool;

//!Simple errortype
typedef enum{
	OK,				//!<All was as allright
	NOK				//!<An error occurred
} ERROR;

#define FOSC	16000000 //!<Clock frequency of the MCU [Hz]

void init_ADC(void);
void timer1_init(void);

byte performReverb(int newSample);

ERROR initSineGen(void);
int generateSin(void);
int fastGenerateSin(void);
byte mixer(int sineWave, signed char sample);

ERROR initSawtoothVibrato(void);
byte performSawtoothVibrato(signed char newSample);
byte performSquareVibrato(signed char newSample);
byte performSinewaveVibrato(signed char newSample);

#endif