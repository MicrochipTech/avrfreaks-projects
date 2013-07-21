/*
	Documentation
*/

#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define TEXTOUT_P(arg1)		uart1_tx_string_P(arg1)
	#define TEXTOUT(arg1)		uart1_tx_string(arg1)
	#define TEXTOUT_FLOAT(arg1)	do { sprintf(text, "%5.2f", arg1); uart1_tx_string(text); } while(0)
	
//********************************************************************************************************
// Local defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static char 			text[10];

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

//********************************************************************************************************
// Public functions
//********************************************************************************************************

void power_init(void)
{
	//Fast PWM mode OC0, clear on match, set at bottom(0)
	DDRB |= _BV(PB4);

	//Fosc gives 62kHz PWM_-
	TCCR0 =  _BV(WGM01)+_BV(WGM00)+_BV(COM01)+_BV(CS00);

	//maximum voltage
	OCR0=0; 
}

void power_vled_set(unsigned char x)
{
	OCR0 = 255-x;
}

unsigned char power_vled_get(void)
{
	unsigned char x;
	x = 255-OCR0;
	return x;
}

//initial supply test executed at reset
void power_test(void)
{
	//LEDs off
	led_blank();

	led_framerate(50);
	power_vled_set(255);
	led_waitframe(25);

	TEXTOUT_P(PSTR("\r\nSupply input         - "));
	TEXTOUT_FLOAT(a2d_supply_v());
	TEXTOUT_P(PSTR("v\r\nMaximum LED supply   - "));
	TEXTOUT_FLOAT(a2d_led_v());
	TEXTOUT_P(PSTR("v\r\n"));

	if(a2d_led_v() > POWER_VLED_LIMIT)
	{
		TEXTOUT_P(PSTR("ERROR!! (Too high!)"));
		while(1);
	};

	TEXTOUT_P(PSTR("Load test...\r\n"));
	memset(led_frame_bitmap, 0xFF, 64 );
	led_plane_intensity_all(255);
	led_waitframe(50);

	TEXTOUT_P(PSTR("\r\nSupply input         - "));
	TEXTOUT_FLOAT(a2d_supply_v());
	TEXTOUT_P(PSTR("v\r\nMaximum LED supply   - "));
	TEXTOUT_FLOAT(a2d_led_v());
	TEXTOUT_P(PSTR("v\r\nMaximum LED current  - "));
	TEXTOUT_FLOAT(a2d_led_i());
	TEXTOUT_P(PSTR("mA\r\nLED forward drop     - "));
	TEXTOUT_FLOAT(a2d_led_fv());
	TEXTOUT_P(PSTR("v\r\n"));
	led_blank();
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************
