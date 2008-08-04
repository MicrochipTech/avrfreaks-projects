#include "main.h"
#include "uart.h"
#include "i2csniff.h"


int main( void )
{
  init_uart( XTAL / BAUD );
  init_i2c();
  sei();
  uputs( "I2C-Sniffer:" LINEFEED );
  i2c_sniff();
  for(;;){}			// avoid warning (but never reached)
}
