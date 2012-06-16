/*******************************************************************
 * isr.h
 * written by      Ralf Meeh 06.07.2008
 * last changed by Ralf Meeh 27.12.2009
 * Function:       Service Routinen, alle Interrupts,Projektabhängig
 *******************************************************************/

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "defines.h"
#include "io.h"
#include "isr.h"
#include "ctcss_enc.h"
#include "lcd-routines.h"

volatile uint16_t 	tick;

extern int T500_Status;

// Interrupt Service Routine für Timer0 Überlauf
ISR(TIMER0_OVF_vect)
{
	uint8_t i;
	for(i=0;i<ANZ_TIMER;i++)
	  if(T[i])
	    T[i]--;
	cnt_10ms++;

	if(cnt_10ms>=10)	// Prüfung ob 10ms Service ansteht
	{
	  FLAG_10ms=1;
	  cnt_10ms=0;
	}
//  ---- diesen Befehl immer am Ende der ISR stehen lassen sonst Zeitfehler
	TCNT0=COUNT_0 + TCNT0;		//noch COUNT_0 taktzyklen bis Interrupt ->defines.h
}



//---------------------------------------
// 10ms Handler für Timer usw.			-
//---------------------------------------
void handle_10ms(void)
{  	
	uint8_t i;
	read_rotary();
	for(i=0;i<ANZ_TIMER_10;i++)
	  if(T_10[i])
	    T_10[i]--;
	FLAG_10ms = 0;
	cnt_500ms++;
	if(cnt_500ms >= 50)
	{
		FLAG_500ms=1;
		cnt_500ms = 0;
	}
}

//---------------------------------------
// 500ms Handler für lahme Sachen usw.	-
//---------------------------------------
void handle_500ms(void)
{  	
	LED_PORT ^= (1<<LED);
    set_cursor(14,2); 
/*	if((LED_PORT & (1<<LED)) == (1<<LED))
		lcd_string("-");
	else
		lcd_string("|");*/
	switch(T500_Status)
	{
		case 0:
			{
				lcd_string("-");
			}
			break;
		case 1:
			{
				lcd_data(42);
			}
			break;
		case 2:
			{
				lcd_string("|");
			}
			break;
		case 3:
			{
				lcd_string("/");
			}
			break;
		default:
				lcd_string("*");
	}

	T500_Status++;
	if(T500_Status > 3)
		T500_Status = 0;
	if(T_ACTIVE == 1)
	{
		set_cursor(12,1);
		lcd_string("ON ");
	}
	else
	{
		set_cursor(12,1);
		lcd_string("OFF");
	}

	FLAG_500ms = 0;
}


/*
ISR(ADC_vect)
{
}

ISR(ANA_COMP_vect)
{
}

ISR(EE_RDY_vect)
{
}

ISR(INT0_vect)
{
}

ISR(INT1_vect)
{
}

ISR(SPI_STC_vect)
{
}

ISR(TIMER1_CAPT_vect)
{
}

ISR(TIMER1_COMPA_vect)
{
}

ISR(TIMER1_COMPB_vect)
{
}

ISR(TIMER1_OVF_vect)
{
}

ISR(TIMER2_OVF_vect)
{
}

ISR(TWI_vect)
{
}

ISR(USART_RXC_vect)
{
}

ISR(USART_TXC_vect)
{
}

ISR(USART_UDRE_vect)
{
}

ISR(SPM_RDY_vect)
{
}
*/

