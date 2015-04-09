/*
 * interrupts.c

 *
 *  Created on: 22 paŸ 2013
 *      Author: M.R.O.
 */

/**************************
 * --->>  Includes  <<--- *
 **************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include "hd44780.h"
#include "Functions.h"
#include "usart.h"
#include "Adc.h"
#include <util/delay.h>
#include "Menu.h"
#include "dma_driver.h"
#include "Globals_Defs.h"

/***************************
 * --->>  Interrupts  <<--- *
 ***************************/

ISR(PORTR_INT1_vect) /* Interrupts from encoders */
{
	if (PORTH_IN & PIN3_bm) { A--; }
		else 	{ A++; }
}

ISR(PORTR_INT0_vect)
{
	if (PORTH_IN & PIN5_bm)	{ B++; }
		else	{ B--;  }
}

ISR(PORTH_INT0_vect)
{
	if (PORTH_IN & PIN0_bm)	{ C--; }
		else	{ C++; }
}

ISR(PORTJ_INT0_vect)
{
	if (PORTJ_IN & PIN4_bm) { D--; }
		else 	{ D++; }
}

ISR(PORTQ_INT0_vect)
{
	if (PORTJ_IN & PIN0_bm)	{ E--; }
		else	{ E++; }
}

ISR(PORTK_INT0_vect)
{
	if (PORTJ_IN & PIN3_bm)	{ F--; }
		else 	{ F++; }
}

ISR (TCC1_OVF_vect) /* Time base, 32Mhz/1024 = 31250 Hz */
{
	TCC1.CNT = 65535;
	Counter_Value++;
}

ISR (TCD0_OVF_vect) /* Timer for Delay_ms function */
{
	TCD0.CNT = 33537; /* 1000Hz */
	WaitTime--;
	if (WaitTime == 0)
	{
		DelayFlag = TRUE;
		TCD0.CTRLA = TC_CLKSEL_OFF_gc;
	}
}

ISR (DMA_CH2_vect)
{
	DMA.CH2.CTRLB |= DMA_CH_TRNIF_bm; /* Clear interrupt flag */

	if(Conversion == 0 )
	{
		Conversion = 1;
		ADCA.CTRLA = ADC_FLUSH_bm;
		ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN4_gc;
		ADCA.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN5_gc;
		ADCA.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN6_gc;
		ADCA.CH3.MUXCTRL = ADC_CH_MUXPOS_PIN3_gc;	/* Channel 4 is updated each interrupt */

		DMA.CH2.DESTADDR0  =(((uint16_t)&AdcBuf[4])>>0*8) & 0xFF;
		DMA.CH2.DESTADDR1  =(((uint16_t)&AdcBuf[4])>>1*8) & 0xFF;
		DMA.CH2.DESTADDR2 = 0;
	}
	else
	{
		Conversion = 0;
		ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN0_gc;
		ADCA.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc;
		ADCA.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc;
		ADCA.CH3.MUXCTRL = ADC_CH_MUXPOS_PIN3_gc;

		DMA.CH2.DESTADDR0  =(((uint16_t)&AdcBuf)>>0*8) & 0xFF;
		DMA.CH2.DESTADDR1  =(((uint16_t)&AdcBuf)>>1*8) & 0xFF;
		DMA.CH2.DESTADDR2 = 0;
	}
		ADCA.CTRLA = ADC_CH0START_bm | ADC_CH1START_bm | ADC_CH2START_bm | ADC_CH3START_bm | ADC_ENABLE_bm;
}

ISR (USARTC1_RXC_vect)	/* Receiving data form UART */
{
	USART_MergingData();
}
