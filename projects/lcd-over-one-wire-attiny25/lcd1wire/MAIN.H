#ifndef _main_h_
#define _main_h_


#include <io.h>
#include <interrupt.h>
#include "mydefs.h"


#define	RX_INVERT			// RS-232 without level converter


#define	XTAL	8e6			// 8MHz


#define F_CPU   XTAL
#include <delay.h>


#define	LCD_D4		SBIT( PORTB, 0 )
#define	LCD_DDR_D4	SBIT( DDRB, 0 )

#define	LCD_D5		SBIT( PORTB, 4 )
#define	LCD_DDR_D5	SBIT( DDRB, 4 )

#define	LCD_D6		SBIT( PORTB, 3 )	// also UART RXD
#define	LCD_DDR_D6	SBIT( DDRB, 3 )

#define	LCD_D7		SBIT( PORTB, 5 )
#define	LCD_DDR_D7	SBIT( DDRB, 5 )

#define	LCD_RS		SBIT( PORTB, 2 )
#define	LCD_DDR_RS	SBIT( DDRB, 2 )

#define	LCD_E0		SBIT( PORTB, 1 )
#define	LCD_DDR_E0	SBIT( DDRB, 1 )


#define RXD		SBIT( PORTB, 3 )
#define RXD_DDR		SBIT( DDRB, 3 )
#define	RXD_PIN		SBIT( PINB, 3 )
#define RXD_INT		PCINT3


#ifdef RX_INVERT
#define get_rxd()       (RXD_PIN == 0)
#else
#define get_rxd()       (RXD_PIN == 1)
#endif


#endif
