/*************************************************************//**
* \file spi.c
* 
* SPI interface
*
* A simple SPI interface. The initialization is not run-time
* run-time configurable, so you should look it over and modify
* it if need be.
*
* Notes:
*  The SS line must be manually set by the user.
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/
#include <inttypes.h>
#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "spi.h"

#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_MOSI PB5
#define SPI_MISO PB6
#define SPI_SCK  PB7
#define SPI_SS   PB4


/**
 * Assert CS
 */
void
SPI_SSOn(void) {
  SPI_PORT &= ~_BV(SPI_SS);
}

/**
 * Deassert CS
 */
void
SPI_SSOff(void) {
  SPI_PORT |= _BV(SPI_SS);
}


/**
 * Send a block of data over the SPI
 *
 * @param input a pointer to the source data
 * @param length the size of the source data
 */
void
SPI_SendBlock(uint8_t * input, uint16_t length) {
  uint16_t i = 0;
  while(i < length)
    SPI_TrxByte(input[i++]);
}

/**
 * Reads a block of data from the SPI
 *
 * @param input a pointer to the destination buffer
 * @param length the number of bytes to read
 * @param filler a byte to send continuously while reading
 */
void
SPI_GetBlock(uint8_t * input, uint16_t length, uint8_t filler) {
  uint16_t i = 0;
  while(i < length)
    input[i++] = SPI_TrxByte(filler);
}

/**
 * Tx/Rx a byte
 *
 * @param input the byte to send
 *
 * @return the byte read from the SPI line
 */
uint8_t
SPI_TrxByte(uint8_t input) {
  uint8_t status;

  SPDR = input;
  while( !(SPSR & _BV(SPIF)) );
  status = SPDR;

  return status;
}

/**
 * Initialize the SCI
 */
void
SPI_Init(void) {
  /* Master mode: MISO is Input; SS, MOSI, and SCK is output */
  SPI_DDR &= ~(_BV(SPI_MISO));
  SPI_DDR |= _BV(SPI_SS) | _BV(SPI_MOSI) | _BV(SPI_SCK);

  /* Setup MISO with a pull-up resistor */
  SPI_PORT |= _BV(SPI_MISO);

  SPI_SSOff();

  /* Setup SPCR
   * SPI enabled, master mode, fck/2, MSB first
   */
  SPCR |= _BV(SPE) | _BV(MSTR) | _BV(SPI2X);
}

