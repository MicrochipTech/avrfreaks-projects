#include <avr/io.h>

#include "counter.h"
#include "led.h"
#include "in_out.h"

uint8_t run_stop; 

//this function is called every 5ms
void check_buttons(void)
{
uint8_t pb;
static uint8_t pb_last;
	
	pb = PINB;
	
	if ( ( pb & _BV(BUTT_RUN_IB)) < (pb_last & _BV(BUTT_RUN_IB)) )
	{
		toggle_output();
	}
		
	if ( ( pb & _BV(BUTT1_IB) ) < ( pb_last & _BV(BUTT1_IB) ) )
	{ 
		main_counter += 1;	
	}
	if ( ( pb & _BV(BUTT10_IB)) < ( pb_last & _BV(BUTT10_IB) ) )
	{
		main_counter += 10;
	}
	if ( ( pb & _BV(BUTT100_IB) ) < ( pb_last & _BV(BUTT100_IB)) )
	{
		main_counter += 100;
	}
	if ( ( pb & _BV(BUTT1000_IB)) < ( pb_last & _BV(BUTT1000_IB)) )
	{
		main_counter += 1000;
	}
	
	pb_last = pb;	//save old state
}

void port_init(void)
{
	//note : see layout of pins in counter.h file
	
	DDRB = _BV(LED_F_OB) | _BV(LED_G_OB) | _BV(SEG1000_OB);
	DDRC = _BV(CLOCK_OC) | _BV(DIR_OC) | _BV(RELAY_OC);
	DDRD = _BV(LED_A_OD) | _BV(LED_B_OD) | _BV(LED_C_OD) | _BV(LED_D_OD) | _BV(LED_E_OD) | _BV(SEG100_OD) | _BV(SEG10_OD) | _BV(SEG1_OD);
	
	//butons - enable internal pull-ups
	PORTB = _BV(BUTT_RUN_IB) | _BV(BUTT1000_IB) | _BV(BUTT100_IB) | _BV(BUTT10_IB) | _BV(BUTT1_IB);
}

void set_output(uint8_t in)
{
	run_stop = in;
	if (run_stop == RUN) PORTC |= _BV(RELAY_OC);
	else if (run_stop == STOP) PORTC &= ~_BV(RELAY_OC);
}
void toggle_output(void)
{
	if (run_stop == RUN)
	{
		run_stop = STOP;
		PORTC &= ~_BV(RELAY_OC);
	}
	else
	{
		run_stop = RUN;
		PORTC |= _BV(RELAY_OC);
	}
}