/// timers.c ////


#include "Main.h"
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

/*
SIGNAL (SIG_OUTPUT_COMPARE1B)		// ISR for system timer
{ 	
	OCR1B+=TIMER_1_TICK;			//timer is 50ms
	if (++sys_tick > 19)
	{  	sys_tick=0;
		if (++time.sec>59)
		{	time.sec=0;
			if(++time.min>59)
			{	time.min=0;
				if (++time.hour>23)
				{	time.hour=0;
					if (++time.day>29)
					{	time.day=0;
						if (++time.month>12)
						{	time.month=0;
							time.year++;
						}
					}
				}
			}
		}		
	}
}
*/


SIGNAL (SIG_OUTPUT_COMPARE1A)
{	byte counter;
	
	OCR1A+=TIMER_2_TICK;			//timer is 1ms
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
	
	outp (0,GICR);		// disable external interrupts
	
	outp (2,TCCR0);		// Timerclock 0 = Sysclock/8
	TCNT0=TIMER_0_INIT_VALUE;

	TCCR1A=0;
	outp (0x02,TCCR1B);  	//Prescaler 8, Normal mode
	outp (0xff,TCNT1H);
	outp (0xff,TCNT1L);		// Preset counter of timer 1
	OCR1A=TIMER_2_TICK;		//timer 2 is 1ms
//	OCR1B=TIMER_1_TICK;		//TIMER1 is 50ms
	
//	TIMSK|=0x18;  //Enable OC1A,OC1B Interrupt, Disable OF0 Interrupt	
	TIMSK|=0x10; //Enable 0C1A			
}


/*
void pwm_timer_init (void)		//use timer2 for pwm
{	byte u8aux;
	
	u8aux=0x61;				//phase correct PWM, no prescaling, clear on match
	TCCR2=u8aux;
	
	u8aux=0x1A;
	OCR2=u8aux;				//start with duty cycle 10%
}
*/

/*
void reset_fast_timer (void)
{	fast_timer=0;	
}



void delay (word  ticks)
{	reset_fast_timer();	
	while 	((fast_timer) != ticks);
}
*/


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



byte timer_stopped (byte t_number)
{	byte retval;
	
	if (t_number>=NUM_TIMERS)
		retval=1;
	else
	{	if (ms_timer_status[t_number]==STOPPED)
		{	retval=1;
			ms_timer_status[t_number]=FREE;
		}
		else
			retval=0;
	}
	return retval;
}


void stop_timer (byte t_number)
{	if (t_number!=NUM_TIMERS)
	{	if	(ms_timer_status[t_number]!=FREE)
			ms_timer_status[t_number]=FREE;
	}
	
}


void delay (word ticks)
{	byte wait_timer;
	
	wait_timer=start_timer(ticks);
	while (!timer_stopped(wait_timer));
}



