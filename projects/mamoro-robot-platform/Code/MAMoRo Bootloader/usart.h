/*****************************************************************************
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	3.4.2007
*
* Definitions for using usart.c
*
****************************************************************************/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#define BAUD 115200UL
#define BAUD_UBRR 16 //see MCU's datasheet
#define ENABLE_U2X //remark this line to disable double speed mode (U2X) 


void usart_init(unsigned char option);
void usart_transmit(unsigned char data );
void usart_transmits(char *data);
void usart_transmiti(unsigned long uint32);
void usart_transmitd(unsigned char *data, unsigned int count);
unsigned char usart_receive( void );
