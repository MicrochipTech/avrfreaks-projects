/******************************************************************************
 * Project      : Software UART ATtiny13
 * File         : Software UART driver.h
 * Author      	: Jeroen Lodder
 ******************************************************************************
  Change History:

    Version 1.0 - June 2010
    > Created
	Version 2.0 - November 2011
	> Revisioned
	> Optimised to Flash 552 bytes
	> Added detailed comments
	> Can disable functions with one // to save flash
******************************************************************************/
#ifndef byte
	#define byte uint8_t			//Very helpful
#endif

#include <avr/interrupt.h>
#include <avr/io.h>

/* Changeables: */
#define TXport		PB2		//TX port, (use bit number)
#define PORTREG		PORTB
#define DIRREG		DDRB
#define TIMERVALUE 	62		//See below for calculation method (4,8 mhz, 9600 8n1 == 62)

/* Comment the defines of functions you do not need below */
#define putint				//Send a 16 bit int
#define putbyte_ascii 		//Send byte as ascii to terminal
#define putint_ascii		//Send a 16 bit int as ascii to terminal
#define putstring			//Send a string to terminal
//uses a lot af flash because the strings you send are stored in flash and ram

/* Not to change*/
#define IDLE		0
#define STARTBIT	1
#define TRANSMIT	2
#define STOPBIT 	3
#define FINISHED	4

//Main functions
void init_suart(void);				//Initialise the software uart and timers
void sTX_putchar(byte);				//Start sending a byte 

//Terminal functions
#ifdef putbyte_ascii 
	void sTX_putbyte_ascii(byte);		//Send a byte as ascii
#endif
#ifdef putint
	void sTX_putint(int);				//Send a integer
#endif
#ifdef putint_ascii
	void sTX_putint_ascii(int);			//Send a integer as ascii
#endif
#ifdef putstring
	void sTX_putstring(char *p);		//Send a string
#endif



/*	Calculation for compare register: 
Xtal = sysclk
Baud = Baudrate
Prec = Prescaler of timer
Check for overflows

			Xtal			4800000			
N Ocnx	=	------------- =	---------	=	61,5
			Baud	Presc	9600	8		
*/
