/*
Description:
    A library to easily access 4x4 keypad with AVR series of
    MCUs from Atmel. Use with AVR studio and WinAVR.

Author:
    Panos Mavrogiannopoulos
Web:
    http://nmav.homeip.net
*/

#include <avr/io.h>
#include <inttypes.h>

#ifndef F_CPU
	#define F_CPU 2000000UL
#endif

#include <util/delay.h>

#include "keypad4x4.h"

#define LOCK_INPUT_TIME 100     /* time to wait after a button press */








int check_if_button_pressed()
{
	int i;
	
		
		//first column
		keypad_port =0b01111111;
	
		//check for rows and send key number to portD
		//instead sending key number to PORTD you can use
		// any function that serves pressed button

		


		i=keypad_pin;
		if(keypad_pin == 0b01110111) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 1");
		return(49);
		}

		if(keypad_pin == 0b01111011) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 2");
		return(50);
		}


        if(keypad_pin == 0b01111101) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 3");
		return(51);
		}


		if(keypad_pin == 0b01111110) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED:A");
		return(65);
		}


				
		keypad_port =0b10111111;

		i=keypad_pin;
		if(keypad_pin == 0b10110111) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 4"); 
		return(52);
		}

		if(keypad_pin == 0b10111011) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 5");
		return(53);
		}


        if(keypad_pin == 0b10111101) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 6");
		return(54);
		}


		if(keypad_pin == 0b10111110) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED:B");
		return(66);
		}

		//third column
		keypad_port =0b11011111;

		i=keypad_pin;
		if(keypad_pin == 0b11010111) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 7");
		return(55);
		}

		if(keypad_pin == 0b11011011) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 8");
		return(56);
		}


        if(keypad_pin == 0b11011101) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 9");
		return(57);
		}

		if(keypad_pin == 0b11011110) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED:C");
		return(67);
		}

		//fourth column
		keypad_port =0b11101111;

		i=keypad_pin;
		if(keypad_pin == 0b11100111) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED:*");
		return(42);
		}

		if(keypad_pin == 0b11101011) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED 0");
	    return(48);
		}
	

        if(keypad_pin == 0b11101101) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED:#");
		return(35);
		}

		if(keypad_pin == 0b11101110) 
		{
		_delay_ms(LOCK_INPUT_TIME);
		//LCDWriteStringXY(0,0, "BUTTON PRESSED: D");
        return(68);
		}

}





