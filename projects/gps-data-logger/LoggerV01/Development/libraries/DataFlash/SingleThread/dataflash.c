/******** Example Code : Accessing Atmel AT45Dxxx dataflash on STK500 *******

Device      : 	AT90S8515

File name   : 	dataflash.c

Description : 	Functions to access the Atmel AT45Dxxx dataflash series
      				  Supports 512Kbit - 64Mbit
                 
Last change:    16 Aug 2001   AS
 
****************************************************************************/


#include <INA90.H>
#include <io.h>
#include "dataflash.h" 

// Constants

//Look-up table for these sizes ->  512k, 1M, 2M, 4M, 8M, 16M, 32M, 64M
prog_uint8_t DF_pagebits[]  ={  9,  9,  9,  9,  9,  10,  10,  11};	//index of internal page address bits

// Globals
unsigned char PageBits;

// Functions

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
void DFSPIInit (void)
{
	PORTB |= (1<<PORTB4);										//Set SS high		
	DDRB  |= (1<<PB4) | (1<<PB5)| (1<<PB7);					//Set SS, MOSI and SCK as outputs
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL);	//Enable SPI in Master mode, mode 3
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
static unsigned char DF_SPI_RW (unsigned char output)
{
	unsigned char input;
	
	SPDR = output;							//put byte 'output' in SPI data register
	while(!(SPSR & 0x80));					//wait for transfer complete, poll SPIF-flag
	input = SPDR;							//read value in SPI data reg.
	
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
******************************************************************************/
unsigned char DFReadStatus (void)
{
	unsigned char result,index_copy;
	
	DF_CS_inactive;							//make sure to toggle CS signal in order
	DF_CS_active;							//to reset dataflash command decoder
	result = DF_SPI_RW(StatusReg);			//send status register read op-code
	result = DF_SPI_RW(0x00);				//dummy write to get result
	
	index_copy = ((result & 0x38) >> 3);	//get the size info from status register
//	PageBits   = DF_pagebits[index_copy];	//get number of internal page address bits from look-up table

PageBits = pgm_read_byte_near(DF_pagebits+index_copy);

	return result;							//return the read status register value
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
void DFPageToBuffer (unsigned int PageAdr, unsigned char BufferNo)
{
	DF_CS_inactive;												//make sure to toggle CS signal in order
	DF_CS_active;												//to reset dataflash command decoder
	
	if (1 == BufferNo)											//transfer flash page to buffer 1
		DF_SPI_RW(FlashToBuf1Transfer);							//transfer to buffer 1 op-code
	else
		DF_SPI_RW(FlashToBuf2Transfer);							//transfer to buffer 2 op-code
	
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
	DF_SPI_RW(0x00);										//don't cares
	
	DF_CS_inactive;												//initiate the transfer
	DF_CS_active;
	
	while(!(DFReadStatus() & 0x80));							//monitor the status register, wait until busy-flag is high
}



/*****************************************************************************
*
*	Function name : Buffer_Read_Byte
*
*	Returns :		One read byte (any value)
*
*	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
*					IntPageAdr	->	Internal page address
*
*	Purpose :		Reads one byte from one of the dataflash
*					internal SRAM buffers
*
******************************************************************************/
unsigned char DFBufferReadByte (unsigned char BufferNo, unsigned int IntPageAdr)
{
	unsigned char data=0;
	
	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	
	if (1 == BufferNo)							//read byte from buffer 1
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
	else
		DF_SPI_RW(Buf2Read);					//buffer 2 read op-code

	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	DF_SPI_RW(0x00);						//don't cares
	data = DF_SPI_RW(0x00);					//read byte
	
	
	return data;								//return the read data byte
}



/*****************************************************************************
*
*	Function name : Buffer_Read_Str
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
void DFBufferReadStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	
	if (1 == BufferNo)							//read byte(s) from buffer 1
		DF_SPI_RW(Buf1Read);					//buffer 1 read op-code
	else
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
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..



/*****************************************************************************
*
*	Function name : Buffer_Write_Enable
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to start writing from
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*					
*	Purpose :		Enables continous write functionality to one of the dataflash buffers
*					buffers. NOTE : User must ensure that CS goes high to terminate
*					this mode before accessing other dataflash functionalities 
*
******************************************************************************/
void DFBufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr)
{
	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	
	if (1 == BufferNo)							//write enable to buffer 1
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code

	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
}



/*****************************************************************************
*
*	Function name : Buffer_Write_Byte
*
*	Returns :		None
*
*	Parameters :	IntPageAdr	->	Internal page address to write byte to
*					BufferAdr	->	Decides usage of either buffer 1 or 2
*					Data		->	Data byte to be written
*
*	Purpose :		Writes one byte to one of the dataflash
*					internal SRAM buffers
*
******************************************************************************/
void DFBufferWriteByte (unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
	
	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	
	if (1 == BufferNo)							//write byte to buffer 1
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code

	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	DF_SPI_RW(Data);						//write data byte	
}


/*****************************************************************************
*
*	Function name : Buffer_Write_Str
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
void DFBufferWriteStr (unsigned char BufferNo, unsigned int IntPageAdr, unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	DF_CS_inactive;								//make sure to toggle CS signal in order
	DF_CS_active;								//to reset dataflash command decoder
	
	if (1 == BufferNo)							//write byte(s) to buffer 1
		DF_SPI_RW(Buf1Write);					//buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);					//buffer 2 write op-code

	DF_SPI_RW(0x00);						//don't cares
	DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	for( i=0; i<No_of_bytes; i++)
	{
		DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to dataflash buffer 1 location
		BufferPtr++;						//point to next element in AVR buffer
	}

}
//NB : Monitorer busy-flag i status-reg.
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..



/*****************************************************************************
*
*	Function name : Buffer_To_Page
*
*	Returns :		None
*
*	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
*					PageAdr		->	Address of flash page to be programmed
*
*	Purpose :		Transfers a page from dataflash SRAM buffer to flash
*					
******************************************************************************/
void DFBufferToPage (unsigned char BufferNo, unsigned int PageAdr)
{
	DF_CS_inactive;												//make sure to toggle CS signal in order
	DF_CS_active;												//to reset dataflash command decoder
		
	if (1 == BufferNo)											//program flash page from buffer 1	{
		DF_SPI_RW(Buf1ToFlashWE);								//buffer 1 to flash with erase op-code
	else
		DF_SPI_RW(Buf2ToFlashWE);								//buffer 2 to flash with erase op-code

	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));	//upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PageBits - 8)));	//lower part of page address
	DF_SPI_RW(0x00);										//don't cares
	
	DF_CS_inactive;												//initiate flash page programming
	DF_CS_active;												
	
	while(!(DFReadStatus() & 0x80));							//monitor the status register, wait until busy-flag is high
}


/*****************************************************************************
*
*	Function name : Cont_Flash_Read_Enable
*
*	Returns :		None
*
*	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
*					IntPageAdr	->	Internal page address where cont.read starts from
*
*	Purpose :		Initiates a continuous read from a location in the DataFlash
*					
******************************************************************************/
void DFContFlashReadEnable (unsigned int PageAdr, unsigned int IntPageAdr)
{
	DF_CS_inactive;																//make sure to toggle CS signal in order
	DF_CS_active;																//to reset dataflash command decoder
	
	DF_SPI_RW(ContArrayRead);													//Continuous Array Read op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PageBits)));						//upper part of page address
	DF_SPI_RW((unsigned char)((PageAdr << (PageBits - 8))+ (IntPageAdr>>8)));	//lower part of page address and MSB of int.page adr.
	DF_SPI_RW((unsigned char)(IntPageAdr));										//LSB byte of internal page address
	DF_SPI_RW(0x00);															//perform 4 dummy writes
	DF_SPI_RW(0x00);															//in order to intiate DataFlash
	DF_SPI_RW(0x00);															//address pointers
	DF_SPI_RW(0x00);
}


