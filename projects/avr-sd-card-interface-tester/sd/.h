#ifndef _SD_H
#define _SD_H 1

/*************************************************************//**
* \file sd.h
*
* SD SPI interface header
*
* A simple SD interface using SPI. This relies on the associated
* SPI interface, but should be relatively simple to recreate.
*
******************************************************************/

uint8_t   SD_Init(void);
uint8_t   SD_ReadBlock(uint8_t *buf, uint32_t addr, uint16_t length);
uint8_t   SD_WriteBlock(uint8_t *buf, uint32_t addr, uint16_t length);
uint8_t   SD_EraseBlock(uint32_t addr, uint32_t length);

#endif
