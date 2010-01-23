/*
Author:  Matt Meerian
Date:  November 14, 2008
Purpose:  This program displayes images on an 8 x 16 LED array.  One image
	gradually fades from one image to another image.  

Developement System:  AVR Studio, Version 4.14, build 589
Target Microcontroller:  ATTiny861 from Atmel (AVR series) (8K of flash)
Target Board:  009-0024-00 Rev A
Software Version:  1.0

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

#include <avr\io.h>
#include <avr\interrupt.h>  //used to allow interrupts
#include <stdbool.h>    	//this file allows boolean data types in "C"
#include "define.h"			//defines, must be before the global variables since some global
#include <avr\pgmspace.h>   //must be included to store arrays in FLASH
#include <avr\eeprom.h>     //used for writing to the EEPROM
#include <string.h>

//subroutine definitions for the main.c file
void SrvTock(void);	//every 20mS this routine is run
void uCinit(void);	//initialization routine for the ATMEGA128
void display16Bytes(void);	//loads 16 bytes of data in to the display drivers
void setupBits(void);		//changes around the display bits so they will display properly on the display
unsigned char bitReverseAByte( unsigned char x );	//used to reverse the bits in a byte

//---------------------------------------------------------------------------
//Declair the data types
typedef unsigned char u08;  //A character, 8 bits
typedef unsigned short u16; //a short integer, big endian 16 bits, 0 to 65535
typedef bool ubool;         //a boolean, true or false, must include the header file "stdbool.h"

/*global varibles*/
//Note:  Variables used in interrupts must be declaired as volatile
volatile u16 uTock=defTock;	//A counter used for the 20mS tock
volatile u08 u08State;	//the variable for the state machine
u08 u08Temp;	//used in several places
u08 u08StateTmr=0x01;
unsigned char u08DisplayData[16];	//data to be displayed on the 16 byte display (post process)
unsigned char u08Input[16];			//preprossed image data
unsigned char u08FirImage[16];		//an array in ram to hold the first frame data from flash
unsigned char u08SecImage[16];		//an array in ram to hold the second frame data from flash
unsigned char u08FrameDisplay;		//a used to set which image is being displayed
volatile u08 u08TmrDimming;		//used in the interrupt to keep track of where we are in the dimming cycle
volatile u08 u08DimmingLevel;	//sets the diming level of the display
volatile bool boolFirstFrame;	//when true, the first frame is being displayed
bool uboolFadeOut;	//when true, the display is fading out
bool uboolFadeIn;	//when true, the display is fading in

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))		//Set and clear bits in I/O registers 
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 		//Macro for testing of a single bit in an I/O location

//space invader
prog_uchar strLedScrn1[]={0xF8, 0x3, 0x80, 0x7, 0xE0, 0x1E, 0x80, 0x7, 0x80, 0x7, 0xE0, 0x1E, 0x80, 0x7, 0xF8, 0x3, 0x00,0x00,0x00};
//pacman
prog_uchar strLedScrn2[]={0xE0, 0xF, 0x10, 0x10, 0x8, 0x20, 0x8, 0x20, 0x28, 0x21, 0x88, 0x22, 0x50, 0x14, 0x20, 0x8, 0x8, 0x00,0x00,0x00};
//happy face
prog_uchar strLedScrn3[]={0xFC, 0x3F, 0x2, 0x40, 0x11, 0x88, 0x81, 0x90, 0x81, 0x90, 0x11, 0x88, 0x2, 0x40, 0xFC, 0x3F,0x00,0x00,0x00};
//hangman person
prog_uchar strLedScrn4[]={ 0x0, 0x41, 0x86, 0x20, 0x49, 0x10, 0xF9, 0xF, 0x49, 0x10, 0x86, 0x20, 0x0, 0x41, 0x0, 0x0, 0x00,0x00,0x00};
//cube, on the bottom
prog_uchar strLedScrn5[]={ 0x0, 0xFE, 0x0, 0x83, 0x80, 0x82, 0x40, 0x82, 0x40, 0xFE, 0x40, 0x41, 0xC0, 0x20, 0xC0, 0x1F, 0x00,0x00,0x00};
//a candy cane
prog_uchar strLedScrn6[]={ 0xFC, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xF, 0x0, 0x7, 0x0, 0xF, 0x0, 0x1E, 0x0, 0x1C, 0x0, 0x00,0x00,0x00};
//snowman
prog_uchar strLedScrn7[]={ 0x0, 0x38, 0xC0, 0x45, 0x2C, 0x82, 0x12, 0x82, 0x12, 0x82, 0x2C, 0x82, 0xC0, 0x45, 0x0, 0x38, 0x00,0x00,0x00};
//Martini glass
prog_uchar strLedScrn8[]={ 0x18, 0x0, 0x78, 0x80, 0xF8, 0x81, 0xF8, 0xFF, 0xF8, 0xFF, 0xFC, 0x81, 0x7A, 0x80, 0x19, 0x0,0x00,0x00,0x00};
//Christmas tree
prog_uchar strLedScrn9[]={ 0x80, 0x24, 0xD0, 0x36, 0xF8, 0x3F, 0xFC, 0xFF, 0xFC, 0xFF, 0xF8, 0x3F, 0xD0, 0x36, 0x80, 0x24, 0x00,0x00,0x00};
prog_uchar strLedScrn10[]={ 0x00,0x00,0x00};
prog_uchar strLedScrn11[]={ 0x00,0x00,0x00};

//the numbers 1 through 4, used to test the fading in and out of images
prog_uchar str1[]={ 0x0, 0x0, 0x1F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00,0x00,0x00};
prog_uchar str2[]={ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x12, 0x0, 0x11, 0x0, 0x19, 0x0, 0x16, 0x0,0x0, 0x00,0x00,0x00};
prog_uchar str3[]={ 0x40, 0x4, 0x20, 0x8, 0x20, 0x9, 0xC0, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,  0x0, 0x00,0x00,0x00};
prog_uchar str4[]={ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC0, 0x1, 0x0, 0x1, 0xC0, 0x7,  0x00,0x00,0x00};


//----------------------------------------------------------------------------
//Routine:     main (the main loop of the program)
//
//Function:    This is where to program will exicute until an interrupt happens or
//              someone yanks the power plug.
//
//Varibles passed:
//       none
//Returns:
//       integer (no idea where this integer is returned to)
//----------------------------------------------------------------------------
// Main loop
int main( void )
{
	uCinit();		//go through the initialization process for the microcontroller
	sei();        	//enable interrupts
	u08State=0;		//start out state

	while(1)       // go into an infinite loop here
	{
        SrvTock();        //go to the tock every 20ms
	}
	return 0;
}

//**************************************************************************
//----------------------------------------------------------------------------
//Routine:     signal(sig_overflow0)  (timer 0 overflow interrupt)
//
//Function:    signal handler for tcnt0 overflow interrupt
//              happens every 64.39 microseconds
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
SIGNAL(SIG_OVERFLOW0)
{
	u08 u08IntTemp;

	if (uTock > 0)
	{
      uTock--;      //This counts down to a 20ms tock.  (which is run in the main loop)
	}

	if(boolFirstFrame==true)	//are we showing the first image?
	{
		SETBIT(PORTB,6);		//debug
		if((u08DimmingLevel == u08TmrDimming) && u08DimmingLevel>0)	//is it time to turn on the display?
		{
			setupBits();	//convert the 16 byte input array into the output array for the display
			display16Bytes();	//load the output array into the display drivers
		}
		if(u08TmrDimming==0)	//it is time to turn off the display?
		{
			SETBIT(PORTB,3);
			for(u08IntTemp=0;u08IntTemp<16;u08IntTemp++)	//step through the 16 display bytes
			{
				u08DisplayData[u08IntTemp]=0;	//turn off the display
			}
			display16Bytes();	//load the output array into the display drivers
			CLEARBIT(PORTB,3);	//debug
		}
	}
	else	//we must be showing the second image
	{
		CLEARBIT(PORTB,6);	//debug
		if(u08DimmingLevel == u08TmrDimming)	//is it time to turn off the display?
		{
			for(u08IntTemp=0;u08IntTemp<16;u08IntTemp++)	//step through the 16 display bytes
			{
				u08DisplayData[u08IntTemp]=0;	//turn off the display
			}
			display16Bytes();	//load the output array into the display drivers
		}
		if(u08TmrDimming==0 && u08DimmingLevel<(defIntPwmDimming))	//it is time to turn on the display?
		{
			setupBits();	//convert the 16 byte input array into the output array for the display
			display16Bytes();	//load the output array into the display drivers
		}
	}

	if(u08TmrDimming==0)	//have we reached the end of a PWM dimming cycle?
	{
		u08TmrDimming=defIntPwmDimming;	//was 41 //reset the dimming timer
		
		if(boolFirstFrame==true)	//are we displaying the first frame?
		{
			boolFirstFrame=false;	//display the second frame next time through
			for(u08IntTemp=0;u08IntTemp<16;u08IntTemp++)
			{
				u08Input[u08IntTemp]=u08FirImage[u08IntTemp];	//load up the first image in the display array
			}
		}
		else	//we must be displaying the second frame
		{
			boolFirstFrame=true;	//display the first frame next time through
			for(u08IntTemp=0;u08IntTemp<16;u08IntTemp++)
			{
				u08Input[u08IntTemp]=u08SecImage[u08IntTemp];	//load up the second image in the display array
			}
		}
	}
	else
	{
		u08TmrDimming--;	//we are PWM dimming the display
	}
	
}

//----------------------------------------------------------------------------
//Routine:      SrvTock  (Service the tock)
//
//Function:     Exicute the tock every 20mS from the main loop.
//				This is where ALL the good stuff happens
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void SrvTock(void)
{
    if (uTock == 0)
    {
        uTock = defTock;   //reload the Tock (20mS) timer

		if(uboolFadeIn==true)	//is the display fading in?
		{
			u08DimmingLevel++;	//increase the intensity of the display
			if(u08DimmingLevel>(defIntPwmDimming-1))	//was 35	//are we done increasing the intensity?
			{
				uboolFadeIn=false;	//stop increasing the intensity of the display
			}
		}

		if(uboolFadeOut==true)	//is the display fading out?
		{
			u08DimmingLevel--;		//decrease the intensity of the of the display
			if(u08DimmingLevel==0)	//are we done increasing the intensity?	(note: this leaves the display slightly on)
			{
				uboolFadeOut=false;	//stop decreasing the intensity 
			}
		}

		//This switch statement is the main state machine
        switch (u08State)
        {
			case 0:
				u08StateTmr--;
				if(u08StateTmr==100)	//is it time to fade in?
				{
					for(u08Temp=0;u08Temp<16;u08Temp++)	//load all 16 bytes from flash into the ram array
					{
						if(u08FrameDisplay==0)
						{
							u08SecImage[u08Temp]=pgm_read_byte(&strLedScrn3[u08Temp]);
						}
						else if(u08FrameDisplay==1)
						{
							u08FirImage[u08Temp]=pgm_read_byte(&strLedScrn6[u08Temp]);
						}
						else if(u08FrameDisplay==2)
						{
							u08SecImage[u08Temp]=pgm_read_byte(&strLedScrn7[u08Temp]);
						}
						else
						{
							u08FirImage[u08Temp]=pgm_read_byte(&strLedScrn9[u08Temp]);
						}
					}
					if(u08FrameDisplay==0 || u08FrameDisplay==2)
					{
						uboolFadeIn=true;	//fade in the current image
					}
					if(u08FrameDisplay==1 || u08FrameDisplay==3)
					{
						uboolFadeOut=true;	//fade out the current image
					}
					u08FrameDisplay++;	//advance to the next image in the display
					if(u08FrameDisplay>3)	//is it time to reset the image steps?
					{
						u08FrameDisplay=0;	//reset back to the first image
					}
				}
				if(u08StateTmr==0)	//is it time to reset the fade-in/fade-out timer?
				{
					u08StateTmr=101;
				}
			break;

		}
	}
}

//Reverses the order of bits in a byte. (the MSB is swapped with LSB, etc.)
unsigned char bitReverseAByte(unsigned char u08ByteToReverse )
{
    u08ByteToReverse = ((u08ByteToReverse >> 1) & 0x55) | ((u08ByteToReverse << 1) & 0xaa);
    u08ByteToReverse = ((u08ByteToReverse >> 2) & 0x33) | ((u08ByteToReverse << 2) & 0xcc);
    u08ByteToReverse = ((u08ByteToReverse >> 4) & 0x0f) | ((u08ByteToReverse << 4) & 0xf0);
    return u08ByteToReverse;   
} 

//----------------------------------------------------------------------------
//Routine:		setupBits (setup the bits for the display)
//
//Function:		
//				Good luck figuring this one out.  The LED's on the board were not laid out from 0 to 15
//				from top to bottom.  Since the LEDs are not in the correct order on the PCB, this 
//				routine puts the bits in the correct order.  The data is loaded into the u08Input array
//				and the resultant array is u08DisplayData.
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void setupBits(void)
{
	u08 u08CountThrough;

	for(u08CountThrough=0; u08CountThrough<16; u08CountThrough+=2)	//go through all 8 of the driver chips (each chip takes 2 bytes)
	{
		//byte 0, lower nibble;  reverse the lower byte and then mask off the lower nibble
		u08DisplayData[u08CountThrough+1]=bitReverseAByte(u08Input[u08CountThrough]) & 0xF0;	
		//byte 0, upper nibble;  shift the upper nibble 4 positions lower and then write to the output array
		u08DisplayData[u08CountThrough] = u08Input[u08CountThrough] >> 4;		
		//byte 1, lower nibble;  take the upper byte, lower nibble 4 positions to the left
		u08DisplayData[u08CountThrough] = u08DisplayData[u08CountThrough] | (u08Input[u08CountThrough+1] << 4);
		//byte 1, upper nibble;  reverse the upper byte, mask off the upper nibble and then "or" the results
		u08DisplayData[u08CountThrough+1] = u08DisplayData[u08CountThrough+1] | (bitReverseAByte(u08Input[u08CountThrough+1])&0x0F);
	}
}

//----------------------------------------------------------------------------
//Routine:		display16Bytes
//
//Function:		This routine bit-bangs data through an SPI port to the LED
//				display drivers.  (using a data line, clock line, latch line and
//				an enable line)  There are 8 LED driver chips.  The display data
//				is stored in the 16 byte array "u08DisplayData"
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void display16Bytes(void)
{
	volatile u08 u08Counter=1;		//this int holds the bit mask to shift through the 16 bit integer
	volatile u08 u08ShiftCounter;	//used to count a bit shift through a 16 bit interger
	volatile u08 u08CurrentByte;
	
	//defDispDriverPort &= ~(1<<defLedLatch);		//start out with all of the lines going to the driver chip held low
	//defDispDriverPort &= ~(1<<defLedEnable);
	//defDispDriverPort &= ~(1<<defLedData);
	//defDispDriverPort &= ~(1<<defLedClock);
	asm volatile("nop");
	defDispDriverPort |= (1<<defLedLatch);	//take the latch high to start loading in data
	defDispDriverPort |= (1<<defLedEnable);	//turn the drivers off while shift in data
	for(u08CurrentByte=0;u08CurrentByte<16;u08CurrentByte++)	//cycle through all 16 bytes that are to be displayed
	{
		u08Counter=1;	//reset the bit shift counter
		for(u08ShiftCounter=0;u08ShiftCounter<8;u08ShiftCounter++)	//cycle through each bit in the 16 bytes
		{
			if(u08DisplayData[u08CurrentByte] & u08Counter)		//This uses a bit mask to set the data line 
			{
				defDispDriverPort |= (1<<defLedData);		//we want to turn on the corresponding LED driver output using the data line
			}
			else
			{
				defDispDriverPort &= ~(1<<defLedData);		//we want to turn off the corresponding LED driver output
			}
			u08Counter = (u08Counter<<1);	//shift the bitmask to the left
			defDispDriverPort |= (1<<defLedClock);			//clock the data into the LED driver chip
			asm volatile("nop");
			defDispDriverPort &= ~(1<<defLedClock);		//get the clock line ready for next time
		}
	}	
	defDispDriverPort &= ~(1<<defLedLatch);	//take the latch low to latch in the data
	defDispDriverPort &= ~(1<<defLedEnable);	//turn the drivers on after the data has been loaded into the driver ic
}

//----------------------------------------------------------------------------
//Routine:		uCinit 
//
//Function:		Initialize the microcontroller registers.
//
//Varibles passed:
//				none
//Returns:
//       		none
//----------------------------------------------------------------------------
void uCinit(void)
{
		PORTA=0x10;          	//enable pull-up resistors
		DDRA=0xEF;   			//set PORTA to outputs
		PORTB=0xE0;          	//enable pull-up resistors
		DDRB=0xFF;   			//set PORTB to outputs

		//8 bit timer initialization (timer 0)
		TIMSK=(1<<TOIE0);   //Enable TCNT0 overflow
		TCNT0H=0;         	// reset TCNT0
		TCNT0L=0;         	// reset TCNT0
		TCCR0A=0;			//just a normal overflow
		TCCR0B=(1<<CS01);	//|(1<<CS00);        	//count with cpu clock/1
}

