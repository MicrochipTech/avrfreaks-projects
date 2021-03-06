/// timers.c ////


#include "main.h"
#include "timers.h"

#include <string.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#define FREE 0
#define RUNNING 1
#define STOPPED 2


byte temp;
word ms_timer[NUM_TIMERS];
volatile word ms_timer_status[NUM_TIMERS+1];
volatile word tick1ms;



SIGNAL (SIG_OUTPUT_COMPARE2)
{	byte counter;
	
	OCR2+=TIMER_2_TICK;			//timer is 1ms
	tick1ms++;

	for (counter=0;counter<NUM_TIMERS;counter++)
	{	if (ms_timer_status[counter]==RUNNING)
		{	if (--ms_timer[counter]==0)
				ms_timer_status[counter]=STOPPED;
		}
	}
}


/*
SIGNAL (SIG_OVERFLOW0)				//timer is 100us
{	
	fast_timer++;
	TCNT0=TIMER_0_INIT_VALUE;
}
*/



void main_timer_init (void)		// Initialize Timers
{		
	//free all timers
	memset(&ms_timer_status,FREE,sizeof(ms_timer_status));
	
	GICR=0;		// disable external interrupts
	
	TCCR2=0x04;				//prescaler 64
	OCR2=TIMER_2_TICK;		//timer 2 is 1ms
	
	TIMSK|=0x80; //Enable 0C2			
}





void pwm_timer_init (void)		//use timer1 for pwm
{		
	ICR1=PWM_STEPS;
	TCCR1A=0xA2;
	TCCR1B=0x19;			//pwm on OC1A and B, no prescaling, fast pwm
	
	
	OCR1A=0;	//start with duty cycle 0%
	OCR1B=0;		

}
	
	
	
byte set_pwm_cycle (byte pwm_num, word value)
{	byte error;

	error=0;
	if (value>PWM_STEPS)
	{	value=PWM_STEPS;
		error=1;
	}
		
	if (pwm_num==1)
		OCR1A=value;
	else
		OCR1B=value;
	
	return error;
}
	
 
 
byte start_timer (word ticks)
{	byte retval,counter;

	counter=0;
	retval=NUM_TIMERS;
	
	do
	{	if (ms_timer_status[counter]==FREE)
		{	ms_timer[counter]=ticks;
			ms_timer_status[counter]=RUNNING;
			retval=counter;
			counter=NUM_TIMERS; //generate abort condition
		}
		counter++;
	}
	while (counter<(NUM_TIMERS));
	return retval;
}



void restart_timer (byte t_number,word ticks)
{	ms_timer[t_number]=ticks;			//reload timer
	ms_timer_status[t_number]=RUNNING;
}

	

byte timer_stopped (byte t_number,byte release)
{	byte retval;
	
	if (t_number>=NUM_TIMERS)
		retval=1;
	else
	{	if (ms_timer_status[t_number]==STOPPED)
		{	retval=1;
			if (release)
				ms_timer_status[t_number]=FREE;
		}
		else
			retval=0;
	}
	return retval;
}


void stop_timer (byte t_number)
{	if (t_number!=NUM_TIMERS)
	{	ms_timer_status[t_number]=FREE;
	}
	
}


void delay (word ticks)
{	byte wait_timer;
	
	wait_timer=start_timer(ticks);
	while (!timer_stopped(wait_timer,0));
}



