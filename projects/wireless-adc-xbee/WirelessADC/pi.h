#include <avr/io.h>

#ifndef SPI_H
#define SPI_H

//SPI PIN CONFIGURATIONS
#define SPI_PORT    PORTB
#define SPI_DDR     DDRB

//pins
#define SCK_PIN     PB7
#define MISO_PIN    PB6
#define MOSI_PIN    PB5
#define CS_PIN      PB4

//CS: Chip Select
#define CS_HIGH   SPI_PORT|=(1<<CS_PIN)
#define CS_LOW    SPI_PORT&=(~(1<<CS_PIN))

void spi_master_init(void);
uint8_t spi_write(uint8_t data);
void spi_slave_init(void);

#endif