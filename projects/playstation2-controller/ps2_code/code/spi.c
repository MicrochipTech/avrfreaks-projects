/* 
 * SPI on Atmel MCU
 * 
 * V1.0  
 * 
 * By: Lars Ivar Miljeteig, September 2010
 * Platform: AVR ATmega168 (avr-gcc)
 * Dependencies: avr/io delay.h stdint.h larslib.h
 *
 * Reading data from SPI as master
 */

#include "larslib.h"
#include "spi.h"

#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SPI_CS   PB2

void SPI_init()
{
    // Set MOSI and SCK as output, others as input
    SPI_DDR = (1<<PB3)|(1<<PB5)|(1<<PB2);

    // Enable SPI, Master, set clock rate fck/64, high-when-idle (CPOL), sample on trail (CPHA), LSB (DORD)
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<CPOL)|(1<<DORD)|(1<<CPHA);
}

uint8_t SPI_MTx(uint8_t dataout)
{
    uint8_t datain;

    // Start transmission (MOSI)
    SPDR = dataout;

    // Wait for transmission complete
    while(!(SPSR & (1<<SPIF)));

    // wait and get return value
    _delay_us(30);  
    datain = SPDR;

    return datain;

}
