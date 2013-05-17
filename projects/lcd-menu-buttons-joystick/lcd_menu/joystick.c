/*
Description:
    A library to easily access ADC with AVR series of
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

#include "joystick.h"
#include "lcd.h"





/////////////////////////////////////////////////////////////////////////////
///This function takes x,y from adc and finds the direction that ////////////
///////////////////points the joystick.//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////0-->Do nothing////////////
/////1-->Go up/////////////////
/////2-->Go right//////////////
/////3-->Go down///////////////
/////4-->Go left///////////////
/////5-->Go up-right///////////
/////6-->Go right-down/////////
/////7-->Go down-left//////////
/////8-->Go left-up////////////
///////////////////////////////
uint16_t find_direction_from_xy(uint16_t x,uint16_t y)
{

	if(x>255 && x<765 && y>765)
	return(1);

	else if(x>765 && y>255 && y<765)
	return(2);

	else if(x>255 && x<765 && y<255)
	return(3);

	else if(x<255 && y>255 && y<765)
	return(4);

	else if(x>765 && y>765)
	return(5);

	else if(x>765 && y<255)
	return(6);

	else if(x<255 && y<255)
	return(7);

	else if(x<255 && y>765)
	return(8);

	else
	return(0);
}

