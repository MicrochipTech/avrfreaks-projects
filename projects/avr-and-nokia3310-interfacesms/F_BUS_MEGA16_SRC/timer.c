//*******************************************************
//*****          SOURCE FILE : timers.c         ********
//*******************************************************
#include "timer.h"
#include "F-Bus.h"
extern unsigned char InitFrameIndex , ACKrecieved ,SendAns;
unsigned char t=0;
void _init_timer1(void)
{
	OCR1A=0xFFFF; /* 1 Sec */
	TCCR1B=(1<<CS10)|(1<<CS11)|(1<<WGM12);
	//TIMSK|=(1<<OCIE1A);
}
void _init_timer0(void)
{
	TCCR0=(1<<CS02)|(1<<CS01)|(1<<CS00); // 8000000/(1024*256 cycles) = 
	/*TIMSK|=(1<<TOIE0);*/
}


ISR(SIG_OUTPUT_COMPARE1A)
{
	if(++t == 8)
	{
		t=0;
		__toggle_led();
		__disable_tr1_interrupt();
		SendAns = YES;
		initPhone ();
	}
}

