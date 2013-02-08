/*
 * spi.c
 *
 * Created: 1/1/2013 4:24:23 PM
 *  Author: sasquach
 */ 

#include <avr/io.h>
#include "stdlib.h"
#include "config.h"
#include "spi.h"
#include <avr/interrupt.h>

uint8_t SPI_DATA_MISO[SPI_DATA_BUFFER_SIZE];
uint8_t SPI_DATA_MOSI[SPI_DATA_BUFFER_SIZE];
uint8_t SPI_DATA_COUNT;

void spi_master_init(void)
{
	SPI_SLAVE_SELECT_PORT |= (_BV(SPI_SLAVE_SELECT_SS));
	SPI_SLAVE_SELECT_DDR |= _BV(SPI_SLAVE_SELECT_SS);
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);
//	PORT_SPI |= (_BV(DD_MISO))  | (_BV(DD_SCK)); //| (_BV(DD_MOSI))
	SPI_SLAVE_SELECT_DDR |= _BV(SPI_SLAVE_SELECT_SS);

	
	SPI_DATA_MISO[14]=0xac;
	SPI_DATA_MISO[15]=0xdd;
//	for (uint8_t i=0;i<16;i++)
//	{
//	SPI_DATA_MOSI[i]=0xbd;
//	}	
	
}

void spi_slave_init(void)
{
	/* Set MISO output, all others input */
	DDR_SPI = (1<<DD_MISO);
	/* Enable SPI */
	SPCR = (1<<SPE)| (1<<SPIE);
//    PORT_SPI |= (_BV(DD_MOSI)) | (_BV(DD_SCK));
}

void spi_master(void)
{
	for (;;)
	{
		SPI_SLAVE_SELECT_PORT &= ~(_BV(SPI_SLAVE_SELECT_SS));
		SPI_SLAVE_SELECT_DDR |= _BV(SPI_SLAVE_SELECT_SS);
		SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);
		vTaskDelay(SPI_MASTER_DELAY_SHORT);

		for (uint8_t i=0; i < SPI_DATA_BUFFER_SIZE; i++ )
		{
			/* Start transmission */
			SPDR = (SPI_DATA_MOSI[i]);
			/* Wait for transmission complete */
			while ((SPSR & (_BV(SPIF))) == 0)
			{
			vTaskDelay(SPI_MASTER_DELAY_SHORT);
			}				
			SPI_DATA_MISO[((i-1) & SPI_DATA_BUFFER_MASK)]=SPDR;

		}		
		vTaskDelay((SPI_MASTER_DELAY_LONG/2));
		SPI_SLAVE_SELECT_PORT |= _BV(SPI_SLAVE_SELECT_SS);
		vTaskDelay((SPI_MASTER_DELAY_LONG/2));
	}	
}

void spi_slave(void)
{
	for (;;)
	{
		if ( (SPI_SLAVE_SELECT_PIN & (_BV(SPI_SLAVE_SELECT_SS)))== (_BV(SPI_SLAVE_SELECT_SS))) 
		{
			SPI_DATA_COUNT=0;	
		}		
		vTaskDelay(SPI_SLAVE_DELAY);
	}	
}

#ifdef SPI_SLAVE_
ISR (SPI_STC_vect)
{
	SPI_DATA_MOSI[SPI_DATA_COUNT]=SPDR;
	SPDR=SPI_DATA_MISO[SPI_DATA_COUNT];
	SPI_DATA_COUNT++;
	SPI_DATA_COUNT &= SPI_DATA_BUFFER_MASK;

}
#endif

#ifdef SPI_MASTER_
//ISR (SPI_STC_vect)
//{
	
//}
#endif