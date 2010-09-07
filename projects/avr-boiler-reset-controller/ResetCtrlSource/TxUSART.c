/* ========================================================================== */
/*   TxUSART.c                                                                */
/*                                                                            */
/*   Description                                                              */
/*   Transmit Only Serial Communication via USART                             */
/* ========================================================================== */

#include "TxUSART.h"

void USART_Init(void)
{
   UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
   UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register

   UCSRB |= (1 << TXEN);   // Turn on the transmission circuitry only
   UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // Use 8-bit character sizes
   sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed

}


//************************************************************************
//; UDR Empty Handler
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
int USART_putchar(char c)
{
	while((TBUFSIZE - tbuflen()) <= 2);  // Wait...
	
	// Add data to the transmit buffer, enable TXCIE
	//
	tbuf[t_in & TMASK] = c;
	t_in++;	
	UCSRB |= (1<<UDRIE);			// Enable UDR empty interrupt	
	return(0);
}

//*************************************************************************
// Retrieve pending chars in TX buffer
//*************************************************************************
char tbuflen(void)
{
	return(t_in - t_out);
}
