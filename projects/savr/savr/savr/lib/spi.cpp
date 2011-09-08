/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/
#include <inttypes.h>
#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include <savr/cpp_pgmspace.h>
#include <savr/spi.h>


#if defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_MOSI PB3
#define SPI_MISO PB4
#define SPI_SCK  PB5
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__) \
   || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168__)
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_MOSI PB5
#define SPI_MISO PB6
#define SPI_SCK  PB7
#else
#error Unsupported AVR target for SPI interface
#endif

//! For SPI divider settings
typedef struct {
    uint8_t spcr;
    uint8_t spsr;
} SPIConfig;

//! For a divider of 2^x, use regFreqCfg[x-1]
static const SPIConfig CPP_PROGMEM regFreqCfg[] = {
    {                    0, _BV(SPI2X)},    // /2   ... 2^1
    {                    0,          0},    // /4   ... 2^2
    {            _BV(SPR0), _BV(SPI2X)},    // /8   ... 2^3
    {            _BV(SPR0),          0},    // /16  ... 2^4
    {_BV(SPR1)            , _BV(SPI2X)},    // /32  ... 2^5
    {_BV(SPR1)            ,          0},    // /64  ... 2^6
    {_BV(SPR1) | _BV(SPR0), _BV(SPI2X)},    // /128 ... 2^7
};
#define FREQ_CFG_SIZE sizeof(regFreqCfg)/sizeof(SPIConfig)



/**
 * @par Implementation Notes:
 */
void
SPI::SendBlock(uint8_t * input, size_t length)
{
    uint16_t i = 0;
    while(i < length)
        SPI::TrxByte(input[i++]);
}


/**
 * @par Implementation Notes:
 */
void
SPI::GetBlock(uint8_t * input, size_t length, uint8_t filler)
{
    uint16_t i = 0;
    while(i < length)
        input[i++] = SPI::TrxByte(filler);
}


/**
 * @par Implementation Notes:
 */
uint8_t
SPI::TrxByte(uint8_t input)
{
    uint8_t status;

    SPDR = input;
    while( !(SPSR & _BV(SPIF)) ) ;
    status = SPDR;

    return status;
}


/**
 * Find the highest bit that is a 1
 *
 * @param word The 32bit word to search over
 * @return The MSB that is a 1
 *
 * Note that MSB is 31, LSB is 0
 */
uint8_t highestBit(uint32_t word)
{
    uint8_t bitCount = 0;

    if(word == 0) return 0;

    while(word != 1) {
        bitCount++;
        word>>=1;
    }
    return bitCount;
}


/**
 * @par Implementation Notes:
 */
void
SPI::Init(uint32_t sysClock, uint32_t spiFreq)
{
    uint8_t divExp = 0;

    /* Master mode: MISO is Input; MOSI, and SCK is output */
    SPI_DDR &= ~(_BV(SPI_MISO));
    SPI_DDR |= _BV(SPI_MOSI) | _BV(SPI_SCK);

    /* Setup MISO with a pull-up resistor */
    SPI_PORT |= _BV(SPI_MISO);

    // Round down divider and find 2^x (highest bit)
    divExp = highestBit(sysClock/spiFreq);

    // Bounds. divExp to be subtracted by one in a few lines...
    // Sooo, our bounds are [1, FREQ_CFG_SIZE], not the normal [0, FREQ_CFG_SIZE-1] (both inclusive)
    if(divExp == 0) divExp = 1;
    if(divExp > FREQ_CFG_SIZE) divExp = FREQ_CFG_SIZE;
    divExp--;

    /* Setup SPCR
     * SPI enabled, master mode, fck/2, MSB first
     * Mode 0
     */
    SPCR |= _BV(SPE) | _BV(MSTR) | pgm_read_byte(&regFreqCfg[divExp].spcr);
    SPSR |= pgm_read_byte(&regFreqCfg[divExp].spsr);
}

