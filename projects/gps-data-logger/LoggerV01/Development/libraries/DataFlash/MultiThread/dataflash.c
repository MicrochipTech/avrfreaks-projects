//--------- Example Code : Accessing Atmel AT45Dxxx dataflash on STK500 
//
//
// 
//File name   : 	dataflash.c
//
//Description : 	Functions to access the Atmel AT45Dxxx dataflash series
//      				  Supports 512Kbit - 64Mbit
//                 
//Last change:  Aug 2001   	AS
//				Nov 04		Me: changed SPI interface, now uses SPI library
//							    This facilutates SPI sharing btwn devices.
//								AND from >1 thread accessing the Flash simultaneously
// 


#include <avr\io.h>
#include <avr\pgmspace.h>

#include "..\libraries\DataFlash\MultiThread\dataflash.h" 
#include "..\libraries\SPI\MultiThread\SPILib.h"

#if !defined(BV)
#  define BV(A) (1<<A)
#endif

#define cbi(P, B) P &= ~BV(B)
#define sbi(P, B) P |= BV(B)

//Look-up table for these sizes ->    512k, 1M, 2M, 4M, 8M, 16M, 32M, 64M
static uint8_t PROGMEM DF_pagebits[]  ={9,  9,  9,  9,  9,  10,  10,  11}; //index of internal page address bits

static unsigned char PageBits;
static SPIControlBlock cb;		//our SPI configuration (address must not change)
								//as CPI library uses address to id caller.

static unsigned char ReadStatus (void);
static bool flashWorking =false;

//-----------------------------------------------------------------------

bool DFFlashOK(void)
{
	return flashWorking;
}
//-----------------------------------------------------------------------
//	Sets up the HW SPI in Master mode, Mode 3
//	Note -> Uses the SS line to control the DF CS-line.

void DFSPIInit (void)
{
	flashWorking = false;

	sbi(DFCtlPort,   DFChipSelect);	// bit for flash device chip select (active lo)
	sbi(DFCtlPortDD, DFChipSelect); // port for flash device chip select
	
	// set out wanted SPI config.
	// this is ussed when Access is wanted (SPITake())
	
	cb.ConfigSPCR  = (1<<SPE) | (1<<MSTR) | (1<<CPHA) | (1<<CPOL);	//Enable SPI in Master, mode 3
	cb.ConfigSPSR  = 0;												//4MHz clock

}
//-----------------------------------------------------------------------
//Read and writes one byte from/to SPI master
//Returns :		Byte read from SPI data register (any value)
//Parameters :	Byte to be written to SPI data register (any value)
/*

REMOVED, NOW USING SPI LIBRARIES VERSION

static unsigned char SPITransferByte (unsigned char output)
{
	unsigned char input;	
	SPDR = output;							//put byte 'output' in SPI data register
	while(!(SPSR & 0x80));					//wait for transfer complete, poll SPIF-flag
	input = SPDR;							//read value in SPI data reg.	
	return input;							//return the byte clocked in from SPI slave
}
*/		
//-----------------------------------------------------------------------
//Status info concerning the Dataflash is busy or not.
//Status info concerning compare between buffer and flash page
//Status info concerning size of actual device
//Returns :		One status byte. Consult Dataflash datasheet for further
//				decoding info

unsigned char DFReadStatus (void)
{
	unsigned char result;
	
	SPITake(&cb);
	{
		result = ReadStatus();
	}
	SPIRelease();
	
	flashWorking = (result == DF2MBStatusOK)  || (result == DFSTK500StatusOK);	//stk500's and AT45DB161
	
	return result;								//return the read status register value
}
//-----------------------------------------------------------------------
//Transfers a page from flash to dataflash SRAM buffer
//BufferNo	->	Decides usage of either buffer 1 or 2
//PageAdr	->	Address of page to be transferred to buffer

void DFPageToBuffer (unsigned int PageAdr, unsigned char BufferNo)
{
	
	SPITake(&cb);
	{
		DF_CS_inactive;												//make sure to toggle CS signal in order
		DF_CS_active;												//to reset dataflash command decoder
		
		if (1 == BufferNo)											//transfer flash page to buffer 1
			SPITransferByte(FlashToBuf1Transfer);					//transfer to buffer 1 op-code
		else
			SPITransferByte(FlashToBuf2Transfer);					//transfer to buffer 2 op-code
		
		SPITransferByte((unsigned char)(PageAdr >> (16 - PageBits)));//upper part of page address
		SPITransferByte((unsigned char)(PageAdr << (PageBits - 8)));//lower part of page address
		SPITransferByte(0x00);										//don't cares
		
		DF_CS_inactive;												//initiate the transfer
		DF_CS_active;
		
		while(!(ReadStatus() & 0x80));								//monitor status reg, wait til busy-flag hi

		DF_CS_inactive;												//deselect
	}
	SPIRelease();
	
}

//-----------------------------------------------------------------------
//TCompares a page from flash to dataflash SRAM buffer
//BufferNo	->	Decides usage of either buffer 1 or 2
//PageAdr	->	Address of page to be transferred to buffer
//returns true if match ok

bool DFPageToBufferCompare (unsigned int PageAdr, unsigned char BufferNo)
{
	unsigned char status;
	
	SPITake(&cb);
	{
		DF_CS_inactive;												//make sure to toggle CS signal in order
		DF_CS_active;												//to reset dataflash command decoder
		
		if (1 == BufferNo)											//transfer flash page to buffer 1
			SPITransferByte(FlashToBuf1Compare);					//transfer to buffer 1 op-code
		else
			SPITransferByte(FlashToBuf2Compare);					//transfer to buffer 2 op-code
		
		SPITransferByte((unsigned char)(PageAdr >> (16 - PageBits)));//upper part of page address
		SPITransferByte((unsigned char)(PageAdr << (PageBits - 8)));//lower part of page address
		SPITransferByte(0x00);										//don't cares
		
		DF_CS_inactive;												//initiate the transfer
		DF_CS_active;
		
		while(!((status = ReadStatus()) & 0x80));					//monitor the status register, 
																	//wait until busy-flag is high
		
		DF_CS_inactive;												//deselect
	}
	
	
	SPIRelease();
	
	return (status & 0x40) == 0;		//status:6 set if difference
}
//-----------------------------------------------------------------------
//	Reads one byte from one of the dataflash internal SRAM buffers
//	Returns :		One read byte (any value)
//	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
//					IntPageAdr	->	Internal page address

unsigned char DFBufferReadByte (unsigned char BufferNo, unsigned int IntPageAdr)
{
	unsigned char data=0;
	
	SPITake(&cb);
	{
		DF_CS_inactive;									//make sure to toggle CS signal in order
		DF_CS_active;									//to reset dataflash command decoder
		
		if (1 == BufferNo)								//read byte from buffer 1
			SPITransferByte(Buf1Read);					//buffer 1 read op-code
		else
			SPITransferByte(Buf2Read);					//buffer 2 read op-code
	
		SPITransferByte(0x00);							//don't cares
		SPITransferByte((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		SPITransferByte((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		SPITransferByte(0x00);							//don't cares
		data = SPITransferByte(0x00);					//read byte
		
		DF_CS_inactive;									//deselect
	}
	SPIRelease();
	
	return data;										//return the read data byte
}
//-----------------------------------------------------------------------
//Reads one or more bytes from one of the dataflash
//internal SRAM buffers, and puts read bytes into
//buffer pointed to by *BufferPtr
//Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
//				IntPageAdr	->	Internal page address
//				No_of_bytes	->	Number of bytes to be read
//				*BufferPtr	->	address of buffer to be used for read bytes

void DFReadBuffer (	unsigned char BufferNo, unsigned int IntPageAdr, 
					unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	SPITake(&cb);
	{
		DF_CS_inactive;									//make sure to toggle CS signal in order
		DF_CS_active;									//to reset dataflash command decoder
		
		if (1 == BufferNo)								//read byte(s) from buffer 1
			SPITransferByte(Buf1Read);					//buffer 1 read op-code
		else
			SPITransferByte(Buf2Read);					//buffer 2 read op-code
		
		SPITransferByte(0x00);							//don't cares
		SPITransferByte((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		SPITransferByte((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		SPITransferByte(0x00);							//don't cares
		for( i=0; i<No_of_bytes; i++)
		{
			*(BufferPtr) = SPITransferByte(0x00);		//read byte and put it in AVR buffer pointed 
														//to by *BufferPtr
			BufferPtr++;								//point to next element in AVR buffer
		}
		
		DF_CS_inactive;									//deselect
	}
	SPIRelease();
	
}

//-----------------------------------------------------------------------
//Copies one or more bytes to one of the dataflash internal SRAM buffers from 
//AVR SRAM buffer pointed to by *BufferPtr
//BufferNo	->	Decides usage of either buffer 1 or 2
//IntPageAdr	->	Internal page address
//No_of_bytes	->	Number of bytes to be written
//*BufferPtr	->	address of buffer to be used for copy of bytes
//					from AVR buffer to dataflash buffer 1 (or 2)

void DFWriteToBuffer (	unsigned char BufferNo, unsigned int IntPageAdr, 
						unsigned int No_of_bytes, unsigned char *BufferPtr)
{
	unsigned int i;

	SPITake(&cb);
	{
		DF_CS_inactive;									//make sure to toggle CS signal in order
		DF_CS_active;									//to reset dataflash command decoder
		
		if (1 == BufferNo)								//write byte(s) to buffer 1
			SPITransferByte(Buf1Write);					//buffer 1 write op-code
		else
			SPITransferByte(Buf2Write);					//buffer 2 write op-code
	
		SPITransferByte(0x00);							//don't cares
		SPITransferByte((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		SPITransferByte((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		for( i=0; i<No_of_bytes; i++)
		{
			SPITransferByte(*(BufferPtr));				//write byte pointed at by *BufferPtr 
														//to dataflash buffer 1 location
			BufferPtr++;								//point to next element in AVR buffer
		}
		
		DF_CS_inactive;									//deselect

	}
	SPIRelease();

}

//-----------------------------------------------------------------------
//Purpose :		Transfers a page from dataflash SRAM buffer to flash
//BufferAdr	->	Decides usage of either buffer 1 or 2
//PageAdr		->	Address of flash page to be programmed

void DFBufferToPage (unsigned char BufferNo, unsigned int PageAdr)
{
	SPITake(&cb);
	{
		DF_CS_inactive;												//make sure to toggle CS signal in order
		DF_CS_active;												//to reset dataflash command decoder
			
		if (1 == BufferNo)											//program flash page from buffer 1	{
			SPITransferByte(Buf1ToFlashWE);							//buffer 1 to flash with erase op-code
		else
			SPITransferByte(Buf2ToFlashWE);							//buffer 2 to flash with erase op-code
	
		SPITransferByte((unsigned char)(PageAdr >> (16 - PageBits)));//upper part of page address
		SPITransferByte((unsigned char)(PageAdr << (PageBits - 8)));//lower part of page address
		SPITransferByte(0x00);										//don't cares
		
		DF_CS_inactive;												//initiate flash page programming
		DF_CS_active;												
		
		while(!(ReadStatus() & 0x80));								//monitor status, wait til busy-flag high
		
		DF_CS_inactive;												//deselect

	}
	SPIRelease();
}
//-----------------------------------------------------------------------

void DFErasePage (unsigned int PageAdr)
{
	SPITake(&cb);
	{
		DF_CS_inactive;												//make sure to toggle CS signal in order
		DF_CS_active;												//to reset dataflash command decoder
			
		SPITransferByte(PageErase);									//buffer 1 to flash with erase op-code
	
		SPITransferByte((unsigned char)(PageAdr >> (16 - PageBits)));//upper part of page address
		SPITransferByte((unsigned char)(PageAdr << (PageBits - 8)));//lower part of page address
		SPITransferByte(0x00);										//don't cares
		
		DF_CS_inactive;												//initiate flash page programming
		DF_CS_active;												
		
		while(!(ReadStatus() & 0x80));								//monitor status, wait til busy-flag high
		
		DF_CS_inactive;												//deselect

	}
	SPIRelease();
}
//-----------------------------------------------------------------------
//Initiates a continuous read from a location in the DataFlash
//PageAdr		->	Address of flash page where cont.read starts from
//IntPageAdr	->	Internal page address where cont.read starts from
				
void DFContFlashReadEnable (unsigned int PageAdr, unsigned int IntPageAdr)
{
	SPITake(&cb);
	{
		DF_CS_inactive;													//must toggle CS signal in order
		DF_CS_active;													//to reset dataflash command decoder
		
		SPITransferByte(ContArrayRead);									//Continuous Array Read op-code
		SPITransferByte((unsigned char)(PageAdr >> (16 - PageBits)));	//hi part of page address
		SPITransferByte((unsigned char)((PageAdr << 
						(PageBits - 8))+ (IntPageAdr>>8)));				//lo part of page addr & MSB of IntPageAdr.
		SPITransferByte((unsigned char)(IntPageAdr));					//LSB byte of internal page address
		SPITransferByte(0x00);											//perform 4 dummy writes
		SPITransferByte(0x00);											//in order to intiate DataFlash
		SPITransferByte(0x00);											//address pointers
		SPITransferByte(0x00);
		
		DF_CS_inactive;													//deselect

	}
	SPIRelease();
}



//-----------------------------------------------------------------------
// private version, no SPI sempahore management as its callers responsibility

static unsigned char ReadStatus (void)
{
	unsigned char result,index_copy;
	
	DF_CS_inactive;							//make sure to toggle CS signal in order
	DF_CS_active;							//to reset dataflash command decoder
	result = SPITransferByte(StatusReg);	//send status register read op-code
	result = SPITransferByte(0x00);			//dummy write to get result
		
	DF_CS_inactive;							//deselect
	
	index_copy = ((result & 0x38) >> 3);	//get the size info from status register

	PageBits = pgm_read_byte_near(DF_pagebits+index_copy);

	return result;								//return the read status register value
}


//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..


//-----------------------------------------------------------------------
//Enables continous write functionality to one of the dataflash buffers
//buffers. NOTE : User must ensure that CS goes high to terminate
//this mode before accessing other dataflash functionalities 
//Parameters :	IntPageAdr	->	Internal page address to start writing from
//				BufferAdr	->	Decides usage of either buffer 1 or 2
/*					
void DFBufferWriteEnable (unsigned char BufferNo, unsigned int IntPageAdr)
{
	SPITake(&cb);
	{
		DF_CS_inactive;									//make sure to toggle CS signal in order
		DF_CS_active;									//to reset dataflash command decoder
		
		if (1 == BufferNo)								//write enable to buffer 1
			SPITransferByte(Buf1Write);					//buffer 1 write op-code
		else
			SPITransferByte(Buf2Write);					//buffer 2 write op-code
	
		SPITransferByte(0x00);							//don't cares
		SPITransferByte((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		SPITransferByte((unsigned char)(IntPageAdr));	//lower part of internal buffer address

		DF_CS_inactive;									//deselect

	}
	SPIRelease();
}
//-----------------------------------------------------------------------
//Writes one byte to one of the dataflash internal SRAM buffers
//IntPageAdr	->	Internal page address to write byte to
//BufferAdr	->	Decides usage of either buffer 1 or 2
//Data		->	Data byte to be written

void DFBufferWriteByte (unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
	
	SPITake(&cb);
	{
		DF_CS_inactive;									//make sure to toggle CS signal in order
		DF_CS_active;									//to reset dataflash command decoder
		
		if (1 == BufferNo)								//write byte to buffer 1
			SPITransferByte(Buf1Write);					//buffer 1 write op-code
		else
			SPITransferByte(Buf2Write);					//buffer 2 write op-code
	
		SPITransferByte(0x00);							//don't cares
		SPITransferByte((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
		SPITransferByte((unsigned char)(IntPageAdr));	//lower part of internal buffer address
		SPITransferByte(Data);							//write data byte

		DF_CS_inactive;									//deselect
	
	}
	SPIRelease();
}
*/