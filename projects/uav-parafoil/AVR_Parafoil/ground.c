#include "main.h"
#include "ground.h"
 
volatile u08 pwm_counter=0x00;
volatile u08 pwm_period=0x00;
 
ISR(PCINT1_vect)
{
	static u08 pulse;
	if(PORTC & 0x01)									//start of C.0 pulse
	{
		TCNT0=0x00;										//reset timer at start of C.0 pulse
		pulse=TRUE;
	}
	else if(pulse)												//end of C.0 pulse
	{
		pulse=FALSE;
		if(TCNT0>upper_pwm_bound && TCNT0<lower_pwm_bound)	//valid pwm
		{
			if(pwm_counter<0x0A)
			{
				pwm_counter++;
			}
			else
			{
				pwm_period=TCNT0;	//store the pwm period, the kalman filter will like it!
			}
		}
		else
		{
			sbi(TCCR1A,COM1A1);								//reenable the PWM
			pwm_counter=0x00;
		}
	}
	if(pwm_counter==0x0A)									//a stream of valid pwm
	{
		cbi(TCCR1A,COM1A1);									//turn off PWM output to servo
		PORTB=(PORTB & ~0x03)|(PORTC & 0x03);			//move PORTC 0,1 to PORTB 0,1
	}
}
 
ISR(TIMER0_OVF_vect)
{
	sbi(TCCR1A,COM1A1);								//reenable the PWM
	pwm_counter=0x00;									//with valid pwm, there will be no overflows
}
 
void enable_ground_control()
{
	sbi(TIMSK0, TOIE0);									//Timer0 interrupts on
	sbi(PCICR, PCIE1);									//Pin change interrupts on
}
 
void disable_ground_control()
{
	cbi(TIMSK0, TOIE0);									//Timer0 interrupts on
	cbi(PCICR, PCIE1);
	pwm_counter=0x00;									//Pin change interrupts on
}

