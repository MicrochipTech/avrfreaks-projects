// By OZHAN KD

#ifndef SPI_XM_V1
#define SPI_XM_V1

#include "avr_compiler.h"
#include "cpu_xm_v1.h"

void SpiInit(void);
void SpiSendByte(unsigned char data);
unsigned char SpiTransferByte(unsigned char data);
unsigned int SpiTransferWord(unsigned int data);
unsigned char SpiReceiveByte(unsigned char *data);

#endif
