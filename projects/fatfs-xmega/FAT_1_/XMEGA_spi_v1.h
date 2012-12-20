// By OZHAN KD

#ifndef XM_SPI_V1
#define XM_SPI_V1

#include "avr_compiler.h"
#include "XMEGA_cpu_v2.h"

void SpiInit(void);
void SpiSendByte(unsigned char data);
unsigned char SpiTransferByte(unsigned char data);
unsigned int SpiTransferWord(unsigned int data);
unsigned char SpiReceiveByte(unsigned char *data);

#endif
