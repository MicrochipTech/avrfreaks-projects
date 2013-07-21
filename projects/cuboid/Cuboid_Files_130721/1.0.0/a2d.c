/*

A2D module

*/

	#include "includes.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define LED_I_CHANNEL		1
	#define	SUPPLY_V_CHANNEL	2
	#define	LED_V_CHANNEL		3
	#define LIGHT_CHANNEL		4

	//Time to leave all LEDs off before measuring light level
	#define LIGHT_SETUP_TIME	0.3
	
	//Time to average light measurement, removes mains hum from fluro lights
	#define LIGHT_SAMPLE_TIME	0.3

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static unsigned int	time;
	static char			vled_monitor;
	static char			vled_monitor_stop;
	
//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

//********************************************************************************************************
// Public functions
//********************************************************************************************************

void a2d_tick(void)
{
	if(time)
		time--;
}

void a2d_fly(void)
{
	//Repeatedly check LED supply voltage, reset if over safety threshold
	//to stop this process, set vled_monitor_stop, and wait for v_led_monitor_run==FALSE
	//to start, begin a conversion of vled channel and set vled_monitor
	if(vled_monitor)
	{
		//Conversion finished?
		if(!(ADCSRA & _BV(ADSC)))
		{
			//If supply is over limit, shutdown leds and cause reset.
			if(ADC > (unsigned int)(POWER_VLED_LIMIT * 102.4))
			{
				led_shutdown();
				wdt_enable(WDTO_15MS);
				while(1);
			};
			
			//request to stop monitoring vled?
			if(vled_monitor_stop)
				vled_monitor=FALSE;	//stop
			else
				ADCSRA |=_BV(ADSC);	//else start next conversion
		};
	};
}

void a2d_init(void)
{
	//Enable A2D with Fosc/128 which gives 125kHz A2D clock if FOSC=16MHz
	ADCSRA = 0x87;

	//A2D inputs with no pullup
	DDRF  &=~_BV(LED_I_CHANNEL);
	DDRF  &=~_BV(SUPPLY_V_CHANNEL);
	DDRF  &=~_BV(LED_V_CHANNEL);
	DDRF  &=~_BV(LIGHT_CHANNEL);

	//A2D no pullup
	PORTF  &=~_BV(LED_I_CHANNEL);
	PORTF  &=~_BV(SUPPLY_V_CHANNEL);
	PORTF  &=~_BV(LED_V_CHANNEL);
	PORTF  &=~_BV(LIGHT_CHANNEL);

	//start conversion of LED_V_CHANNEL
	ADMUX = _BV(REFS0)+LED_V_CHANNEL;
	ADCSRA |=_BV(ADSC);
	vled_monitor=TRUE;
}

double a2d_led_i(void)
{
	double x;

	//ensure column is ON full intensity
	led_plane_intensity_all(255);
	led_line(0,0,3,0,7,3,1);
	led_waitframe(2);

	//stop monitoring led supply voltage	
	vled_monitor_stop = TRUE;
	while(vled_monitor)
		main_fly();
	
	//convert LED_I_CHANNEL
	ADMUX = _BV(REFS0)+LED_I_CHANNEL;
	ADCSRA |=_BV(ADSC);
	while(ADCSRA & _BV(ADSC))
		main_fly();

	//calculate result
	x = ADC/12.33;

	//re-start monitoring of LED_V_CHANNEL
	ADMUX = _BV(REFS0)+LED_V_CHANNEL;
	ADCSRA |= _BV(ADSC);
	vled_monitor=TRUE;

	return x;
}

double a2d_led_fv(void)
{
	double x;
	
	x = a2d_led_v() - (a2d_led_i()/71.42);
	
	return x;
}

double a2d_led_v(void)
{
	double x;

	//stop monitoring led supply voltage	
	vled_monitor_stop = TRUE;
	while(vled_monitor)
		main_fly();

	//convert LED_V_CHANNEL
	ADMUX = _BV(REFS0)+LED_V_CHANNEL;
	ADCSRA |=_BV(ADSC);
	while(ADCSRA & _BV(ADSC))
		main_fly();

	//calculate result
	x = ADC/102.4;

	//re-start monitoring of LED_V_CHANNEL
	ADMUX = _BV(REFS0)+LED_V_CHANNEL;
	ADCSRA |= _BV(ADSC);
	vled_monitor=TRUE;

	return x;
}

double a2d_supply_v(void)
{
	double x;

	//stop monitoring led supply voltage	
	vled_monitor_stop = TRUE;
	while(vled_monitor)
		main_fly();
	
	//convert SUPPLY_V_CHANNEL
	ADMUX = _BV(REFS0)+SUPPLY_V_CHANNEL;
	ADCSRA |=_BV(ADSC);
	while(ADCSRA & _BV(ADSC))
		main_fly();

	//calculate result
	x = ADC/47.63;

	//re-start monitoring of LED_V_CHANNEL
	ADMUX = _BV(REFS0)+LED_V_CHANNEL;
	ADCSRA |= _BV(ADSC);
	vled_monitor=TRUE;

	return x;
}

unsigned int a2d_light(void)
{
	unsigned int x;
	unsigned int sample_count=0;
	unsigned long sample_acc=0;
	
	//All LEDs OFF!
	led_blank();

	//wait sample time to ensure LEDs do not affect reading
	time = (unsigned int)(LIGHT_SETUP_TIME/TIMER_TICK);
	while(time)
		main_fly();
	
	//stop monitoring led supply voltage	
	vled_monitor_stop = TRUE;
	while(vled_monitor)
		main_fly();
	
	//convert
	time = (unsigned int)(LIGHT_SAMPLE_TIME/TIMER_TICK);
	while(time)
	{
		//convert LIGHT_CHANNEL
		ADMUX = _BV(REFS0)+LIGHT_CHANNEL;
		ADCSRA |= _BV(ADSC);
		do
		{
			main_fly();
		}while(ADCSRA & _BV(ADSC));
		sample_acc += ADC;
		sample_count++;
	};
	sample_acc /= sample_count;

	//calculate result
	x = 1023-sample_acc;

	//re-start monitoring of LED_V_CHANNEL
	ADMUX = _BV(REFS0)+LED_V_CHANNEL;
	ADCSRA |= _BV(ADSC);
	vled_monitor=TRUE;

	return x;
}


//********************************************************************************************************
// Private functions
//********************************************************************************************************
