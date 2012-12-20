// Written by Ozhan KD (www.knowledgeplus.ir)

#include "XMEGA_timer_v2.h"

volatile bool timer_f=false;
volatile unsigned char time_out;
//---------------------------------

ISR(TC_OVF_INTERRUPT)
{
 static unsigned char x20=0;
 timer_f=true;
 if(++x20==20)
 {
  x20=0; 
  if(time_out)--time_out;
 }
}

//---------------------------------

void TimerInit(void)
{
 TC_PER=(unsigned int)((F_CPU/800U)-1U);
 TC_CTRLA=TC_CLKSEL_DIV8_gc;
 TC_INTCTRLA=TC_OVFINTLVL_LO_gc;
 PMIC_CTRL|=PMIC_LOLVLEN_bm;
}

//---------------------------------

void delay_ms(unsigned int delay)
{
 while(delay--) delay_us(1000);
}

//---------------------------------