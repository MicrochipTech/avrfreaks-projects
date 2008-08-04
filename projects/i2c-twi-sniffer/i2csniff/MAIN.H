#ifndef _main_h_
#define _main_h_
#include <interrupt.h>
#include <io.h>
#include "mydefs.h"


#define XTAL		14.756e6

//#define BAUD		230400
#define	BAUD		115200

//#define	TX_INVERT			// without MAX232

#define	LINEFEED	"\r\n"

#define	F_CPU	XTAL
#include <util/delay.h>

#define	STX		SBIT( PORTB, 1 )
#define	STX_DDR		SBIT( DDRB, 1 )

#define	SCL		SBIT( PORTB, 2 )
#define SCL_DDR		SBIT( DDRB, 2 )
#define	SCL_PIN		SBIT( PINB, 2 )

#define	SDA		SBIT( PORTB, 0 )
#define	SDA_DDR		SBIT( DDRB, 0 )
#define	SDA_PIN		SBIT( PINB, 0 )

#endif
