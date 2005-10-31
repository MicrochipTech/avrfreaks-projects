/*! \file define.h
 * Contains declaration of routines
 */
 
#ifndef DEFINE_H
#define DEFINE_H

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

//! Set bit on position y of register x
#define SET_BIT(x,y) 	x |= (1<<y)
//! Clear bit on position y of register x
#define CLR_BIT(x,y)	x &= ~(1<<y)
/*! Check if bit on position y of register x is 1.
 * \return 1 when byte was 1 else 0
 */
#define TST_BIT(x,y)	(x & (1<<y))		//OK, werkt


#define FOSC	16000000 //!<Clock frequency of the MCU [Hz]

//!Boolean type
typedef enum{
	FALSE,			//!<0
	TRUE			//!<1
} BOOL;


#define RXBUFSIZE 250//!<Size of receive buffer in bytes
//!Maximum size of a presentation layer message
#define PRESENTATION_SIZE 253

void init_USART(uint32_t baud, BOOL RX_ON);
void send_USART(uint8_t* buf, uint8_t length);
void presLayer(uint8_t* paramType, uint8_t paramLength);



#define RS			7		//!<RS connected to pin 7 of PORTA
#define E			6		//!<EN connected to pin 6 of PORTA
#define RING_DET	5		//!<ring detect  to pin 5 of PORTA
#define LOCK_DET	7		//!<lock detect  to pin 7 of PORTD
#define FSK_IN		0		//!<fsk input    to pin 0 of PORTD

void initDisplay(void);
void setSentence(char* s, uint8_t length, uint8_t pos);

/*!Softwaretimer that is used to check whether the PLL of the 2211-chip
 *is locked.
 */
#define LOCK_TIMER	0		
void timer0_init(void);
BOOL timer0isElapsed(uint8_t index);
void setTimer0(uint8_t index, uint8_t val);

#endif