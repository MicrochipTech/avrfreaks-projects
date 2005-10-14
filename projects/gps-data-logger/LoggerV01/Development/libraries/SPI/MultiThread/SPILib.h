
#ifndef _SPILIB_H_ 
#define _SPILIB_H_ 1

#include <inttypes.h>

typedef struct SPIControlBlock
{
	uint8_t ConfigSPCR;  //eg (1<<CPHA)|(1<<CPOL)|(1<<MSTR)|(1<<SPE)|(1<<SPR0)|(0<<SPR1);
	uint8_t ConfigSPSR;  
	
} SPIControlBlock;

void SPITake(SPIControlBlock* cb);
void SPIRelease(void);

void SPIInit(void);
uint8_t SPITransferByte(uint8_t data);
uint8_t SPIReciveByte( uint8_t dataOut);

#endif //_SPILIB_H_ 
