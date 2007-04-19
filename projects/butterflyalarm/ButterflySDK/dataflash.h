/** @file dataflash.h Dataflash module header file. */

#ifndef __DATA_FLASH_HEADER__
#define __DATA_FLASH_HEADER__

/*******************************************************************************
* AT45DB041B DATAFLASH Constants
*******************************************************************************/

#define DENSITY_BIT_MASK	0x1C
#define PAGE_BITS			9
#define PAGE_SIZE			264

/*******************************************************************************
* Dataflash op-codes
*******************************************************************************/

#define CONTINUOUS_ARRAY_READ							0x68
#define MAIN_MEMORY_PAGE_READ							0x52
#define BUFFER_1_READ									0x54
#define BUFFER_2_READ									0x56
#define STATUS_REGISTER_READ							0x57
#define BUFFER_1_WRITE									0x84
#define BUFFER_2_WRITE									0x87
#define BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_ERASE		0x83
#define BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_ERASE		0x86
#define BUFFER_1_TO_MAIN_MEMORY_PAGE_PROGRAM_NO_ERASE	0x88
#define BUFFER_2_TO_MAIN_MEMORY_PAGE_PROGRAM_NO_ERASE	0x89
#define PAGE_ERASE										0x81
#define BLOCK_ERASE										0x50
#define MAIN_MEMORY_PAGE_PROGRAM_THROUGH_BUFFER_1		0x82
#define MAIN_MEMORY_PAGE_PROGRAM_THROUGH_BUFFER_2		0x85
#define MAIN_MEMORY_PAGE_TO_BUFFER_1_TRANSFER			0x53
#define MAIN_MEMORY_PAGE_TO_BUFFER_2_TRANSFER			0x55
#define MAIN_MEMORY_PAGE_TO_BUFFER_1_COMPARE			0x60
#define MAIN_MEMORY_PAGE_TO_BUFFER_2_COMPARE			0x61
#define AUTO_PAGE_REWRITE_THROUGH_BUFFER_1				0x58
#define AUTO_PAGE_REWRITE_THROUGH_BUFFER_2				0x59

/*******************************************************************************
* Dataflash macros
*******************************************************************************/

/** @def DATAFLASH_CHIP_SELECT_ACTIVE Activates the dataflash */
#define DATAFLASH_CHIP_SELECT_ACTIVE()		cbiBF(PORTB,0)

/** @def DATAFLASH_CHIP_SELECT_INACTIVE De-activates the dataflash */
#define DATAFLASH_CHIP_SELECT_INACTIVE()	sbiBF(PORTB,0)

/*******************************************************************************
* Dataflash exported functions
*******************************************************************************/

extern uint8_t DataFlashReadStatus(void);
extern void DataFlashPageToBuffer(uint16_t pageAddress, uint8_t bufferNumber);
extern uint8_t DataFlashBufferReadByte(uint8_t bufferNumber,
									   uint16_t byteAddress);
extern void DataFlashBufferReadBytes(uint8_t bufferNumber,
									 uint16_t byteAddress,
									 uint16_t bufferLength,
									 uint8_t *buffer);
extern void DataFlashBufferWriteEnable(uint8_t bufferNumber,
									   uint16_t byteAddress);
extern void DataFlashBufferWriteByte(uint8_t bufferNumber,
									 uint16_t byteAddress,
									 uint8_t byte);
extern void DataFlashBufferWriteBytes(uint8_t bufferNumber,
									  uint16_t byteAddress,
									  uint16_t bufferLength,
									  uint8_t *buffer);
extern void DataFlashBufferToPage(uint8_t bufferNumber, uint16_t pageAddress);
extern void DataFlashContinuousReadEnable(uint16_t pageAddress,
										  uint16_t byteAddress);
extern void DataFlashPageErase(uint16_t pageAddress);
extern void DataFlashAutoPageRewrite(uint16_t pageAddress, uint8_t bufferNumber);

#endif // __DATA_FLASH_HEADER__

