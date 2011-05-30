#include <avr/io.h>
#include "spi_com.h"


void SPI_MasterConfig(void)
{
/* Set MOSI and SCK output, all others input */
DDRB = (1<<DDB2)|(1<<DDB1)|(1<<DDB0);
/* Enable SPI, Master*/
SPCR = (1<<SPE)|(1<<MSTR);
/*set clock rate fck/2 */
SPSR |=(1<<SPI2X);
}

void SPI_send(unsigned char cData)
{
/* Start transmission */
SPDR = cData;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF))){;}
}


void set_xlat(void)
	{
	PORTB |= _BV(PB0);
	}

void clear_xlat(void)
	{
	PORTB &= ~(_BV(PB0));
	}
	
void set_blank(void){
	PORTB |=1<PB5;	
}	

void clear_blank(void){
	PORTB &=~(1<PB5);	
	
}
void togle_blank(void){
	PORTB ^=(1<<PB5);
	
}