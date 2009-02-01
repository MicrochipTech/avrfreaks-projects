#include <avr/io.h>
#include <util/delay.h>

#define FOSC 8000000UL //system clock speed
#define	BAUD 9600UL
#define MYUBRR FOSC/16/(BAUD-1)

void USART_init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);

int main(void)
{
	USART_init(MYUBRR);
	
	while(1)
	{
		int i;

		for(i=0x30; i<0x3A; i++)
		{
			USART_Transmit(i);

			_delay_ms(20);
		}

	}
	return(0);
}
