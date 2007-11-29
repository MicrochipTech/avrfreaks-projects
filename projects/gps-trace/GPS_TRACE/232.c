#include <avr/io.h>
#include "RS232.h"

#define F_OSC 4000000   /* oscillator-frequency in Hz */
#define UART_BAUD_RATE 4800			/* baudrate */
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)

void uart0_init(char *s)
{
   // set baud rate
   UBRR0H = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
   UBRR0L = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);

   // Enable receiver and transmitter; enable RX interrupt
   UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

   //asynchronous 8N1
   UCSR0C = (1 << URSEL0) | (3 << UCSZ00);
}

void uart0_putc(unsigned char c)
{
   // wait until UDR ready
   while(!(UCSR0A & (1 << UDRE0)));
   UDR0 = c;    // send character
}

void uart0_puts(char *s)
{
   while (*s) {
      uart0_putc(*s);
      s++;
   }
}

void uart0_send_initial_char()
{
	while(!(UCSR0A & (1 << UDRE0)));
   		UDR0 = 0x43; // "C"
   	while(!(UCSR0A & (1 << UDRE0)));
   		UDR0= 0x0d;
}

void uart1_init()
{
 // set baud rate
   UBRR1H = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
   UBRR1L = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);

   // Enable receiver and transmitter; enable RX interrupt
   UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);

   //asynchronous 8N1
   UCSR1C = (1 << URSEL1) | (3 << UCSZ10);
}

void uart1_putc(unsigned char c)
{
   // wait until UDR ready
   while(!(UCSR1A & (1 << UDRE1)));
   UDR1 = c;    // send character
}

void uart1_puts(char *s)
{
   //  loop until *s != NULL
   while (*s) {
      uart1_putc(*s);
      s++;
   }
}

void uart1_send_initial_char()
{
   while(!(UCSR1A & (1 << UDRE1)));
   	UDR1 = 0x43; // "C"
   while(!(UCSR1A & (1 << UDRE1)));
   	UDR1 = 0x0d;
}
