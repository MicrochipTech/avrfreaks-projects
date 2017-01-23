/*
 * spi.h
 *
 * Created: 12/30/2014 1:54:59 AM
 *  Author: Marko Mlakar (marko@mlakar.si)
 */ 

#ifndef SPI_H_
#define SPI_H_

uint8_t spi_transfer(uint8_t data);
void spi_init();
void spi_mode(unsigned char mode, unsigned char prescaler);

#endif /* SPI_H_ */