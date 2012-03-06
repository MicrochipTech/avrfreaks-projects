/**************************************************************
/
/			DEVBOARD.C
/	
/	Definitions for the devices on board
/	
/
**************************************************************/

#include "devboard.h"

/*		FUNCTION DEFINITIONS		*/


//Initialise LED and switches
void devboard_init()
{
	// enable switches
	DDR(SW1_PORT) &= ~(1<<SW1_POS);	// Switch-1 pin as input with
	PORT(SW1_PORT) |= (1<<SW1_POS);	// pull-up enabled
	DDR(SW2_PORT) &= ~(1<<SW2_POS);	// Also, switch-2 pin as input
	PORT(SW2_PORT) |= (1<<SW2_POS);	// with pull-up enabled
	
	//make LED ready
	DDR(LED1_PORT) |= (1<<LED1_POS);
	DDR(LED2_PORT) |= (1<<LED2_POS);
		
	return;
}


	
	