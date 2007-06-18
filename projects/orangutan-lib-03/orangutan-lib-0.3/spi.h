// spi.h
//
// SPI master routines were lifted from a post to the Pololu Robotics
// forums made by Ben from Pololu.  They were originally written to
// run on an ATMega644, so the only real change was to go from hard-
// coded register names to soft names that are defined in device.h
// since the ATMega48/168 and ATMega644 use different pinouts and
// registers for SPI.

#ifndef _UART_SPI_
#define _UART_SPI_

// Desired bits per second for SPI communication
#define BPS 50000

// Calculate the BRR register value
#define BRREG ((F_CPU/(2*BPS))-1) 

// Loop until the TX buffer is empty and can receive another byte
#define uart_spi_wait_tx()	while (!(UCSR0A & (1 << UDRE0)))

// Loop until any current SPI receive is done
#define uart_spi_wait_rx()	while (!(UCSR0A & (1 << RXC0)))

// lowest-level SPI functions
void uart_spi_init();
uint8_t uart_spi_transfer( uint8_t data );

#endif // _UART_SPI_
