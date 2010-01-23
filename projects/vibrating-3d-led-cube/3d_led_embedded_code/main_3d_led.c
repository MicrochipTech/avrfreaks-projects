/*!
Author:  Matt Meerian
Date:  January 2, 2009
Purpose:  
	This software displays a three dimensional LED matrix.  The LED's are in a 5x5 pixel
	arrangement.  The 5x5 arrangement is then vibrated at about 10Hz to give the third
	dimension.  A photo interruptor is used to detect the position of the LED's within
	the mechanical oscillations.
Developement System:  AVR studio Version 4.15, build 623
Compiler:  WINAVR 20081205
Target Microcontroller:  ATMega16 from Atmel (AVR series)

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//! Set the data types
typedef unsigned char u08;  //! A character, 8 bits
typedef unsigned short u16; //! a short integer, big endian 16 bits, 0 to 65535

//! these are the include files
#include <avr\io.h>
#include <avr\interrupt.h>  //! used to allow interrupts
#include <stdbool.h>    //! this file allows boolean data types in "C"
#include <avr\pgmspace.h>   //! must be included to store arrays in FLASH
#include <avr\wdt.h>        //! for the watchdog timer
#include "define.h"		//! hold constants for the program
#include "flash.h"
//! NOTE:  we must define F_CPU in define.h before delay.h is called
#include <util\delay.h>		//! used to put inline delays

//! subroutine definitions
void uCinit(void);		//! initialization routine for the microcontroller
void SrvTock(void);

//! global varibles
volatile unsigned int intTock=defTock;         //! A counter used for the 20mS tock
volatile u08 u08TmrFlick=0;		//! a delay to turn on an LED
volatile u08 u08CurrentFrame=0;	//! a value used to store the current frame in the image
volatile u08 u08FrameDelay=0;	//! holds the number of times each 3D image repeats
volatile u08 u08ShownFrame=0;	//! contains the current frame shown on the 5x5 matrix
u08 u08State=0;			//! This is used in the state machine
u08 u08TmrState=0;		//! a state timer
u08 u08AnimationSeq=0;	//controls which animation is being seen on the display

//! Macros to make bit and byte port operation a little easier
#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))		//! Set and clear bits in I/O registers 
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 		//! Macro for testing of a single bit in an I/O location

//! ----------------------------------------------------------------------------
//! This is where to program will exicute until an interrupt happens or someone yanks the power plug.
//! ----------------------------------------------------------------------------
int main( void )
{
	cli();			//! disable interrupts
	uCinit();		//! go through the initialization process for the microcontroller
	sei();			//! enable interrupts
	
	while(1)		//!  go into an infinite loop here
	{
		wdt_reset();	//! reset the watchdog
		SrvTock();		//! go to the tock every 20ms
	}
   return 0; 
}

//! ----------------------------------------------------------------------------
//! Initialize the I/O lines, timers, interrupts, etc. of the microcontroller
//! ----------------------------------------------------------------------------
void uCinit(void)
{
	//! port initialization
	PORTA=0b00000000;		//! pullups, 0=no pullup, 1=pullup on
	DDRA=0b11111111;     	//! input/output, 0=input, 1=output
	PORTB=0b00000000;		//! pullups, 0=no pullup, 1=pullup on
	DDRB=0b11111111; 		//! input/output, 0=input, 1=output
	PORTC=0b00000000;		//! pullups, 0=no pullup, 1=pullup on
	DDRC=0b11111111;     	//! input/output, 0=input, 1=output
	PORTD=0b00000100;		//! pullups, 0=no pullup, 1=pullup on
	DDRD=0b11111011; 		//! input/output, 0=input, 1=output

	//! external interrupt 0
	MCUCR=(1<<ISC01)|(1<<ISC00);	//! the rising edge generates the external interrupt
	GICR=(1<<INT0);		//! enable external interrupt 0

	//! 8 bit timer initialization (timer 0)
	TIMSK=(1<<TOIE0);   //! Enable TCNT0 overflow
	TCNT0=0;         	//!  reset TCNT0
	TCCR0=1;

	PORTA = 0xFF;	//! turn off all of the LED's in the 5x5 matrix
	PORTC = 0xFF;
	PORTB = 0xFF;
	PORTD = 0xFF;
}

//! ----------------------------------------------------------------------------
//! Timer 0 overflow interrupt happens every 64.39 microseconds
//! ----------------------------------------------------------------------------
ISR (SIG_OVERFLOW0)
{
	if(intTock>0)	
	{	
		intTock--;	//! counts down to a 20mS tock used in the main loop
	}
}

//! ----------------------------------------------------------------------------
//! Interrupt for the external interrupt 0.  This routine runs when the photo interrupter fires
//! ----------------------------------------------------------------------------
ISR (SIG_INTERRUPT0)
{
	u08 u08IsrTemp;	//! used to step through the four bytes in each image
	u08 u08FlashValue[4];	//! used to hold the flash image bytes in RAM

	u08TmrFlick=15;

	for(u08CurrentFrame=0;u08CurrentFrame<5;u08CurrentFrame++)	//! step through the frames
	{
		for(u08IsrTemp=0;u08IsrTemp<=3;u08IsrTemp++)		//! step through the flash bytes
		{
			switch(u08ShownFrame)
			{
				//! start of single wave
				case 0:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave1[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 1:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave2[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 2:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave3[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 3:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave4[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 4:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave5[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 5:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave6[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 6:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave7[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 7:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave8[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 8:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave9[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 9:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave10[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 10:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave11[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 11:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strSingleWave12[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				//! end of single wave

				case 20:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strHelix1[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 21:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strHelix2[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 22:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strHelix3[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 23:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strHelix4[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 24:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strHelix5[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 25:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strHelix6[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 26:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strHelix7[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
				case 27:
					u08FlashValue[u08IsrTemp]=pgm_read_byte(&strHelix8[u08IsrTemp+(u08CurrentFrame*4)]);	//! load the flash bytes of the image arrray into ram
				break;
			}
		}
		PORTA = u08FlashValue[0];	//! load the image into the ports for the LED's
		PORTC = u08FlashValue[1];	//! load the image into the ports for the LED's
		PORTB = u08FlashValue[2];	//! load the image into the ports for the LED's
		PORTD = u08FlashValue[3];	//! load the image into the ports for the LED's
		_delay_us(30);	//! an inline delay between frames (30uS) Makes the LED's look like they are on instantaneously
		PORTA = 0xFF;	//! turn off all of the LED's
		PORTC = 0xFF;	//! turn off all of the LED's
		PORTB = 0xFF;	//! turn off all of the LED's
		PORTD = 0xFF;	//! turn off all of the LED's
		_delay_ms(8);	//! an inline delay between frames (was 8mS)
	}
	PORTA = 0xFF;	//! turn off all of the LED's (we are done with the current frame)
	PORTC = 0xFF;	//! turn off all of the LED's (we are done with the current frame)
	PORTB = 0xFF;	//! turn off all of the LED's (we are done with the current frame)
	PORTD = 0xFF;	//! turn off all of the LED's (we are done with the current frame)
	if(u08FrameDelay>1)	//! This is the number of times each 3D image repeats
	{
		u08FrameDelay=0;	//! reset the counter for the next image
		u08ShownFrame++;			//! we are ready for the next image
	}
	u08FrameDelay++;	//! advance the frame delay counter
	if(u08ShownFrame>11)	//! have we reached the end of the frames?  (11 for LtoRwave, 27 for helix
	{
		u08ShownFrame=0;	//! we are ready to start over again with the animation (0 for LtoRwave, 20 for helix)
	}
}

//! ----------------------------------------------------------------------------
//! Execute the tock every 20mS from the main loop.
//! ----------------------------------------------------------------------------
void SrvTock(void)
{
	if (intTock==0)
	{
		intTock=defTock;		//! reload 311 for a 20mS tock at 4MHz
		switch (u08State)	//! controls the very small state machine
		{
			case 0:  //! are we showing the first animation sequence?
				if(u08TmrFlick==0)	//! is it time to change the animation sequence?
				{
					u08State=1;		//go to the next animation sequence
					u08TmrFlick=0xFF;	//! start the delay off at the maximum for sequence
				}
				else
				{
					u08TmrFlick--;
				}

			break;

			case 1:  //! are we showing the first animation sequence?
				if(u08TmrFlick==0)	//! is it time to change the animation sequence?
				{
					u08State=0;		//go to the next animation sequence
					u08TmrFlick=0xFF;	//! start the delay off at the maximum for sequence
				}
				else
				{
					u08TmrFlick--;
				}

			break;
		}
	}
}





