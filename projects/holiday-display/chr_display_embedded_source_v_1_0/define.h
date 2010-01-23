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

//defines
#define defTock         	250		//when the timer interrupt allows for a 20mS tock (was 800 for a 51mS tock)

#define def861		//are we using an ATTiny861?
//#undef def8611	//comment in to use the ATTiny2313 processor

#ifdef def861
	#define defDispDriverPort	PORTA	//The port which go to the display driver pins
	//portA equates
	#define defLedData			0		//data line for the MBI5027GN LED display driver
	#define defLedClock			2		//clock line for the MBI5027GN LED display driver
	#define defLedLatch			1		//latch line for the MBI5027GN LED display driver
	#define defLedEnable		3		//enable line for the MBI5027GN LED display driver
	#define defLedFeedback		4		//Feedback line for the MBI5027GN LED display driver
#else		//we must be using the ATTiny2313 processor
	#define defDispDriverPort	PORTD	//The port which go to the display driver pins
	//portD equates
	#define defLedData			4		//data line for the MBI5027GN LED display driver
	#define defLedClock			2		//clock line for the MBI5027GN LED display driver
	#define defLedLatch			1		//latch line for the MBI5027GN LED display driver
	#define defLedEnable		3		//enable line for the MBI5027GN LED display driver
	#define defLedFeedback		6		//Feedback line for the MBI5027GN LED display driver
#endif




#define defBtnDbn			12		//This controls the button debounce time (20mS * value) (was 5)
#define defAdcDelayTime		25		//sets the delay between ADC readings

#define defIntPwmDimming	41		//sets the number of interrupts to create a full on/off cycle of the PWM dimming
