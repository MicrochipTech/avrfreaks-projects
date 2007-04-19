/** @file dataflash.c Dataflash module. Provides functions for interacting with
		the dataflash. The processor and the dataflash are connected via an
		SPI bus. A non-interrupt driven SPI driver is used. Therefore all the
		dataflash routines will block, until all bytes are sent or received.
		Additionally, the routines poll the dataflash status register for the
		busy flag to be cleared. For write operations this could take up to
		20ms. Therefore the programmer must carefully consider the timing
		requirements, when using these routines.
		It is not recommended to write to the dataflash, when the coin
		barrery is used for power supply.
*/

#include <avr/io.h>
#include <inttypes.h>

#include "c_ext.h"
#include "spi.h"
#include "dataflash.h"

/*******************************************************************************
* Read operations
*******************************************************************************/

/** Reads the dataflash status register
	@result The current value of the dataflash status register
*/
uint8_t DataFlashReadStatus(void)
{
	uint8_t result;

	// Toggle CS signal in order to reset dataflash command decoder
	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	result = SpiReadWrite(STATUS_REGISTER_READ);
	result = SpiReadWrite(0x00);

	return result;
}

/** Copies a page to a dataflash buffer.
	@param pageAddress The address of the page
	@param bufferNumber The number of the buffer (0 or 1)
*/
void DataFlashPageToBuffer(uint16_t pageAddress, uint8_t bufferNumber)
{
	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	if ((bufferNumber & 1) == 0)
	{
		SpiReadWrite(MAIN_MEMORY_PAGE_TO_BUFFER_1_TRANSFER);
	}
	else
	{
		SpiReadWrite(MAIN_MEMORY_PAGE_TO_BUFFER_2_TRANSFER);
	}
	SpiReadWrite((uint8_t) (pageAddress >> (16 - PAGE_BITS)));
	SpiReadWrite((uint8_t) (pageAddress << (PAGE_BITS - 8)));
	SpiReadWrite(0x00);

	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	// Monitor the status register, wait until busy-flag is high
	while(!(DataFlashReadStatus() & 0x80));
}

/** Reads a byte from one of the dataflash buffer. #SpiReadWrite can be called
	afterwards to retrieve more bytes. Dataflash must be manually de-activated!
	@param bufferNumber The number of the buffer (0 or 1)
	@param byteAddress The address of the byte in the buffer
	@result The byte read
*/
uint8_t DataFlashBufferReadByte(uint8_t bufferNumber, uint16_t byteAddress)
{
	uint8_t data;

	data='0';

	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	if ((bufferNumber & 1) == 0)
	{
		SpiReadWrite(BUFFER_1_READ);
	}
	else
	{
		SpiReadWrite(BUFFER_2_READ);
	}

	SpiReadWrite(0x00);
	SpiReadWrite((uint8_t)(byteAddress>>8));
	SpiReadWrite((uint8_t)(byteAddress));
	SpiReadWrite(0x00);
	data = SpiReadWrite(0x00);

	return data;
}

/** Reads a number of bytes from the buffer. #SpiReadWrite can be called
	afterwards to retrieve more bytes. Dataflash must be manually de-activated!
	@param bufferNumber The number of the buffer (0 or 1)
	@param byteAddress The address of the first byte in the buffer
	@param bufferLength Number of bytes to read
	@param buffer Buffer in RAM, where the read bytes will be copied to
*/
void DataFlashBufferReadBytes(uint8_t bufferNumber,
							  uint16_t byteAddress,
							  uint16_t bufferLength,
							  uint8_t *buffer)
{
	uint16_t i;

	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	if ((bufferNumber & 1) == 0)
	{
		SpiReadWrite(BUFFER_1_READ);
	}
	else
	{
		SpiReadWrite(BUFFER_2_READ);
	}

	SpiReadWrite(0x00);
	SpiReadWrite((uint8_t)(byteAddress>>8));
	SpiReadWrite((uint8_t)(byteAddress));
	SpiReadWrite(0x00);
	for( i=0; i<bufferLength; i++)
	{
		*(buffer) = SpiReadWrite(0x00);
		buffer++;
	}
}

/** Enables continuous read from a page. #SpiReadWrite can be called
	afterwards to read data. Dataflash must be manually de-activated!
	@param pageAddress The address of the page
	@param byteAddress The address of the first byte in the page
*/
void DataFlashContinuousReadEnable(uint16_t pageAddress, uint16_t byteAddress)
{
	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	SpiReadWrite(CONTINUOUS_ARRAY_READ);
	SpiReadWrite((uint8_t)(pageAddress >> (16 - PAGE_BITS)));
	SpiReadWrite((uint8_t)((pageAddress << (PAGE_BITS - 8)) + (byteAddress>>8)));
	SpiReadWrite((uint8_t)(byteAddress));
	SpiReadWrite(0x00);
	SpiReadWrite(0x00);
	SpiReadWrite(0x00);
	SpiReadWrite(0x00);
}

/*******************************************************************************
* Write operations
*******************************************************************************/

/** Enables continuous write functionality to one of the dataflash buffers. Data
	can subsequently be written by calling #SpiReadWrite. User must ensure that
	CS goes high to terminate this mode before accessing other dataflash
	functionalities.
	@param bufferNumber The number of the buffer (0 or 1)
	@param byteAddress The address of the first byte in the buffer
*/
void DataFlashBufferWriteEnable(uint8_t bufferNumber, uint16_t byteAddress)
{
	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	if ((bufferNumber & 1) == 0)
	{
		SpiReadWrite(BUFFER_1_WRITE);
	}
	else
	{
		SpiReadWrite(BUFFER_2_WRITE);
	}

	SpiReadWrite(0x00);
	SpiReadWrite((uint8_t)(byteAddress>>8));
	SpiReadWrite((uint8_t)(byteAddress));
}

/** Enables continuous write functionality to one of the dataflash buffers and
	writes one byte to it. Data can subsequently be written by calling
	#SpiReadWrite. User must ensure that CS goes high to terminate this mode
	before accessing other dataflash functionalities.
	@param bufferNumber The number of the buffer (0 or 1)
	@param byteAddress The address of the first byte in the buffer
	@param byte The byte to write
*/
void DataFlashBufferWriteByte(uint8_t bufferNumber,
							  uint16_t byteAddress,
							  uint8_t byte)
{
	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	if ((bufferNumber & 1) == 0)
	{
		SpiReadWrite(BUFFER_1_WRITE);
	}
	else
	{
		SpiReadWrite(BUFFER_2_WRITE);
	}

	SpiReadWrite(0x00);
	SpiReadWrite((uint8_t)(byteAddress>>8));
	SpiReadWrite((uint8_t)(byteAddress));
	SpiReadWrite(byte);
}

/** Enables continuous write functionality to one of the dataflash buffers and
	writes a number of bytes to it.  User must ensure that CS goes high to
	terminate this mode before accessing other dataflash functionalities.
	@param bufferNumber The number of the buffer (0 or 1)
	@param byteAddress The address of the first byte in the buffer
	@param bufferLength The number of bytes to write
	@param buffer Buffer in RAM to read the data from
*/
void DataFlashBufferWriteBytes(uint8_t bufferNumber,
							   uint16_t byteAddress,
							   uint16_t bufferLength,
							   uint8_t *buffer)
{
	uint16_t i;

	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	if ((bufferNumber & 1) == 0)
	{
		SpiReadWrite(BUFFER_1_WRITE);
	}
	else
	{
		SpiReadWrite(BUFFER_2_WRITE);
	}

	SpiReadWrite(0x00);
	SpiReadWrite((uint8_t)(byteAddress>>8));
	SpiReadWrite((uint8_t)(byteAddress));
	for( i=0; i<bufferLength; i++)
	{
		SpiReadWrite(*(buffer));
		buffer++;
	}
}

/** Copies the contents of a dataflash buffer to a page.
	@param bufferNumber The number of the buffer (0 or 1)
	@param pageAddress The address of the page
*/
void DataFlashBufferToPage(uint8_t bufferNumber, uint16_t pageAddress)
{
	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	if ((bufferNumber & 1) == 0)
	{
		SpiReadWrite(BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_ERASE);
	}
	else
	{
		SpiReadWrite(BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_ERASE);
	}

	SpiReadWrite((uint8_t)(pageAddress >> (16 - PAGE_BITS)));
	SpiReadWrite((uint8_t)(pageAddress << (PAGE_BITS - 8)));
	SpiReadWrite(0x00);

	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	while(!(DataFlashReadStatus() & 0x80));
}

/** Erases one page.
	@param pageAddress The address of the page
*/
void DataFlashPageErase(uint16_t pageAddress)
{
	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	SpiReadWrite(PAGE_ERASE);
	SpiReadWrite((uint8_t)(pageAddress >> (16 - PAGE_BITS)));
	SpiReadWrite((uint8_t)(pageAddress << (PAGE_BITS - 8)));
	SpiReadWrite(0x00);

	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	while(!(DataFlashReadStatus() & 0x80));
}

/** Auto re-writes a page using one of the dataflash buffers. According to the
	AT45DB041 data sheet, each page within a sector must be updated/rewritten
	at least once within every 10,000 cumulative page erage/program operations
	in that sector. Calling this function is not necessary, if a sector is
	programmed sequentially page-by-page. It is required however, when the
	pages are randomly written.
	@param pageAddress The address of the page
	@param bufferNumber The number of the buffer (0 or 1)
*/
void DataFlashAutoPageRewrite(uint16_t pageAddress, uint8_t bufferNumber)
{
	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	if ((bufferNumber & 1) == 0)
	{
		SpiReadWrite(AUTO_PAGE_REWRITE_THROUGH_BUFFER_1);
	}
	else
	{
		SpiReadWrite(AUTO_PAGE_REWRITE_THROUGH_BUFFER_2);
	}
	SpiReadWrite((uint8_t) (pageAddress >> (16 - PAGE_BITS)));
	SpiReadWrite((uint8_t) (pageAddress << (PAGE_BITS - 8)));
	SpiReadWrite(0x00);

	DATAFLASH_CHIP_SELECT_INACTIVE();
	DATAFLASH_CHIP_SELECT_ACTIVE();

	// Monitor the status register, wait until busy-flag is high
	while(!(DataFlashReadStatus() & 0x80));
}
