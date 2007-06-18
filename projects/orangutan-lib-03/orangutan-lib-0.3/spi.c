// uart-spi.c
//
// UART-SPI master routines were lifted from the Atmel ATMega168
// datasheet.
//
// Since not all Orangutans bring their SPI I/O lines out to headers,
// an alternatie to the stock SPI routines was required.  ATMega
// processors can emulate SPI communication on the USART pins by
// putting the USART into SPI master mode.  Unfortunately, the
// Orangutan and Orangutan-X2 do bring their UART TX and RX lines
// to headers, but do not bring the synchronous clock line, XCK,
// out to headers.  So this set of routines also only works on the
// Baby-Orangutan.

#include "device.h"

#include <avr/io.h>
#include <util/delay.h>

#include "uart-spi.h"

// Initialize the SPI as master
void uart_spi_init()
{
	// Zero out our baud rate (baud rate can only be set
	// AFTER the transmitter is enabled) 
	UBRR0 = 0;

	// Set the XCK0 port pin as an output, enables master mode
	XCK0_DDR |= (1 << XCK0);

	// Set MSPI mode of operation and SPI mode 0,0
	UCSR0C = (1 << UMSEL01) | (1 << UMSEL00) | (0 << UCPHA0) | (0 << UCPOL0);

	// Enable the receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// Set the baud rate from the value calculated in uart-spi.h
	UBRR0 = BRREG;
}

// Initiate an exchange of data with the slave by transmitting a byte
// and receiving a byte.  Not all SPI devices support two-way
// communication, but the SPI standard does not differentiate between
// transmitting and receiving.  Every transfer is an exchange, with a
// byte sent and a byte received, even if the received byte is zero.
uint8_t uart_spi_transfer( uint8_t data )
{
	// Wait for any pending transmissions
	uart_spi_wait_tx();

	// Put data into the send buffer
	UDR0 = data;

	// Wait for the data to be received
	uart_spi_wait_rx();

	// Get and return received data from the buffer
	return UDR0;
}

