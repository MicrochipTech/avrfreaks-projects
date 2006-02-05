#include "main.h"
#include "suart.h"


int main( void )
{
  suart_init();
  sei();
  sputs("Hallo Peter !\n\r" );

  for(;;){				// main loop
    sputchar( '-' );
    while( !kbhit() );			// wait until byte received
    sputchar( sgetchar() + 1 );		// sent byte + 1
  }
}
