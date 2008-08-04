/************************************************************************/
/*                                                                      */
/*                      I2C Sniffer					*/
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/
#include "main.h"
#include "uart0.h"
#include "i2csniff.h"


void init_i2c( void )
{
  SCL = 1;
  SCL_DDR = 1;				// force busy waiting
  SDA = 1;
  SDA_DDR = 0;				// listen only, no ACK, no data
  USICR = 1<<USIWM1^1<<USIWM0;		// I2C, no counter
}


void putnibble( u8 j )
{
  j &= 0x0F;
  if( j > 9 )
    j += 'A' - '0' - 10;
  uputchar0( j + '0' );
}


void i2c_sniff( void )
{
  u8 get_ack = 0;
  u8 i2cdat;

  for(;;){
    if( USISR & 1<<USISIF ){		// START received
      while( SCL_PIN );			// wait until SCL = low
      USICR = 1<<USIWM1^1<<USIWM0^1<<USICS1^1<<USICS0;
					// I2C, get data on neg. edge
      USISR = 1<<USISIF^1<<USIOIF^(0x0F & -16);
					// clear USISIF, count 16 edges
      get_ack = 0;
      uputchar0( START );
    }
    if( USISR & 1<<USIOIF ){		// Data / ACK received
      i2cdat = USIDR;
      if( ++get_ack & 1 ){		// toggle read data / ACK
	USISR = 1<<USIOIF^(0x0F & -2);	// count 2 edges
	putnibble( i2cdat >> 4 );
	putnibble( i2cdat );
      }else{
	USISR = 1<<USIOIF^(0x0F & -16);	// count 16 edges
	if( i2cdat & 1 )
	  uputchar0( NACK );
	else
	  uputchar0( ACK );
      }
    }
    if( USISR & 1<<USIPF ){		// STOP received
      USICR = 1<<USIWM1^1<<USIWM0;	// I2C, no counter
      USISR = 1<<USIPF;			// clear USISIF
      uputchar0( STOP );
      uputs0( LINEFEED );
    }
  }
}
