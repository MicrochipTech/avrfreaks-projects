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
//#define BAUD_UBRR (F_CPU/(BAUD * DIVISOR) - 1)
#define BAUD_UBRR 8
#define ENABLE_U2X //remark this line to disable double speed mode (U2X) 


void USART_init(void);
void USART_Transmit( uint8_t data );
uint8_t USART_Receive( void );
