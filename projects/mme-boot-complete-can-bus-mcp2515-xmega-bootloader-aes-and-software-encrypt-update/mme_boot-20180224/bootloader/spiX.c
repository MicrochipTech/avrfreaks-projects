/*
 * spi.c
 *
 * Created: 12/30/2014 1:53:09 AM
 *  Author: Marko Mlakar
 */ 

#include <avr/io.h>
#include <compat/deprecated.h>
#include <util/delay.h>

uint8_t spi_transfer(uint8_t data)
{
	SPIC.DATA = data;
	while(!(SPIC.STATUS & SPI_IF_bm));	
	return SPIC.DATA;
}

void spi_mode(unsigned char mode, unsigned char prescaler)
{
	SPIC.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | mode | prescaler;	
}

void spi_init()
{
	/* SPI */
	sbi(PORTC_DIR, 4); // SS - OUTPUT
	sbi(PORTC_DIR, 5); // MOSI - OUTPUT
	cbi(PORTC_DIR, 6); // MISO - INPUT
	sbi(PORTC_DIR, 7); // SCK - OUTPUT	
	
	spi_mode(SPI_MODE_0_gc, SPI_PRESCALER_DIV4_gc);
}
