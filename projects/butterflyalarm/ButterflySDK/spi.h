/** @file spi.h Spi driver header file.
*/

#ifndef __SPI_HEADER__
#define __SPI_HEADER__

/*******************************************************************************
* Exported functions
*******************************************************************************/

extern void SpiInit(void);
extern void SpiDisable(void);
extern uint8_t SpiReadWrite(uint8_t output);

#endif // __SPI_HEADER__

