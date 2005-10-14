
#ifndef _SPILIB_H_ 
#define _SPILIB_H_ 1

#include <inttypes.h>
void SPIInitMaster(void);
void SPIInitSlave(void);
uint8_t SPITransferByte(uint8_t data);
uint8_t SPIReciveByte( uint8_t dataOut,uint8_t *dataIn);

#endif //_SPILIB_H_ 
