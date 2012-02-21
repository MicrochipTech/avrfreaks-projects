
#include "uart.h"

//************************************************************************
// USART_init()  initialize USART and buffers
//************************************************************************
void USART_init(void)
{
   uint8_t v;
   t_in = t_out = r_in = r_out = 0; // reset the buffer pointers.

   v = BAUD_PRESCALE;
   UBRRL = v;                    // Load lower 8-bits of the baud rate value
   v = (BAUD_PRESCALE >> 8);     //  into the low byte of the UBRR register
   UBRRH = v;                    // Load upper 8-bits of the baud rate value
                                 //  into the high byte of the UBRR register

   UCSRB |= (1 << RXEN) | (1 << TXEN);   // Turn on the transmission and reception circuitry
   UCSRC |= (1 << UCSZ0) | (1 << UCSZ1); // Use 8-bit character size No Parity 1 Stop Bit

   UCSRB |= (1 << RXCIE); // Enable the USART Recieve Complete interrupt
}

//************************************************************************
//; USART RX Complete Interrupt Handler
//************************************************************************
ISR(USART_RX_vect)
{
   char ReceivedByte;
   ReceivedByte = UDR; // Fetch the recieved byte value into the variable "ByteReceived"
   rbuf[r_in & RMASK] = ReceivedByte;  // add it to recieve queue.
   r_in++;             // incremnt recieve index.

}

//************************************************************************
//; UDR Empty Interrupt Handler
//************************************************************************
ISR(USART_UDRE_vect)
{
	if(t_in != t_out)    // ** if transmit buffer not empty **
    {
	 	 UDR = tbuf[t_out & TMASK];   // ** load next byte to output reg **
		 t_out++;	                   //  ** update buffer pointer
	  }
	else
   {
		 UCSRB &= ~(1<<UDRIE);   // ** clear the buffer empty int. flag.
	 }
	
}


//************************************************************************
// Fills the transmit buffer, if it is full wait
//************************************************************************
void UART_put_chr(char c)
{
	while((TBUFSIZE - tbuflen()) <= 2);  // buffer full... Wait...
	
	// Add data to the transmit buffer, enable TXCIE
	//
	tbuf[t_in & TMASK] = c;
	t_in++;	
	UCSRB |= (1<<UDRIE);			// Enable UDR empty interrupt	
}

//************************************************************************
// UART_put_str(char *str)  send the string to the USART
//************************************************************************
void UART_put_str(char *str)
{
   int p=0;
   while((char)str[p] != 0)
     UART_put_chr((char)str[p++]);
}


//************************************************************************
// Retieves character from UART.
//
//************************************************************************
char UART_get_chr(void)
{
	char c;
	while(rbuflen() == 0);	  // Wait...
	c = rbuf[r_out & RMASK];
	r_out++;	
	return(c);
}

//*************************************************************************
// Retrieve pending chars in TX buffer
//*************************************************************************
char tbuflen(void)
{
	return(t_in - t_out);
}

// ***********************************************************************
// Retrive pending chars in RX buffer
//************************************************************************
char rbuflen(void)
{
	return(r_in - r_out);
}
