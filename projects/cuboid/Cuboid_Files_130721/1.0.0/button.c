/*
Buttons functions are:

Select - Choose display sequence number, or select Auto.

Brightness increase
Brightness decrease

Skip/Hold	-	If patterns are rotating
					Pressing once stops rotation.
					
				If patterns are not rotating
					Pressing rotates through patterns manually.
					Holding for 1sec enables rotation

*/

	#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define BUTTON_PORT		PORTA
	#define BUTTON_DDR		DDRA
	#define BUTTON_PIN		PINA
	
	#define BUTTON_SELECT_BIT	PA0
	#define BUTTON_SKIP_BIT		PA1
	#define BUTTON_BRDEC_BIT	PA2
	#define BUTTON_BRINC_BIT	PA3
	
	#define BUTTON_MASK	(_BV(BUTTON_SELECT_BIT)+_BV(BUTTON_SKIP_BIT)+_BV(BUTTON_BRINC_BIT)+_BV(BUTTON_BRDEC_BIT))

	#define BRIGHTNESS_MIN		90

	//Time to hold SELECT to enable/disable sequence rotation
	#define SELECT_HOLD		1

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

//********************************************************************************************************
// Public functions
//********************************************************************************************************

void button_tick(void)
{
	unsigned char x;
	static int skip_hold_time;
	static int select_hold_time;
	static char brightness_modified=FALSE;

	if(!(BUTTON_PIN & _BV(BUTTON_SELECT_BIT)))
	{
		//Press instant
		if(select_hold_time == 0)
		{
			//pass button press to mode selector
			sequence_select_press();
			
			select_hold_time++;
		};
	}
	else
		select_hold_time=0;

	if(!(BUTTON_PIN & _BV(BUTTON_SKIP_BIT)))
	{
		//Press instant
		if(skip_hold_time == 0)
			sequence_skip();			
		
		//Held for 1sec, enable rotation
		if(skip_hold_time == (int)(SELECT_HOLD/TIMER_TICK))
			sequence_rotate_en();
		
		skip_hold_time++;
	}
	else
		skip_hold_time=0;

	//Increase brightness
	if(!(BUTTON_PIN & _BV(BUTTON_BRINC_BIT)))
	{
		x=power_vled_get();
		if(x!=255)
			power_vled_set(x+1);
		brightness_modified = TRUE;
	}
	//Decrease brighntess
	else if(!(BUTTON_PIN & _BV(BUTTON_BRDEC_BIT)))
	{
		x=power_vled_get();
		if(x > BRIGHTNESS_MIN)
			power_vled_set(x-1);
		brightness_modified = TRUE;
	}
	else if(brightness_modified)
	{
		brightness_modified=FALSE;
		sequence_save_brightness();	//this may cause a hickup in the displayed pattern due to eeprom write time (oh well)
	};	
}

void button_init(void)
{
	//Button lines inputs with pullups enabled
	BUTTON_DDR &=~BUTTON_MASK;
	BUTTON_PORT |= BUTTON_MASK;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************
