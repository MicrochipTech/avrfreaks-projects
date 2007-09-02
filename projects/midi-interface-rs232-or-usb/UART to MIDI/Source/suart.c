/************************************************************************/
/*                                                                      */
/*                      Software UART using T1                          */
/*                                                                      */
/*              Author: P. Dannegger                                    */
/*                      danni@specs.de                                  */
/*                                                                      */
/*              Revised by L. Lisseck, original source                  */
/*                      garbled TIMSK and TIFR !                        */
/************************************************************************/

/** 
 * Specialities of this implementation:
 * - uses OCR1A, OCR1B and ICP, so Timer1 is fully bound
 * - RX must be the ICP-pin, TX must be the OCR1A-pin
 * - not much jitter, because interrupt jitter does not degrade overall jitter
 * - TX interrupt is activated all the time, so nearly constant perfomance loss
 * - Timer1 registers are used exclusively (not well masked, but maybe a speed advantage)
 */

#include <avr/io.h> 
#include <avr/interrupt.h>
#include "lutzmacro.h"
#include "hardware.h"
#include "suart.h"

#define BIT_TIME	(u16)((F_CPU + SUART_BAUD/2) / SUART_BAUD)

volatile u8 stx_count;
u8 stx_data;

volatile u8 srx_done;
u8 srx_data;
u8 srx_mask;
u8 srx_tmp;


void SUART_init( void )
{
  OCR1A = TCNT1 + 1;			// force first compare
  TCCR1A = 1<<COM1A1^1<<COM1A0;		// set OC1A high, T1 mode 0
  TCCR1B = 1<<ICNC1^1<<CS10;		// noise canceler, 1>0 transition,
					// CLK/1, T1 mode 0
  TIFR |= 1<<ICF1;			// clear pending interrupt
  
  TIMSK &= ~((1<<OCIE1B) | (1<<TOIE1) | (1<<TICIE1) | (1<<OCIE1A));
  TIMSK |= (1<<TICIE1 | 1<<OCIE1A);		// enable tx and wait for start

  stx_count = 0;			// nothing to sent
  srx_done = 0;				// nothing received
  STXDDR |= 1<<STX;			// TX output
}

SIGNAL( SIG_INPUT_CAPTURE1 )		// rx start
{
  OCR1B = ICR1 + (u16)(BIT_TIME * 1.5);	// scan 1.5 bits after start
  srx_tmp = 0;				// clear bit storage
  srx_mask = 1;				// bit mask
  TIFR |= 1<<OCF1B;			// clear pending interrupt
  if( !(SRXPIN & 1<<SRX)) {		// still low
    TIMSK &= ~((1<<OCIE1B) | (1<<TOIE1) | (1<<TICIE1) | (1<<OCIE1A));
    TIMSK |= 1<<OCIE1A^1<<OCIE1B;	// wait for first bit
  }  
}


SIGNAL( SIG_OUTPUT_COMPARE1B )
{
  u8 in = SRXPIN;			// scan rx line

  if( srx_mask ){
    if( in & 1<<SRX )
      srx_tmp |= srx_mask;
    srx_mask <<= 1;
    OCR1B += BIT_TIME;			// next bit slice
  }else{
    srx_done = 1;			// mark rx data valid
    srx_data = srx_tmp;			// store rx data
    TIFR |= 1<<ICF1;			// clear pending interrupt
    TIMSK &= ~((1<<OCIE1B) | (1<<TOIE1) | (1<<TICIE1) | (1<<OCIE1A));
    TIMSK |= 1<<TICIE1^1<<OCIE1A;        // enable tx and wait for start
  }
}


SIGNAL( SIG_OUTPUT_COMPARE1A )		// tx bit
{
  u8 dout;
  u8 count;

  OCR1A += BIT_TIME;			// next bit slice
  count = stx_count;

  if( count ){
    stx_count = --count;		// count down
    dout = 1<<COM1A1;			// set low on next compare
    if( count != 9 ){			// no start bit
      if( !(stx_data & 1) )		// test inverted data
	dout = 1<<COM1A1^1<<COM1A0;	// set high on next compare
      stx_data >>= 1;			// shift zero in from left
    }
    TCCR1A = dout;
  }
}


u8 SUART_getchar( void )			// get byte
{
  while( !srx_done );			// wait until byte received
  srx_done = 0;
  return srx_data;
}


void SUART_putchar( u8 val )			// send byte
{
  while( stx_count );			// until last byte finished
  stx_data = ~val;			// invert data for Stop bit generation
  stx_count = 10;			// 10 bits: Start + data + Stop
}


void SUART_puts( u8 *txt )			// send string
{
  while( *txt )
    SUART_putchar( *txt++ );
}


