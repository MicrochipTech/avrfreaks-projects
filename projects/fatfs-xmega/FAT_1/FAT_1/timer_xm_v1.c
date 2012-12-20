// Written by Ozhan KD (www.knowledgeplus.ir)

#include "timer_xm_v1.h"

volatile bool timer_f=false;
//---------------------------------

ISR(TC_OVF_INTERRUPT)
{
 timer_f=true;
}

//---------------------------------

void TimerInit(void)
{
 TC_PER=(unsigned int)((F_CPU/800U)-1U);
 TC_CTRLA=TC_CLKSEL_DIV8_gc;	
}

//---------------------------------

void delay_ms(unsigned int delay)
{
 while(delay--) delay_us(1000);
}

//---------------------------------