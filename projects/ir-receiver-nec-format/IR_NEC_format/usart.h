#ifndef _USART_H
	#include <stdio.h>
	#define _USART_H
	#define FOSC 4000000 // Clock Speed
	#define BAUD 9600
	#define MYUBRR 51//FOSC/16/BAUD-1

	#define _USE_USART_INTERRUPT 0 // comment this if needed
	#define USART_BUFF_SIZE 64

	void usart_init( unsigned int );
	unsigned char usart_receive( void );
	#ifdef _STDIO_H_
		void usart_transmit(char ,FILE *);
	#else
		void usart_transmit(char);
		void puts_h( unsigned char , unsigned long );
		void puts(char *string );
	#endif
#endif

