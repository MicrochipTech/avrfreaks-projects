/* 
 * SPI on Atmel MCU
 * 
 * V1.0  
 * 
 * By: Lars Ivar Miljeteig, September 2010
 * Platform: AVR ATmega168 (avr-gcc)
 * Dependencies: avr/io delay.h stdint.h
 *
 * Reading data from SPI as master
 */


#ifndef SPI_H
#define SPI_H

void SPI_init();
uint8_t SPI_MTx(uint8_t cData);

#endif // SPI_H
