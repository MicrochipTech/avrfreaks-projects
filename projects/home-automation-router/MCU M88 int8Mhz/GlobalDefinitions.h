#ifndef GLOBALDEFINITIONS_H
#define GLOBALDEFINITIONS_H 1

	#include <avr/io.h>
	
	typedef unsigned char		uint8;
	typedef unsigned int		uint16;

	#define bit_get(p,m)		((p) & (m))
	#define bit_set(p,m)		((p) |= (m))
	#define bit_clear(p,m)		((p) &=~(m))
	#define bit_flip(p,m)		((p) ^= (m))
	#define BIT(x)				(0x01 << (x))
	#define LEDon				bit_set(PORTB,BIT(5))
	#define LEDoff				bit_clear(PORTB,BIT(5))

	#define Message_Length		6	//Sync & End_Byte included
	#define Sync_Byte			0x9B
	#define End_Byte			0x0D	//<Cr>
	#define Checksum_Byte		Message[Message_Length-2]

	#define Start_RX_Over_Timer		(TCCR0B	= 0b00000011)		//'B'register! Div:64
	#define Stop_RX_Over_Timer		Stop_Timer0
	#define RST_RX_Over_Timer		Reset_Timer0

	#define Start_CLR_Timer		(TCCR0B	= 0b00000101)		//'B'register! Div:1024
	#define Stop_CLR_Timer		Stop_Timer1


	#define Disable_UART_TX		bit_clear(UCSR0B,BIT(TXEN0))
	#define Disable_UART_RX		bit_clear(UCSR0B,BIT(RXEN0))
	#define Enable_UART_TX		bit_set(UCSR0B,BIT(TXEN0))
	#define Enable_UART_RX		bit_set(UCSR0B,BIT(RXEN0))

	#define Enable_UART_RXC_ISR		bit_set(UCSR0B,BIT(RXCIE0))
	#define Disable_UART_RXC_ISR	bit_clear(UCSR0B,BIT(RXCIE0))

	#define _NOP()		__asm__ __volatile__ ("nop")
	#define NOP()		_NOP()
	#define TMR0		TCNT0
	#define TMR1		TCNT1
	#define TMR2		TCNT2

	#define Reset_Timer0	TCNT0 = 0
	#define Reset_Timer1	TCNT1 = 0
	#define Reset_Timer2	TCNT2 = 0
	
	#define Stop_Timer0		TCCR0B = 0
	#define Stop_Timer1		TCCR1B = 0
	#define Stop_Timer2		TCCR2B = 0

	
#endif
