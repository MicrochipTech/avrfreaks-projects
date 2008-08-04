#include "main.h"
#include "uart0.h"
#include "i2csniff.h"


int main( void )
{
  init_uart0();
  init_i2c();
  sei();
  uputs0( "I2C-Sniffer:" LINEFEED );

  i2c_sniff();
  for(;;){}			// avoid warning (but never reached)
}
