//	this program counts events from optical input
//	output is displayed on LED display, 4 digits
//	clock = 8MHz, internal RC oscillator
//	input from optical part is periodically strobed every 256us
//  output is valid only in case that last 8(?) values were the same
//
//  Peter Rosko, p.eastman@zoznam.sk, 21.9.2007



#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/wdt.h>


#include "adc.h"
#include "in_out.h"
#include "led.h"
#include "counter.h"

uint8_t clock, dir; //current values
uint8_t clock_last, dir_last; //previous valid values


volatile uint8_t timer256us;
uint8_t timer1ms;
uint8_t timer5ms;
uint8_t timer100ms;

void timer_init(void);

int main(void)
{
	timer_init();
	port_init();
	adc_init();

	main_counter = 0;
	set_output(STOP);
	
	//wd enabled + cca 0.5 sec wd interval
	wdt_enable(WDTO_500MS);		

	sei();
	
	for(;;)
	{
		if (timer100ms >= 10)
		{
			
			timer100ms = 0;
		}
		if (timer5ms >= 20) //called every 100ms
		{
			int16_t main_counter_tmp;
			
			cli();
			main_counter_tmp = main_counter;
			sei();
			
			set_display(main_counter_tmp);	
						
			wdt_reset();
			
			timer100ms++;
			timer5ms = 0;
		}
		
		if (timer1ms >= 5) //called every 5ms
		{
			check_buttons();
			refresh_led();
			
			timer5ms++;
			timer1ms = 0;
		}
																					
		cli();
		if (timer256us >= 4) //called every 1ms
		{
			timer1ms++;
			timer256us -= 4;
		}
		sei();
		
		//indication LEDs
		if ((clock & FILTER_MASK) == FILTER_MASK) PORTC &= ~_BV(CLOCK_OC);
		if (clock == 0) PORTC |= _BV(CLOCK_OC);
		if ((dir & FILTER_MASK) == FILTER_MASK) PORTC &= ~_BV(DIR_OC);
		if (dir == 0) PORTC |= _BV(DIR_OC);
		
		//plausibility check
		if (main_counter > 9999) main_counter = -999;
		else if (main_counter < -999) main_counter = 9999;
	
	}
	
	return(0);
	
}

void timer_init(void)
{

	TCCR0 = _BV(CS01); //(1/8e6)*8*256 = 256us - only general timer here
	TCCR2 = _BV(CS21); //it is the same value as Timer0, but is separatly configurable in a future - input filter
	
	TCNT2 = 0;	
	TCNT0 = 120; //to split effort
	
	TIMSK = _BV(TOIE0) | _BV(TOIE2); //enable interrupt
}

ISR(SIG_OVERFLOW0)
{
	//increases own value every 256us ( (1/8e6)*256*8)
	timer256us++;
}

//check the input signals periodically (every 256us)
//filter them
//calculate number of rotation and direction
ISR(SIG_OVERFLOW2)
{
	clock = clock << 1;	
	if (adc_clock > 200) clock++; //valid 1, add 1
	else if (adc_clock < 50) clock = clock; //valid 0, add 0
	else clock = 2; //forbidden range, type poor value 0b10
	
	dir = dir << 1;
	if (adc_dir > 200) dir++;
	else if (adc_dir < 50) dir = dir;
	else adc_dir = 2;
	
	//if last 8(?) values were the same, then we could take the averaged value as correct info
	if ( (dir == 0) || ((dir & FILTER_MASK ) == FILTER_MASK) ) //valid bits
	{
		dir_last = dir;
	}
	
	if ( (clock == 0) || ((clock & FILTER_MASK) == FILTER_MASK) )
	{
		if ( clock_last > clock )
		{
			if (dir_last)
			{
				main_counter--; //if DIR LED is not connected, then is internally connected to log.1
			}
			else
			{
				main_counter++;
			}
		}
		clock_last = clock;
	}
	
	if (main_counter <= 0) set_output(STOP);
}