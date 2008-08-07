/************************************************************************/
/*                                                                      */
/*                      Interrupt driven buffered UART                  */
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/
#include "main.h"
#include "uart0.h"


static u8 rx_buff[RX0_SIZE];
static u8 rx_in;
static u8 rx_out;
static u8 tx_buff[TX0_SIZE];
static u8 tx_in;
static u8 tx_out;


#define ROLLOVER( x, max )	x = ++x >= max ? 0 : x
					// count up and wrap around

void init_uart0( u16 bauddivider )
{
  UBRR0H = bauddivider >> 8;
  UBRR0L = bauddivider;			// set baud rate
  UCSR0A = 0;				// no U2X, MPCM
  UCSR0C = 1<<UCSZ01^1<<UCSZ00		// 8 Bit
#ifdef URSEL0
	   ^1<<URSEL0			// if UCSR0C shared with UBRR0H
#endif
	   ;
  UCSR0B = 1<<RXEN0^1<<TXEN0^		// enable RX, TX
	   1<<RXCIE0;			// enable RX interrupt
  rx_in = 0;				// set buffers empty
  rx_out = 0;
  tx_in = 0;
  tx_out = 0;
}


u8 ukbhit0( void )
{
  return rx_out ^ vu8(rx_in);		// rx_in modified by interrupt !
}


u8 ugetchar0( void )
{
  u8 data;

  while( !ukbhit0() );			// until at least one byte in
  data = rx_buff[rx_out];		// get byte
  ROLLOVER( rx_out, RX0_SIZE );
  URX0_IEN = 1;				// enable RX interrupt
  return data;
}


ISR( USART0_RX_vect )
{
  u8 i = rx_in;

  ROLLOVER( i, RX0_SIZE );
  if( i == rx_out ){			// buffer overflow
    URX0_IEN = 0;			// disable RX interrupt
    return;
  }
  rx_buff[rx_in] = UDR0;
  rx_in = i;
}


ISR( USART0_UDRE_vect )
{
  if( tx_in == tx_out ){		// nothing to sent
    UTX0_IEN = 0;			// disable TX interrupt
    return;
  }
  UDR0 = tx_buff[tx_out];
  ROLLOVER( tx_out, TX0_SIZE );
}


u8 utx0_ready( void )
{
  u8 i = tx_in;

  ROLLOVER( i, TX0_SIZE );
  return vu8(tx_out) ^ i;		// 0 = busy
}


void uputchar0( u8 c )
{
  u8 i = tx_in;

  ROLLOVER( i, TX0_SIZE );
  tx_buff[tx_in] = c;
  while( i == vu8(tx_out));		// until at least one byte free
					// tx_out modified by interrupt !
  tx_in = i;
  UTX0_IEN = 1;                         // enable TX interrupt
}


void uputs0_( u8 *s )
{
  while( *s )
    uputchar0( *s++ );
}
