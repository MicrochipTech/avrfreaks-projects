/************************************************************************/
/*                                                                      */
/*                      Software UART + FIFO (TX only)			*/
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/
#include "main.h"
#include "uart.h"


#ifdef TX_INVERT
#define	TX_HIGH		0
#else
#define	TX_HIGH		1
#endif

#define	TX_LOW		!TX_HIGH
#define	TX_OUT		SBIT( TCCR1, COM1A0 )	

#define ROLLOVER( x, max )	x = ++x >= max ? 0 : x


static u8 tx_buff[TX0_SIZE];
static u8 tx_in;
static u8 tx_out;
static u8 tx_state;
static u8 tx_data;
					// count up and wrap around

void init_uart( u16 bauddiv )
{
  u8 prescaler = 1<<CTC1^1<<COM1A1^1<<CS10;

  while( bauddiv & 0xFF00 ){		// until compare value <= 8bit
    prescaler++;
    bauddiv >>= 1;
  }
  OCR1A = 0;
  OCR1C = bauddiv-1;
  TCCR1 = prescaler;
  TX_OUT = TX_HIGH;
  GTCCR = 1<<FOC1A;			// output high
  STX_DDR = 1;				// output enable
  TIMSK = 1<<OCIE1A;
  tx_state = 0;
  tx_in = 0;
  tx_out = 0;
}


ISR( TIM1_COMPA_vect )
{
  if( tx_state ){
    tx_state--;
    TX_OUT = TX_HIGH;
    if( tx_data & 1 )			// lsb first
      TX_OUT = TX_LOW;
    tx_data >>= 1;
    return;
  }
  if( tx_in != tx_out ){		// next byte to sent
    tx_data = tx_buff[tx_out];
    ROLLOVER( tx_out, TX0_SIZE );
    tx_state = 9;
    TX_OUT = TX_LOW;			// start bit
  }
}


void uputchar( u8 c )
{
  u8 i = tx_in;

  ROLLOVER( i, TX0_SIZE );
  tx_buff[tx_in] = ~c;			// complement for stop bit after data
  while( i == vu8(tx_out));		// until at least one byte free
					// tx_out modified by interrupt !
  tx_in = i;
}


void uputs_( u8 *s )
{
  while( *s )
    uputchar( *s++ );
}
