// spi.c

#include "main.h"
#include "spi.h"

#include <avr/signal.h>
#include <avr/io.h>


char loc_direction;
unsigned int spi_counter;

char* buffer;
unsigned int buffer_size;


SIGNAL (SIG_SPI)
{
	
	if (!block_complete)
	{	if (loc_direction)
		{	SPDR=*(buffer++);
			spi_counter++;
			block_complete=(spi_counter>=buffer_size);		
		}
		else
		{	*(buffer++)=SPDR;
			spi_counter++;
			block_complete=(spi_counter>=buffer_size);		
			if (!block_complete)
				SPDR=0xFF;		//write dummy byte

		}
		
	}
}



void spi_irq_enable()
{	SPCR|=0x80;
}



void spi_irq_disable()
{	SPCR&=0x7F;
}



void spi_init (char pol,char pha,char ds,char data_order,char master,char clock_pattern)
{	
	pol&=1;
	pha&=1;
	ds&=1;
	data_order&=1;
	master&=1;
	clock_pattern&=3;
	block_complete=1;

	
	if (master)
	{	//PORTB|=0x2C;			//disable /CS, all output signals high
		//PORTB&=0x07;
		DDRB&=0x0F;
		DDRB|=0b10110000;		//configure MISO,MOSI,CLK,/CS direction
//		if (pol)
//			PORTB|=0x20;	//set clk high
	}
	SPCR=0x40|(data_order<<5)|(master<<4)|(pol<<3)|(pha<<2)|clock_pattern;
	SPSR|=ds;
//	PORTB|=0x10;	//pullup at MISO

}



void spi_select(void)
{	PORTB&=~0x10;	//activate /CS
}



void spi_deselect(void)
{	PORTB|=0x10;	//release /CS
}


byte spi_master_transfer_byte(byte txd)
{	SPDR=txd;
	SPCR|=0x10; //enable master-mode
	while (!(SPSR&0x80));
	return SPDR;
}

void spi_off()
{	SPCR=0;
}

char spi_master_transfer_block (char direction,char* buff,word size)	//1=transmit,0=receive, interrupt function mandatory
{	char status;
	
	status=!block_complete;
	
	if (!status)
	{	spi_counter=0;
		buffer=buff;
		buffer_size=size;
		block_complete=0;
		loc_direction=direction;
		if (direction)
		{	SPDR=*(buffer++);
			spi_counter++;
		}
		else
		{	SPDR=0xFF;		//write dummy byte
		}
	}
	return status;
}
