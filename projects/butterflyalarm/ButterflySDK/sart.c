/** @file usart.c Usart driver. Provides functions for sending and receiving
		data from the Usart. Data are received through the Usart Receive
		Interrupt and are stored in circular buffer of 8 length. The
		transmit interrupt is not used. This means that sending a lot
		of bytes at one go, will block the program for a considerable amount
		of time.
	*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "circBuffer.h"

/*******************************************************************************
* Static Variables
*******************************************************************************/

/** Circular buffer used to store the data receieved from the Rx Interrupt. It
	has a length of 8.
*/
static CircBuffer8 circBuffer;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes the Usart driver.
	@param baudrate The baud rate setting. It is not recommended to use
		anything else than the #USART_BAUD_RATE_9600.
*/
void UsartInit(unsigned int baudrate)
{
	// Set baud rate
	UBRRH = (unsigned char)(baudrate>>8);
	UBRRL = (unsigned char)baudrate;

	// Enable 2x speed
	UCSRA = (1<<U2X);

	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(0<<UDRIE);

	// Async. mode, 8N1
	UCSRC = (0<<UMSEL)|(0<<UPM0)|(0<<USBS)|(3<<UCSZ0)|(0<<UCPOL);

	CIRC_BUFFER8_INIT(circBuffer);
}

/** Disables the Usart. It must be called when the serial port is not used
	to save power. */
void UsartDisable(void)
{
	UCSRB = 0;
}

/** Tests whether the Usart is enabled.
	@result TRUE if the Usart is enabled.
*/
BOOL IsUsartEnabled(void)
{
	return UCSRB != 0;
}

/** Transmits a char through the Usart. This is a blocking operation.
	(It takes 10/9600 = 1.04ms to complete)
	@param byte The byte to transmit
*/
void UsartTxChar(uint8_t byte)
{
	while (!(UCSRA & (1<<UDRE)));
	UDR = byte;
}

/** Gets the number of unread bytes in the circular buffer.
	@result The number of unread bytes in the circular buffer
*/
uint8_t UsartNewDataIn(void)
{
	return CIRC_BUFFER8_UNREAD_BYTES(circBuffer);
}

/** Reads a byte from the circular buffer. The position of the byte in the
	buffer is freed, when it is read.
	@result The byte read.
*/
uint8_t UsartRxChar(void)
{
	uint8_t byte;

	CIRC_BUFFER8_READ(circBuffer, byte);
	return byte;
}

/** Transmits a RAM string through the Usart. This is blocking operation.
	(It takes strlen(str) * 10 / 9600 sec to complete)
	@param str The string to transmit
*/
void UsartTxString(char *str)
{
	uint8_t i = 0;

	while (str[i] != '\0')
	{
		while (!(UCSRA & (1<<UDRE)));
		UDR = str[i++];
	}
}

/** Transmits a flash string through the Usart. This is blocking operation.
	(It takes strlen(str) * 10 / 9600 sec to complete)
	@param str The string to transmit
*/
void UsartTxString_F(const char *str)
{
	uint8_t i = 0;
	char c;

	c = (const char) pgm_read_byte(&str[0]);
	while (c != '\0')
	{
		while (!(UCSRA & (1<<UDRE)));
		UDR = c;
		c = (const char) pgm_read_byte(&str[++i]);
	}
}

/** @fn ISR(USART0_RX_vect)
	Usart Receive Interrupt. Places the byte received in the circular buffer.
*/
ISR(USART0_RX_vect)
{
	CIRC_BUFFER8_WRITE(circBuffer, UDR);
}
