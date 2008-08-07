/************************************************************************/
/*                                                                      */
/*                      UART test program                               */
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/
#include "main.h"
#include "uart0.h"


void delay_1ms( u16 i )
{
  do{
    _delay_ms( 1 );
  }while( --i );
}


int main( void )
{
  init_uart0( (u16)(XTAL / BAUD / 16 - 0.5) );
  sei();
  delay_1ms( 300 );
  uputs0("Hallo Peter\n\r");
  uputs0("UART buffer test\n\r");

  for(;;){
    if( !ukbhit0()){
      continue;
    }
    delay_1ms( 10000 );        			// wait 10s
						// to see rx buffer overflow
    do{
      uputchar0( ugetchar0());			// send without waiting
      if( !utx0_ready() ){			// if tx buffer full
	if( !ukbhit0()){
	  break;
	}
	uputchar0( '.' );			// to see tx buffer size
	do{
	  uputchar0( ugetchar0());		// send with busy waiting
	}while( ukbhit0());
      }
    }while( ukbhit0());
    uputchar0( '\n' );
    uputchar0( '\r' );
  }
}
