/*
,-----------------------------------------------------------------------------------------.
| io/dataflash
|-----------------------------------------------------------------------------------------
| this file implements the driver for a Atmel Dataflash (AT45DB041B)
| - 
|
| Author   : {{removed according to contest rules}}
|            -> circuitcellar.com avr design contest 2006
|            -> Entry #AT2616
|
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/
#include "dataflash.h"
#include "../main.h"
#include <util/delay.h>

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"


//initialise dataflash
// -> SPI must be initialised externally!
void dataflash_init(void){
	//set up port directions:
	DATAFLASH_DDR |= (1<<DATAFLASH_PIN_CS) | (1<<DATAFLASH_PIN_RST);
	
	#if DATAFLASH_DEBUG
	softuart_puts_progmem("DF  : dataflash init: ");
	#endif

	//SPI init (is done in enc28j60...
	#if 0 
		// initialize I/O
		//SPI init
		// initialize I/O
		PORTB |= (1<<7); //sck = hi
	
		//spi = output
		//mega8//DDRB |= (1<<2)|(1<<3)|(1<<5); //SS,MOSI,SCK = OUT
		DDRB |= (1<<4)|(1<<5)|(1<<7); //SS,MOSI,SCK = OUT
		DDRB &= ~(1<<6); //MISO = IN
	
		//SPI init:
		// - master mode
		// - positive clock phase
		// - msb first
		// - maximum SPI speed (fosc/2)
		// - enable spi
		SPCR = (0<<CPOL)|(1<<MSTR)|(0<<DORD)|(0<<SPR1)|(0<<SPR0)|(1<<SPE);
		SPSR = (1<<SPI2X);
	#endif

	//reset dataflash:
	DATAFLASH_RESET_LO();
	DATAFLASH_CS_LO();

	//skip any pending bits:
	for(unsigned char i=0; i<20; i++)
		dataflash_spi_writeread_byte(0x00);

	_delay_us(20); //min 10us
	DATAFLASH_CS_HI();
	_delay_us(1); //min 200ns		
	DATAFLASH_RESET_HI();

	//dataflash is now in SPI Mode 3
	#if DATAFLASH_DEBUG
	softuart_puts_progmem("done.");
	softuart_putnewline();
	#endif
}

void dataflash_dump(){
	for (int a=0; a<DATAFLASH_MAX_PAGE; a++){
		dataflash_copy_page_to_buffer(a, 0);
		for(int b=0; b<(256+8); b++){
			softuart_putc(dataflash_read_buffer(b,0));
		}
	}
}


//copy given buffer to flash page
void dataflash_copy_buffer_to_page(unsigned int page, unsigned char buffer){
	//wait for device ready
	dataflash_busy_wait();

	//select device again:
	DATAFLASH_CS_LO();

	//send buffer select cmd:
	if (buffer == 0)
		dataflash_spi_writeread_byte(DATAFLASH_BUFFER0_TO_MEM_WE);
	else
		dataflash_spi_writeread_byte(DATAFLASH_BUFFER1_TO_MEM_WE);

	//send 0000 0ppp pppp pppp 
	dataflash_spi_writeread_byte((page>>7)&0x0F);
	dataflash_spi_writeread_byte((page<<1)&0xFE);

	//send 8 dont care bits:
	dataflash_spi_writeread_byte(0x00);

	DATAFLASH_CS_HI();
}

//write len bytes to given page buffer starting at <byte> 
void dataflash_write_n_to_page_buffer(unsigned int byte, unsigned char buffer, unsigned char *data, unsigned int len){
	//wait for device ready
	dataflash_busy_wait();

	//select device again:
	DATAFLASH_CS_LO();

	//send buffer select cmd:
	if (buffer == 0)
		dataflash_spi_writeread_byte(DATAFLASH_WRITE_BUFFER0);
	else
		dataflash_spi_writeread_byte(DATAFLASH_WRITE_BUFFER1);

	//15 dont care + 9 adress bits:
	
	//send 8 dont care bits:
	dataflash_spi_writeread_byte(0x00);
	
	//send 0000 000b bbbb bbbb
	dataflash_spi_writeread_byte((byte>>8)&0x01);
	dataflash_spi_writeread_byte((byte)&0xFF);

	//send data
	while(len>0){
		dataflash_spi_writeread_byte((*data));
		*data++;
		len--;
	}

	//deselect device:
	DATAFLASH_CS_HI();
}


//write one byte to given page buffer
void dataflash_write_to_page_buffer(unsigned int byte, unsigned char buffer, unsigned char data){
	//wait for device ready
	dataflash_busy_wait();

	//select device again:
	DATAFLASH_CS_LO();

	//send buffer select cmd:
	if (buffer == 0)
		dataflash_spi_writeread_byte(DATAFLASH_WRITE_BUFFER0);
	else
		dataflash_spi_writeread_byte(DATAFLASH_WRITE_BUFFER1);

	//15 dont care + 9 adress bits:
	
	//send 8 dont care bits:
	dataflash_spi_writeread_byte(0x00);
	
	//send 0000 000b bbbb bbbb
	dataflash_spi_writeread_byte((byte>>8)&0x01);
	dataflash_spi_writeread_byte((byte)&0xFF);

	//send data
	dataflash_spi_writeread_byte(data);

	//deselect device:
	DATAFLASH_CS_HI();
}

//copy flash page to given buffer
void dataflash_copy_page_to_buffer(unsigned int page, unsigned char buffer){
	//wait for device ready
	dataflash_busy_wait();

	//select device again:
	DATAFLASH_CS_LO();

	//send buffer select cmd:
	if (buffer == 0)
		dataflash_spi_writeread_byte(DATAFLASH_MEM_TO_BUFFER0);
	else
		dataflash_spi_writeread_byte(DATAFLASH_MEM_TO_BUFFER1);

	//send 0000 pppp pppp ppp0 
	dataflash_spi_writeread_byte((page>>7)&0x0F);
	dataflash_spi_writeread_byte((page<<1)&0xFE);

	//send 8 dont care bits:
	dataflash_spi_writeread_byte(0x00);

	DATAFLASH_CS_HI();
}

//read one byte from buffer
unsigned char dataflash_read_buffer(unsigned int byte, unsigned char buffer){
	unsigned char data;

	//wait for device ready
	dataflash_busy_wait();

	//select device again:
	DATAFLASH_CS_LO();

	//send buffer select cmd:
	if (buffer == 0)
		dataflash_spi_writeread_byte(DATAFLASH_READ_BUFFER0);
	else
		dataflash_spi_writeread_byte(DATAFLASH_READ_BUFFER1);

	//15 dont care + 9 address + 8 dont care:

	//send address: 0000 0000 0000 000b bbbb bbbb 
	dataflash_spi_writeread_byte(0x00);
	dataflash_spi_writeread_byte((byte>>8)&0x01);
	dataflash_spi_writeread_byte((byte)&0xFF);

	//send one dummy byte
	dataflash_spi_writeread_byte(0x00);

	//now read the data
	data = dataflash_spi_writeread_byte(0x00);

	//deselect device
	DATAFLASH_CS_HI();

	//return data
	return (data);
}

//read n byte from buffer
void dataflash_read_n_to_buffer(unsigned char *buffer, unsigned char selbuf){
	//wait for device ready
	dataflash_busy_wait();

	//select device again:
	DATAFLASH_CS_LO();

	//send buffer select cmd:
	if (selbuf == 0)
		dataflash_spi_writeread_byte(DATAFLASH_READ_BUFFER0);
	else
		dataflash_spi_writeread_byte(DATAFLASH_READ_BUFFER1);

	//15 dont care + 9 address + 8 dont care:

	//send address: 0000 0000 0000 000b bbbb bbbb 
	dataflash_spi_writeread_byte(0x00);
	dataflash_spi_writeread_byte(0);//start with first byte
	dataflash_spi_writeread_byte(0);

	//send one dummy byte
	dataflash_spi_writeread_byte(0x00);

	//now read the data
	*buffer++ = dataflash_spi_writeread_byte(0x00);
	for(unsigned char c=0; c<255; c++){
		*buffer++ = dataflash_spi_writeread_byte(0x00);
	}

	//deselect device
	DATAFLASH_CS_HI();
}


//read 1 byte directly from flash:
unsigned char dataflash_read_flash(unsigned int page, unsigned int byte){
	unsigned char data;

	//wait for device ready
	dataflash_busy_wait();

	//select device again:
	DATAFLASH_CS_LO();

	//send page read cmd:
	dataflash_spi_writeread_byte(DATAFLASH_MEM_PAGE_READ);

	//send 24 address bits
	dataflash_spi_writeread_byte((page>>7) & 0x0F); //send 0000 pppp (4 MSB of page adress)
	dataflash_spi_writeread_byte(((page<<1)&0xFE) | ((byte>>8)&0x01) ); //send pppp pppb (7 LSB of page, 1 MSB of byte adress)
	dataflash_spi_writeread_byte(byte & 0xFF); //send bbbb bbbb (8 LSB of byte address)
	
	//now send 32 dummy dont care bits:
	dataflash_spi_writeread_byte(0x00);	//8
	dataflash_spi_writeread_byte(0x00); //16
	dataflash_spi_writeread_byte(0x00); //24
	dataflash_spi_writeread_byte(0x00); //32

	//now read the data
	data = dataflash_spi_writeread_byte(0x00);

	//deselect device
	DATAFLASH_CS_HI();

	//return data
	return (data);
}

//wait for dataflash ready flag
void dataflash_busy_wait(){
	DATAFLASH_CS_LO();

	//send status req cmd:
	dataflash_spi_writeread_byte(DATAFLASH_READ_STATUS);

	//wait for bit7 in status reg to be 1:
	while ( (dataflash_spi_writeread_byte(DATAFLASH_READ_STATUS)&0x80) != 0x80){
		//wait...
		//uart_putc('.');
	}
	DATAFLASH_CS_HI();
}

//send one byte over SPI and read one byte back
//device must be selected before !!!
unsigned char dataflash_spi_writeread_byte(unsigned char data){
	unsigned char res = 0;	
	//shift data out
	SPDR = data;

	//wait for completion
	while(!(SPSR & (1<<SPIF)));

	//shiftout 8 clocks, now we receive data:
	///SPDR = 0x00;
	//wait for completion
	///while(!(SPSR & (1<<SPIF)));

	// read data
	res = SPDR;

	return res;
}

