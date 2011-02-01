#include "adc.h"
#include "spi.h"

unsigned short read_sample(void)
/*Takes a sample using the configure SPI communication*/
{
	uint8_t data_high, data_low;
	
	CS_LOW; //Start reading sample
	data_high = spi_write(0); //Read first 5 bits
	
	data_high &= 0x1F;

	data_low = spi_write(0); //Read next 7 bits, bit 1 repeated at end
	
	CS_HIGH; //Stop reading samples
	
	return ((data_high << 7)|(data_low >> 1));
}
