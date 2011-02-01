#include <avr/io.h>
#include "spi.h"

void spi_master_init(void)
{
    /* Set MOSI and SCK output, all others input */
    SPI_DDR = (1<<MOSI_PIN)|(1<<SCK_PIN)|(1<<CS_PIN);
    /*Enable SPI, Master, set clock rate fck/16*/
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void spi_slave_init(void)
{
    /* Set MISO output, all others input */
    SPI_DDR = (1<<MOSI_PIN);
    /*Enable SPI */
    SPCR = (1<<SPE);
}

uint8_t spi_write(uint8_t data)
{
    /* Start transmission */
    SPDR = data;
    /*wait for transmission to end*/
    while (!(SPSR & (1<<SPIF)));
	return SPDR;
}
