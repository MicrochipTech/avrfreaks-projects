#include "main.h"


#define	SRX_SIZE	100		// buffer size

#define START_DELAY	50		// cycle delay after start,
					// calculated from listing

u8 srx_buff[SRX_SIZE];
u8 srx_in;
u8 srx_out;
u8 srx_data;
u8 srx_state;
u8 prescale;


u8 suart_init( u16 baud )
{
  u8 baudl;
  u8 baudh;
  u8 i = 1<<CTC1;			// clear on compare with OCR1C
  u8 delay = START_DELAY;

  if( baud < 2*START_DELAY )
    return 1;				// error, baudrate too fast

  do{
    i++;
    delay >>= 1;
    baudl = baud;
    baudh = baud >> 8;
    baud >>= 1;
  }while( baudh );

  prescale = i;
  TCCR1 = 0;
  GTCCR = 0;
  OCR1C = baudl;			// clear on compare value
  OCR1A = baud - delay;			// interrupt on 0.5 bit time
  TIMSK = 1<<OCIE1A;

  srx_in = 1;
  srx_out = 0;				// never reach in pointer

  PCMSK = 1<<RXD_INT;			// enable start detection pin
  GIMSK = 1<<PCIE;
  return 0;				// success
}


u8 sgetchar( void )
{
  u8 idx;
  u8 data;

  idx = srx_out + 1;
  if( idx >= SRX_SIZE )
    idx = 0;

  while( idx == vu8(srx_in) );	      	// until at least one byte in

  data = srx_buff[idx];			// get byte
  srx_out = idx;			// advance out pointer

  return data;
}


ISR( PCINT0_vect )
{
  if( get_rxd() == 0 ){			// if start bit
    TCNT1 = 0;				// interrupt after 0.5 bit time
    TCCR1 = prescale;			// start T1
    PCMSK = 0;				// disable start detection
    srx_state = 10;
  }
}


ISR( TIM1_COMPA_vect )
{
  u8 i;

  srx_state--;
  switch( srx_state ){

    default:	i = srx_data >> 1;	      	// LSB first
		if( get_rxd() == 1 )
		  i |= 0x80;			// data bit = 1
		srx_data = i;
		break;

    case 0:	PCMSK = 1<<RXD_INT;		// enable start detect
		TCCR1 = 0;			// stop T1
		if( get_rxd() == 1 ){		// Stop bit valid
		  if( srx_in != srx_out ){	// no buffer overflow
		    srx_buff[srx_in] = srx_data;
		    srx_in++;			// advance in pointer
		    if( srx_in >= SRX_SIZE )	// roll over
		      srx_in = 0;
		  }
		}
		break;

    case 9:     if( get_rxd() == 1 ){		// no valid start bit
		  PCMSK = 1<<RXD_INT;		// enable start detect
		  TCCR1 = 0;			// stop T1
		}
		break;
  }
}
