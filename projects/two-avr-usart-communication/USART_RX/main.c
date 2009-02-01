//**********************************************************************//
//FILE: main.c 
//AUTHOR: Adam Kadolph
//DATE:	12-24-08
//DESCRIPTION: Program to receive USART data from another microcontroller
//**********************************************************************//

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

#define FOSC 8000000UL //system clock speed
#define	BAUD 9600UL
#define MYUBRR FOSC/16/(BAUD-1)

void USART_init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);

unsigned volatile char flag;
unsigned char data;

ISR(USART_RXC_vect)
{
	if(data >=0x00 || data <0x09)
	{
		flag = 0x01;
	}
	else
	{
		flag = 0x00;
	}
}

int main(void)
{
	LCDinit();
	USART_init(MYUBRR);

	LCDclr();

	SREG |= 0x80;

	while(1)
	{
		data = USART_Receive();

		while(flag)
		{
			SREG &= ~0x80;

			LCDsendChar(data);

			flag = 0x00;

			SREG |= 0x80;
		}
		
		_delay_ms(10);
	}

	return 0;
}

