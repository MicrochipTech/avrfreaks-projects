
#include <avr/io.h> 
#include <avr/interrupt.h>

#define F_CPU 8000000

#define USART_BAUDRATE 9600 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)


void main(void)
{

	UCSRB |= (1 << RXEN) | (1 << TXEN);   					// Turn on the transmission and reception circuitry
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); 	// Use 8-bit character sizes
	UBRRL = BAUD_PRESCALE; 									// Load lower 8-bits of the baud rate value into the low byte of the UBRR register 
	UBRRH = (BAUD_PRESCALE >> 8); 							// Load upper 8-bits of the baud rate value into the high byte of the UBRR register


	TIMSK |= (1 << TOIE1);									// Enable Timer1 overflow interrupt
	TCCR1B |= (1 << CS11);									// Turn on Timer1, CLK/8 prescale


	ADMUX |= (1 << REFS0);									// Select VCC reference
	ADMUX |= (1 << ADLAR); 									// Left adjust ADC result to allow easy 8 bit reading
	ADCSRA |= (1 << ADEN) | (1 << ADIE);					// Turn on ADC and enable conversion complete interrupt
	ADCSRA |= (1 << ADPS1) | (1 << ADPS2);					// ADC prescale of CLK/64


	sei();

	while(1); {}

}								


ISR(TIMER1_OVF_vect)
{

	ADCSRA |= (1 << ADSC);									// Start ADC conversion

}


ISR(ADC_vect)
{
	unsigned long VOLTAGE = ((((ADCH/10)+((ADCH/100)*6))*16)+(ADCH%10));
	
	unsigned int ONES = 0x00;
	unsigned int TENS = 0x00;
	unsigned int HUND = 0x00;
	
	if((VOLTAGE & 0x0800) == 0x0800) HUND |= 0x08; else HUND &= ~0x08;
	if((VOLTAGE & 0x0400) == 0x0400) HUND |= 0x04; else HUND &= ~0x04;
	if((VOLTAGE & 0x0200) == 0x0200) HUND |= 0x02; else HUND &= ~0x02;
	if((VOLTAGE & 0x0100) == 0x0100) HUND |= 0x01; else HUND &= ~0x01;

	if((VOLTAGE & 0x0080) == 0x0080) TENS |= 0x08; else TENS &= ~0x08;
	if((VOLTAGE & 0x0040) == 0x0040) TENS |= 0x04; else TENS &= ~0x04;
	if((VOLTAGE & 0x0020) == 0x0020) TENS |= 0x02; else TENS &= ~0x02;
	if((VOLTAGE & 0x0010) == 0x0010) TENS |= 0x01; else TENS &= ~0x01;

	if((VOLTAGE & 0x0008) == 0x0008) ONES |= 0x08; else ONES &= ~0x08;
	if((VOLTAGE & 0x0004) == 0x0004) ONES |= 0x04; else ONES &= ~0x04;
	if((VOLTAGE & 0x0002) == 0x0002) ONES |= 0x02; else ONES &= ~0x02;
	if((VOLTAGE & 0x0001) == 0x0001) ONES |= 0x01; else ONES &= ~0x01;

	ONES |= 0x30;
	TENS |= 0x30;
	HUND |= 0x30;

	UDR = HUND;												// Send the ADC results to the UART
	while ((UCSRA & (1 << UDRE)) == 0) {}; 					// Do nothing until UDR is ready for more data to be written to it
	UDR = TENS;
	while ((UCSRA & (1 << UDRE)) == 0) {}; 					// Do nothing until UDR is ready for more data to be written to it
	UDR = ONES;
	while ((UCSRA & (1 << UDRE)) == 0) {}; 					// Do nothing until UDR is ready for more data to be written to it
	UDR = 0x0D;												// Send new line
	while ((UCSRA & (1 << UDRE)) == 0) {}; 					// Do nothing until UDR is ready for more data to be written to it
	UDR = 0x0A;
											
}
