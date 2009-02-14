/************************************************************************/
/*                                                                      */
/*                      Test Software UART                              */
/*                                                                      */
/*              Author: P. Dannegger                                    */
/*                                                                      */
/************************************************************************/
//			Target: ATtiny44
#include "main.h"
#include "suart.h"


void init( void )
{
  DDRA = 0;
  DDRB = 0;
  PORTA = 0xFF;
  PORTB = 0xFF;
  suart_init();
}


int main( void )
{
  init();
  sei();
  uputs( "Hallo Peter\n\r" );
  for(;;){
    _delay_ms( 5000 );
    while( kbhit() )
      uputchar( ugetchar());
  }
}
