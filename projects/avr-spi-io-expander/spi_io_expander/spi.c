#include <avr/io.h>

void SPI_MasterInit(void)
{
	//set MOSI and SCK output, all others input
	DDRB = (1<<PB0)|(1<<PB3)|(1<<PB5);	//use PB0 as latch for '595
	
	//enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void SPI_SlaveInit(void)
{
	//set MISO output, all others input
	DDRB = (1<<PB4);
	
	//enable SPI
	SPCR = (1<<SPE);
}

char SPI_SlaveReceive(void)
{
	//wait for reception complete
	while(!(SPSR & (1<<SPIF)));

	//return data register
	return SPDR;
}

void SPI_MasterTransmit(char cData)
{
	//start transmission
	SPDR = cData;
	
	//wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}
