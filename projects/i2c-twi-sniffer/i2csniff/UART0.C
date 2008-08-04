/************************************************************************/
/*                                                                      */
/*                      Software UART + FIFO (TX only)			*/
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/
#include "main.h"
#include "uart0.h"

#ifdef TX_INVERT
#define set_txd(x)	(STX = !x)
#else
#define set_txd(x)	(STX = x)
#endif

static u8 tx_buff[TX0_SIZE];
static u8 tx_in;
static u8 tx_out;
static u8 tx_state;
static u8 tx_data;

#define BIT_TIME	(XTAL / BAUD - 1)

#define ROLLOVER( x, max )	x = ++x >= max ? 0 : x
					// count up and wrap around

void init_uart0( void )
{
  set_txd(1);				// inactive
  STX_DDR = 1;				// output

  TCCR1 = 1<<CTC1^1<<CS10;		// clear on compare, XTAL / 1
  OCR1A = BIT_TIME;
  OCR1C = BIT_TIME;
  TIMSK = 1<<OCIE1A;
  tx_state = 0;
  tx_in = 0;
  tx_out = 0;
}


ISR( TIM1_COMPA_vect )
{
  switch( tx_state ){

    case 10: set_txd( 0 );			// start bit
	     break;

    default: if( !(tx_data & 1) ){
	       set_txd( 0 );			// data bit 0
	     }else{

    case 1:    set_txd( 1 );			// data bit 1 or stop
	     }
	     tx_data >>= 1;
	     break;

    case 0:  if( tx_in == tx_out ){		// nothing to sent
	       TIMSK = 0;			// disable TX interrupt
	     }else{
	       tx_state = 10;
	       tx_data = tx_buff[tx_out];
	       ROLLOVER( tx_out, TX0_SIZE );
	     }
	     return;
  }
  tx_state--;
}


void uputchar0( u8 c )
{
  u8 i = tx_in;

  ROLLOVER( i, TX0_SIZE );
  tx_buff[tx_in] = c;
  while( i == vu8(tx_out));		// until at least one byte free
					// tx_out modified by interrupt !
  tx_in = i;
  TIMSK = 1<<OCIE1A;			// enable TX interrupt
}


void uputs0_( u8 *s )
{
  while( *s )
    uputchar0( *s++ );
}
