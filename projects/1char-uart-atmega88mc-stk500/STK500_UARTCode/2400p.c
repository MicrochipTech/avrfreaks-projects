#include <avr/io.h> 
/* CPU frequency */
#define F_CPU 3686400UL
#define USART_BAUDRATE 2400 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 

int main (void) 
{ 
   char ReceivedByte; 

   UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry 

	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

   UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register 
   UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register 

   for (;;) // Loop forever 
   { 
      while ((UCSR0A & (1 << RXC0)) == 0) {}; // Do nothing until data have been recieved and is ready to be read from UDR 
      ReceivedByte = UDR0; // Fetch the recieved byte value into the variable "ByteReceived" 

      while ((UCSR0A & (1 << UDRE0)) == 0) {}; // Do nothing until UDR is ready for more data to be written to it 
      UDR0 = ReceivedByte; // Echo back the received byte back to the computer 
   }    
} 
