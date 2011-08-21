//
#ifndef __PIN_H
#define __PIN_H

#include <avr/io.h>
//Change the settings for your hardware
#define		CH452_PORT			PORTB
#define		CH452_DDR			DDRB
#define		CH452_PIN			PINB
#define		CH452_DCLK_PIN		PB1
#define		CH452_DIN_PIN		PB2
#define		CH452_LOAD_PIN		PB0
#define		CH452_DOUT_PIN		PB3
//
#define		CH452_DCLK_SET		{CH452_PORT |= (1<<CH452_DCLK_PIN);}
#define		CH452_DCLK_CLR		{CH452_PORT &= ~(1<<CH452_DCLK_PIN);}
#define		CH452_DCLK_D_OUT	{}				
#define		CH452_DIN_SET		{CH452_PORT |= (1<<CH452_DIN_PIN);}
#define		CH452_DIN_CLR		{CH452_PORT &= ~(1<<CH452_DIN_PIN);}
#define		CH452_DIN_D_OUT		{}				
#define		CH452_LOAD_SET		{CH452_PORT |= (1<<CH452_LOAD_PIN);}
#define		CH452_LOAD_CLR		{CH452_PORT &= ~(1<<CH452_LOAD_PIN);}
#define		CH452_LOAD_D_OUT	{}				
#define		CH452_DOUT_D_IN		{}	

#endif
