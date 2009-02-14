/************************************************************************/
/*                                                                      */
/*                      Software UART with FIFO, using T1               */
/*                                                                      */
/*              Author: P. Dannegger                                    */
/*                                                                      */
/************************************************************************/
//				Target: ATtiny44
#include "main.h"
#include "suart.h"


#define BIT_TIME	(u16)(XTAL * 1.0 / BAUD + 0.5)

#define	TX_HIGH		(1<<COM1A1^1<<COM1A0)
#define	TX_LOW		(TX_HIGH^1<<COM1A0)
#define	TX_OUT		TCCR1A		// use compare output mode

#define ROLLOVER( x, max )	x = ++x >= max ? 0 : x
					// count up and wrap around

static u8 stx_buff[STX_SIZE];
static u8 stx_in;
static u8 stx_out;
static u8 stx_data;
static u8 stx_state;

static u8 srx_buff[SRX_SIZE];
static u8 srx_in;
static u8 srx_out;
static u8 srx_data;
static u8 srx_state;


void suart_init( void )
{
  OCR1A = BIT_TIME - 1;
  TCCR1A = TX_HIGH;			// set OC1A high, T1 mode 4
  TCCR1B = 1<<ICNC1^1<<WGM12^1<<CS10;	// noise canceler, 1-0 transition,
					// CLK/1, T1 mode 4 (CTC)
  TCCR1C = 1<<FOC1A;
  stx_state = 0;
  stx_in = 0;
  stx_out = 0;
  srx_in = 0;
  srx_out = 0;
  STXD_DDR = 1;				// output enable
  TIFR1 = 1<<ICF1;			// clear pending interrupt
  TIMSK1 = 1<<ICIE1^1<<OCIE1A;		// enable tx and wait for start
}


u8 kbhit( void )			// check, if rx buffer not empty
{
  return srx_out ^ vu8(srx_in);
}


u8 ugetchar( void )			// wait until byte received
{
  u8 data;

  while( !kbhit());		     	// until at least one byte in
  data = srx_buff[srx_out];		// get byte
  ROLLOVER( srx_out, SRX_SIZE );
  return data;
}


void uputchar( u8 c )			// transmit byte
{
  u8 i = stx_in;

  ROLLOVER( i, STX_SIZE );
  stx_buff[stx_in] = ~c;		// complement for stop bit after data
  while( i == vu8(stx_out));		// until at least one byte free
					// stx_out modified by interrupt !
  stx_in = i;
}


void uputs_( u8 *s )			// transmit string from SRAM
{
  while( *s )
    uputchar( *s++ );
}

/******************************	Interrupts *******************************/

ISR( TIM1_CAPT_vect )				// start detection
{
  s16 i = ICR1 - BIT_TIME / 2;			// scan at 0.5 bit time

  OPTR18					// avoid disoptimization
  if( i < 0 )
    i += BIT_TIME;				// wrap around
  OCR1B = i;
  srx_state = 10;
  TIFR1 = 1<<OCF1B;				// clear pending interrupt
  if( SRXD_PIN == 0 )				// still low
    TIMSK1 = 1<<OCIE1A^1<<OCIE1B;		// wait for first bit
}


ISR( TIM1_COMPB_vect )				// receive data bits
{
  u8 i;

  switch( --srx_state ){

    case 9:  if( SRXD_PIN == 0 )		// start bit valid
	       return;
	     break;

    default: i = srx_data >> 1;	      		// LSB first
	     if( SRXD_PIN == 1 )
	       i |= 0x80;			// data bit = 1
	     srx_data = i;
	     return;

    case 0:  if( SRXD_PIN == 1 ){		// stop bit valid
	       i = srx_in;
	       ROLLOVER( i, SRX_SIZE );
	       if( i != srx_out ){		// no buffer overflow
		 srx_buff[srx_in] = srx_data;
		 srx_in = i;			// advance in pointer
	       }
	     }
	     TIFR1 = 1<<ICF1;			// clear pending interrupt
  }
  TIMSK1 = 1<<ICIE1^1<<OCIE1A;			// enable next start
}


ISR( TIM1_COMPA_vect )				// transmit data bits
{
  if( stx_state ){
    stx_state--;
    TX_OUT = TX_HIGH;
    if( stx_data & 1 )				// lsb first
      TX_OUT = TX_LOW;
    stx_data >>= 1;
    return;
  }
  if( stx_in != stx_out ){			// next byte to sent
    stx_data = stx_buff[stx_out];
    ROLLOVER( stx_out, STX_SIZE );
    stx_state = 9;				// 8 data bits + stop
    TX_OUT = TX_LOW;				// start bit
  }
}
