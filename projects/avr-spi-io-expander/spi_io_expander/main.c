#include <avr/io.h>
#include <util/delay.h>

//this program utilizes the SPI interface and a '595 
//serial to parallel shifter register to 'create' 8 outputs
//controlled by the SPI

void SPI_MasterInit(void);
void SPI_MasterTransmit(char cData);

int main()
{
	unsigned char spi_data;

	SPI_MasterInit();	//initialize the SPI in master mode

	PORTB = 0x00;		//initialize Port B to zero

	spi_data = 0x00;

	while(1)
	{
	

		if(spi_data >= 0xFF)
		{
			spi_data = 0x00;

			PORTB |= 0x01;		//set latch

			SPI_MasterTransmit(spi_data);	//transmit the key press data via SPI

			PORTB &= ~(0x01);	//clear latch
		}
		else
		{
			spi_data++;
			
			PORTB |= 0x01;		//set latch

			SPI_MasterTransmit(spi_data);	//transmit the key press data via SPI	
			
			PORTB &= ~(0x01);	//clear latch
		}
		
		_delay_ms(5);

	}

	return(0);
}
