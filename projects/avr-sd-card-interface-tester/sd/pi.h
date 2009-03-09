#ifndef _SPI_H_
#define _SPI_H_ 1

/*************************************************************//**
* \file spi.h
* 
* SPI interface header
*
* A simple SPI interface.
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/

void        SPI_Init(void);
void        SPI_SendBlock(uint8_t *, uint16_t);
void        SPI_GetBlock(uint8_t *, uint16_t, uint8_t);
uint8_t     SPI_TrxByte(uint8_t);

void        SPI_SSOn(void);
void        SPI_SSOff(void);

#endif
