# XMEGA DAC & DMA control for waveform generation

Uploaded by divgup on 2015-05-05 08:24:00 (rating 0 out of 5)

## Summary


```

/**
 * \file
 *
 * \brief AVR XMEGA Digital to Analog Converter Driver Example 1
 *
 * Copyright (C) 2010-2015 Atmel Corporation. All rights reserved.
 *
 * \asf\_license\_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf\_license\_stop
 *
 */
 // IC Used : ATMEGA 32A4u


#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "gpio.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "conf\_usb.h"
#include "udc.h"
#include "udd.h"
#include "ui.h"
#include "math.h"
#include "nvm.h"
#include "tc.h"
#include "asf/xmega/drivers/dac/dac.h"
#include "asf/xmega/drivers/dma/dma.h"
#include <defines.h>
#include <stdio.h>
#include <stdlib.h>
#include <rvkbd4x4.h>
#include <avr/eeprom.h>
#include "ioport\_compat.h"


#define SPEAKER\_DAC             DACB
#define SPEAKER\_DAC\_CHANNEL DAC\_CH1
//#define CONFIG\_SLEEPMGR\_ENABLE

#define DMA\_CHANNEL     0

/** \brief Size of DMA buffers */


//! Rate at which DA conversions should be done.
#define RATE\_OF\_CONVERSION    100000

//! Number of samples in sample buffer.
#define NR\_OF\_SAMPLES         32
#define DMA\_BUFFER\_SIZE NR\_OF\_SAMPLES

uint16\_t i;

//! Sample buffer with one period of a sine wave.
uint16\_t sine[NR\_OF\_SAMPLES] = {
	32768, 35325, 37784, 40050, 42036, 43666, 44877, 45623,
	45875, 45623, 44877, 43666, 42036, 40050, 37784, 35325,
	32768, 30211, 27752, 25486, 23500, 21870, 20659, 19913,
	19661, 19913, 20659, 21870, 23500, 25486, 27752, 30211,
};

//
//ISR(TCC0\_OVF\_vect)
//{
	//
	////i++;
	//
//}

ISR(DMA\_CH0\_vect)
{
DMA.CH0.CTRLB|=0x10;
		DMA.CH0.CTRLA|=0x10;
		DMA.CH0.CTRLA|=0x80;	
	
	
}
static void configure\_pins(void)
{
	/* Disable the digital part of the DAC output pin */

	ioport\_configure\_pin(DAC\_CH1, IOPORT\_DIR\_OUTPUT |IOPORT\_INPUT\_DISABLE);
}

static void configure\_dac()
{
	struct dac\_config conf;
	//PORTB.DIR|=PIN2\_bm|PIN3\_bm;
	//PORTB.DIRCLR=0x00;
	//PORTB.DIRTGL=0x00;
	
	dac\_read\_configuration(&SPEAKER\_DAC, &conf);

	/* Create configuration:
	 * - 1V from bandgap as reference, left adjusted channel value
	 * - one active DAC channel, no internal output
	 * - conversions triggered by event channel 0
	 * - 1 us conversion intervals
	 */
	dac\_set\_conversion\_parameters(&conf, DAC\_REF\_AREFB, DAC\_ADJ\_LEFT);
	//dac\_set\_active\_channel(&conf, SPEAKER\_DAC\_CHANNEL, DAC\_CH1EN\_bm);
	conf.ctrla=0x08;
	conf.ctrlb=0x22;
	conf.evctrl=0x00;
	//dac\_set\_conversion\_trigger(&conf, SPEAKER\_DAC\_CHANNEL, 0);
	dac\_write\_configuration(&SPEAKER\_DAC, &conf);
	dac\_enable(&SPEAKER\_DAC);
}
/*static void example\_dma\_transfer\_done(enum dma\_channel\_status status)
{
	
	PORTD.OUTTGL=0x08;
}*/


void dma\_out (unsigned short len, unsigned char rep)
{

	
	DMA.CTRL = 0;
	DMA.CTRL = DMA\_RESET\_bm;
	while ((DMA.CTRL & DMA\_RESET\_bm) != 0);
	// configure DMA controller
	DMA.CTRL = DMA\_CH\_ENABLE\_bm |  DMA\_DBUFMODE\_CH01\_gc;
	
	DMA.CH0.REPCNT = rep;
	DMA.CH0.CTRLB|=0x01;
	DMA.CH0.CTRLA = DMA\_CH\_BURSTLEN\_2BYTE\_gc | DMA\_CH\_SINGLE\_bm | DMA\_CH\_REPEAT\_bm;					// ADC result is 2 byte 12 bit word
	DMA.CH0.ADDRCTRL =	DMA\_CH\_SRCRELOAD\_TRANSACTION\_gc | DMA\_CH\_SRCDIR\_INC\_gc |			// reload source after every burst
	DMA\_CH\_DESTRELOAD\_BURST\_gc | DMA\_CH\_DESTDIR\_INC\_gc;	// reload dest after every transaction
	
		DMA.CH0.TRIGSRC = 0x26;//DMA\_CH\_TRIGSRC\_TCC0\_OVF\_gc;
		DMA.CH0.TRFCNT = len;	// always the number of bytes, even if burst length > 1
	
		  DMA.CH0.SRCADDR0 = ((uint16\_t)(&sine[0]) >> 0) & 0xFF;
		  DMA.CH0.SRCADDR1 = ((uint16\_t)(&sine[0]) >>  8) & 0xFF;
		  DMA.CH0.SRCADDR2 =0;
		  
		  DMA.CH0.DESTADDR0 = ((uint16\_t)(&DACB.CH1DATA) >> 0) & 0xFF;
		  DMA.CH0.DESTADDR1 = ((uint16\_t)(&DACB.CH1DATA) >> 8) & 0xFF;
		  DMA.CH0.DESTADDR2 = 0;
	
	DMA.CH0.CTRLA |= DMA\_CH\_ENABLE\_bm;
	
	
}

static void dac\_dma\_timer()
{
	sysclk\_enable\_module(SYSCLK\_PORT\_C, SYSCLK\_TC0);
	TCC0.PER = (sysclk\_get\_per\_hz() / RATE\_OF\_CONVERSION) - 1;

	// Configure event channel 0 to generate events upon T/C overflow.
	sysclk\_enable\_module(SYSCLK\_PORT\_GEN, SYSCLK\_EVSYS);
	EVSYS.CH0MUX = EVSYS\_CHMUX\_TCC0\_OVF\_gc;

	// Start the timer/counter.
	TCC0.CTRLA = TC\_CLKSEL\_DIV1\_gc;
	//TCC0.INTCTRLA=0x01;

	configure\_dac();
	dma\_out(64,0);
}	

int main(void)
{
	
	//uint8\_t           i = 0;
	PORTD.DIR=0x08;
	board\_init();
	sysclk\_init();
	sleepmgr\_init();
	
	
	// Initialize the dac configuration.
	
	configure\_pins();
	dac\_dma\_timer();
	
	
	// Configure timer/counter to generate events at sample rate.

			
	
	
	irq\_initialize\_vectors();
	cpu\_irq\_enable();
	/* Write samples to the DAC channel every time it is ready for new
	 * data, i.e., when it is done converting. Conversions are triggered by
	 * the timer/counter.
	 
	 */

	
	do {

	} while (1);
}

```

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
- ATxmega32A4U
- Industrial Automation
- Waveform Generation using DAC
