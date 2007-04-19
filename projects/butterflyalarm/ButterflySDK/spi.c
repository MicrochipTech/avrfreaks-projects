/** @file spi.c Spi driver. The driver doesn't employ interrupts.
*/

#include <avr/io.h>
#include <inttypes.h>

#include "c_ext.h"

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes the SPI driver. */
void SpiInit(void)
{
	// Set MOSI, SCK AND SS as outputs
	PORTB |= (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);
	DDRB |= (1<<DDB2) | (1<<DDB1) | (1<<DDB0);

	//SPI double speed settings
	SPSR = (1<<SPI2X);

	// Enable SPI in Master mode, mode 3, Fosc/4
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL);
}

/** Disables the SPI driver. It must be called when SPI communication is not
	needed to save power.
*/
void SpiDisable(void)
{
	SPCR = 0;
}

/** Read/writes a byte in the SPI bus. The same function is used for both
	reading and writing. When reading, the output parameter is meaningless.
	@param output The byte to write to the SPI bus
	@result The byte read from the SPI bus
*/
uint8_t SpiReadWrite(uint8_t output)
{
	uint8_t input;

	SPDR = output;
	while(!(SPSR & 0x80));
	input = SPDR;

	return input;
}


