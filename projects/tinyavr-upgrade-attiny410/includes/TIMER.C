//*****************************************************************************
//
//	Zeitgeber installieren 
//
//
// 1MHz Clk
//*****************************************************************************
#include <iotiny10.h>
#include "timer.h"


#pragma interrupt_handler timer0_int:iv_TIM0_OVF

#define Zeitkonstante 100

unsigned int ui_zeit;  //delay
unsigned int ui_z1;    //Abwaertszaehler1

//*****************************************************************************
//	Timer0-Interrupt
//  Input:	-
//	Output:	-
//*****************************************************************************
void timer0_int(void)
{
   TCNT0H = 0xFF;	//65534 - CNT125 1ms; UP-CNT
   TCNT0L = 0x8A;
   TIFR &= 0xFE;	//Overflow Flag reset
	
    if (ui_zeit > 0) ui_zeit--;
 	if (ui_z1 > 0) ui_z1--;
}


//*****************************************************************************
//	Timer - Init
//  Input:	-
//	Output:	-
//*****************************************************************************
void timer_init(void)
{
//    SREG &= 0x7F;
 	ui_zeit = 0;
  				    //Timer init fuer 1ms = 1MHz / VT8 / CNT125 /2
   TCCR0A = 0x00;  //keine PWM
   TCCR0B = 0x02;	//keine Ausgaenge mit Timer schalten, Vorteiler div8
   TCCR0C = 0x00;
   TIMSK |= 0x01;  //Timer0 Int enable
   TCNT0H = 0xFF;	//65534 - CNT125 1ms; UP-CNT
   TCNT0L = 0x8A;
   TIFR |= 0x01;	//Overflow Flag reset  
//    SREG |= 0x80;	//global Int enable
}


//*****************************************************************************
//	Timer - DELAY
//  Input:	Zeitangabe (zeit in ms)
//	Output:	-
//*****************************************************************************
void timer_delay(unsigned int x)
{
 	 ui_zeit = x;
	 while(ui_zeit > 0);		  	   //warte bis zeit abgelaufen
}


//*****************************************************************************
//	Timer - Abwärtszaehler1
//  Input:	(zeit in ms)
//	Output:	-
//*****************************************************************************
void timer_setcount1(unsigned int x)
{
   ui_z1 = x;
}


//*****************************************************************************
//	Timer - Abwärtszaehler1
//  Input:	(zeit in ms)
//	Output:	-
//*****************************************************************************
unsigned int timer_getcount1(void)
{
   return(ui_z1);
}



