
#include <avr/io.h>

void spi_write( uint8_t b )
{
  SPDR = b;
  while( !(SPSR & _BV(SPIF)) );
}

uint8_t spi_read( void )
{
  spi_write(0xFF);
  return SPDR;
}

void spi_init( void )
{
  // set pin modes
  DDRB  |= _BV(PINB2)|_BV(PINB3)|_BV(PINB5) ; // SS, MOSI, CLK : Output
  PORTB |= _BV(PINB2); // disable SPI slave (SS High)
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
  // SPE  1=Enable SPI
  // MSTR 1=Master
  // DORD 0=MSB first
  // CPOL=0
  // CPHA=0
  // SPR clock rate 00=F/4  01=F/16  10=F/64  11=F/128
  SPSR = 0;
  // SPI2X = 0 = clear double speed
}

void spi_stop( void )
{
  PORTB &= ~(_BV(PINB2)); // Dsable SPI slave (SS High)
}

void spi_max( void ) /* Set SPI to maximum speed */
{
  // SPR clock rate 00=F/4  01=F/16 10=F/64 11=F/128
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
  // Enable SPI, Master, clock rate F/32
  SPSR = _BV(SPI2X);  // Set double speed  => F*2 = F/32
}
