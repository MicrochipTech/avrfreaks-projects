#ifndef _main_h_
#define _main_h_
#include <avr\interrupt.h>
#include <avr\io.h>
#include "mydefs.h"


#define XTAL		14.756e6		// baud rate crystal

//#define BAUD		57600
//#define BAUD		115200
#define BAUD		230400

//#define 		TX_INVERT		// without MAX232

#define	LINEFEED	"\r\n"


#define	STX_DDR		SBIT( DDRB, 1 )		// must be OC1A !

#define	SCL		SBIT( PORTB, 2 )
#define SCL_DDR		SBIT( DDRB, 2 )
#define	SCL_PIN		SBIT( PINB, 2 )

#define	SDA		SBIT( PORTB, 0 )
#define	SDA_DDR		SBIT( DDRB, 0 )
#define	SDA_PIN		SBIT( PINB, 0 )

#endif
