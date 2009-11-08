//*****************************************************************************
//
//      COPYRIGHT (c) ATMEL Norway, 1996-2001
//
//      The copyright to the document(s) herein is the property of
//      ATMEL Norway, Norway.
//
//      The document(s) may be used  and/or copied only with the written
//      permission from ATMEL Norway or in accordance with the terms and
//      conditions stipulated in the agreement/contract under which the
//      document(s) have been supplied.
//
//*****************************************************************************
//
//  File........: DATAFLASH.C
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: All AVRs with built-in HW SPI
//
//  Description.: Functions to access the Atmel AT45Dxxx dataflash series
//                Supports 512Kbit - 64Mbit
//
//  Revisions...:
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20011017 - 1.00 - Beta release                                  -  RM
//  20011017 - 0.10 - Generated file                                -  RM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20040121          added compare and erase function              - M.Thomas
//  20090718		  add read/write helper func. to hide block 
//					  addressing of flash.							- J.Landowski
//
//*****************************************************************************

/* 
   remark mthomas: If you plan to use the dataflash functions in own code
   for (battery powered) devices: disable the "chip select" after accessing
   the Dataflash. The current draw with cs enabled is "very" high. You can
   simply use the macro DF_CS_inactive already defined by Atmel after every
   DF access.

   The coin-cell battery on the Butterfly is not a reliable power-source if data
   in the flash-array should be changed (write/erase).
   See the Dataflash datasheet for the current needed during write-accesses.
*/

// Includes
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "main.h"
#include "dataflash.h"

// Constants
//Look-up table for these sizes ->  512k, 1M, 2M, 4M, 8M, 16M, 32M, 64M
const uint8_t DF_pagebits[] PROGMEM ={  9,  9,  9,  9,  9,  10,  10,  11};	    //index of internal page address bits

//Look-up table for these sizes ->  512k, 1M,  2M,  4M,  8M, 16M, 32M, 64M
const uint16_t DF_pagesize[] PROGMEM ={264,264, 264, 264, 264, 528, 528, 1056};	//index of pagesizes

// Globals
unsigned char PageBits = 0;
unsigned int  PageSize = 0;

/*****************************************************************************
*
*	Function name : DF_SPI_init
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Sets up the HW SPI in Master mode, Mode 3
*					Note -> Uses the SS line to control the DF CS-line.
*
******************************************************************************/
void DF_SPI_init (void)
{

	PRR &= ~(1<<PRSPI); // Enable SPI

	PORTB |= (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0);
	DDRB |= (1<<DDB2) | (1<<DDB1) | (1<<DDB0);				//Set MOSI, SCK AND SS as outputs
	
	SPSR = (1<<SPI2X);                                      //SPI double speed settings
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL);	//Enable SPI in Master mode, mode 3, Fosc/4

	Read_DF_status();
}

/*****************************************************************************
*
*	Function name : DF_SPI_RW
*
*	Returns :		Byte read from SPI data register (any value)
*
*	Parameters :	Byte to be written to SPI data register (any value)
*
*	Purpose :		Read and writes one byte from/to SPI master
*
******************************************************************************/
unsigned char DF_SPI_RW (unsigned char output)
{
	unsigned char input;
	cli();
	SPDR = output;							//put byte 'output' in SPI data register
	while(!(SPSR & 0x80));					//wait for transfer complete, poll SPIF-flag
	input = SPDR;							//read value in SPI data reg.
	sei();
	return input;							//return the byte clocked in from SPI slave
}


/*****************************************************************************
*
*	Function name : Read_DF_status
*
*	Returns :		One status byte. Consult Dataflash datasheet for further
*					decoding info
*
*	Parameters :	None
*
*	Purpose :		Status info concerning the Dataflash is busy or not.
*					Status info concerning compare between buffer and flash page
*					Status info concerning size of actual device
*
*   mt: the 'if' marked with 'mt 200401' is a possible optimisation
*   if only one type of Dataflash is used (like on the Butterfly).
*   If the uC controls different types of dataflash keep the PageBits
*   and PageSize decoding in this function to avoid problems.
******************************************************************************/
unsigned char Read_DF_status (void)
{
	unsigned char result,index_copy;
	
	DF_CS_inactive;							//make sure to toggle CS signal in order
	DF_CS_active;							//to reset dataflash command decoder
	result = DF_SPI_RW(StatusReg);			//send status register read op-code
	result = DF_SPI_RW(0x00);				//dummy write to get result

	index_copy = ((result & 0x38) >> 3);	//get the size info from status register
	PageBits   = pgm_read_byte(&DF_pagebits[index_copy]);	//get number of internal page address bits from look-up table
	PageSize   = pgm_read_word(&DF_pagesize[index_copy]);   //get the size of the page (in bytes)

	return result;							//return the read status register value
}



/*****************************************************************************
*
*	Function name : Buffer_Read
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be read
*					*BufferPtr	->	address of buffer to be used for read bytes
*
*	Purpose :		Reads one or more bytes from one of the dataflash
*					internal SRAM buffers, and puts read bytes into
*					buffer pointed to by *BufferPtr
*
******************************************************************************/
void Buffer_Read(unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	

	if (1 == BufferNo)							//read byte(s) from buffer 1
	{
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00);						//don't cares
		for( i=0; i<No_of_bytes; i++)
		{
			*(BufferPtr) = DF_SPI_RW(0x00);		//read byte and put it in AVR buffer pointed to by *BufferPtr
			BufferPtr++;						//point to next element in AVR buffer
		}
	}	
	else
	if (2 == BufferNo)							//read byte(s) from buffer 2
	{
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		DF_SPI_RW(0x00);						//don't cares
		for( i=0; i<No_of_bytes; i++)
		{
			*(BufferPtr) = DF_SPI_RW(0x00);		//read byte and put it in AVR buffer pointed to by *BufferPtr
			BufferPtr++;						//point to next element in AVR buffer
		}
	}


	DF_CS_inactive;

}


/*****************************************************************************
*
*	Function name : Buffer_Write
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*					No_of_bytes	->	Number of bytes to be written
*					*BufferPtr	->	address of buffer to be used for copy of bytes
*									from AVR buffer to dataflash buffer 1 (or 2)
*
*	Purpose :		Copies one or more bytes to one of the dataflash
*					internal SRAM buffers from AVR SRAM buffer
*					pointed to by *BufferPtr
*
******************************************************************************/
void Buffer_Write(unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	

	if (1 == BufferNo)							//write byte(s) to buffer 1
	{
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		for( i=0; i<No_of_bytes; i++)
		{
			DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to dataflash buffer 1 location
			BufferPtr++;						//point to next element in AVR buffer
		}
	}
	else if (2 == BufferNo)							//write byte(s) to buffer 2
	{
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code
		DF_SPI_RW(0x00);						//don't cares
		DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		for( i=0; i<No_of_bytes; i++)
		{
			DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to dataflash buffer 2 location
			BufferPtr++;						//point to next element in AVR buffer
		}
	}


	DF_CS_inactive;

}


/*****************************************************************************
*
*	Function name : Page_To_Buffer
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of page to be transferred to buffer
*
*	Purpose :		Transfers a page from flash to dataflash SRAM buffer
*					
******************************************************************************/
void Page_To_Buffer (unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_inactive;												//make sure to toggle CS signal in order
	DF_CS_active;												//to reset dataflash command decoder

	
	if (1 == BufferNo)											//transfer flash page to buffer 1
	{
		DF_SPI_RW(FlashToBuf1Transfer);							//transfer to buffer 1 op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
	else	
	if (2 == BufferNo)											//transfer flash page to buffer 2
	{
		DF_SPI_RW(FlashToBuf2Transfer);							//transfer to buffer 2 op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
	
	
	DF_CS_inactive;												//initiate the transfer
	DF_CS_active;
	
	while(!(Read_DF_status() & 0x80));		//monitor the status register, wait until busy-flag is high

}


/*****************************************************************************
*
*	Function name : Buffer_To_Page
*
*	Returns :		None
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of flash page to be programmed
*
*	Purpose :		Transfers a page from dataflash SRAM buffer to flash
*					
******************************************************************************/
void Buffer_To_Page (unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_inactive;												//make sure to toggle CS signal in order
	DF_CS_active;												//to reset dataflash command decoder

		
	if (1 == BufferNo)											//program flash page from buffer 1
	{
		DF_SPI_RW(Buf1ToFlashWE);								//buffer 1 to flash with erase op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}
	else	
	if (2 == BufferNo)											//program flash page from buffer 2
	{
		DF_SPI_RW(Buf2ToFlashWE);								//buffer 2 to flash with erase op-code
		DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
		DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
		DF_SPI_RW(0x00);										//don't cares
	}

	
	DF_CS_inactive;												//initiate flash page programming
	DF_CS_active;
	
	while(!(Read_DF_status() & 0x80));		//monitor the status register, wait until busy-flag is high										

}

/*****************************************************************************
*
*	Function name : DF_write
*
*	Returns :		None
*
*	Parameters :	ulAddress	->	DF address to write to
*					ucData		->	Byte to write
*
*	Purpose :		Helper func. to permit writing to the dataflash as a byte 
*					addressable memory with a linear address space.
*					
******************************************************************************/

static unsigned int lastWAdr = 0;
static unsigned int lastRAdr = 0;

// Byte write/read to/from the dataflash. Only uses the first 256 bytes of each page
// to make addressing linear without complexity. This, of course,
// leaves the last 8 bytes of each page unused.  

void DF_write(uint32_t ulAddress, uint8_t ucData)
{
	unsigned int bufferAdr;
	unsigned int wPageAdr;

	bufferAdr = (unsigned int)(ulAddress & 0x00FF);
	wPageAdr  = (unsigned int)((ulAddress >> 8) & 0x07FF);  

	if( lastWAdr != wPageAdr )
	{
		Buffer_To_Page(1, lastWAdr);
		lastWAdr = wPageAdr;
	}
	
	Buffer_Write(1, bufferAdr, 1, &ucData);
}


/*****************************************************************************
*
*	Function name : DF_read
*
*	Returns :		Byte read from data flash
*
*	Parameters :	ulAddress	->	DF address to write to
*
*	Purpose :		Helper func. to permit reading from the dataflash as a byte 
*					addressable memory with a linear address space.
*					
******************************************************************************/
unsigned char DF_read(uint32_t ulAddress)
{
	unsigned int bufferAdr;
	unsigned int rPageAdr;
	unsigned char ucData;

	bufferAdr = (unsigned int)(ulAddress & 0x00FF);
	rPageAdr  = (unsigned int)((ulAddress >> 8) & 0x07FF);
	
	if( (lastRAdr != rPageAdr || gNewSession) )
	{
		Page_To_Buffer(2, rPageAdr);
		lastRAdr = rPageAdr;
		gNewSession = FALSE;
	}

	Buffer_Read(2, bufferAdr, 1, &ucData);

	return ucData;

}
// Flush the data flash buffer to the flash memory
void DF_flush()
{
	Buffer_To_Page(1, lastWAdr);	
}

// *****************************[ End Of DATAFLASH.C ]*************************
