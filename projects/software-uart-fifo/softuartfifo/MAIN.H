#ifndef _main_h_
#define _main_h_


#include <avr\pgmspace.h>
#include <avr\io.h>
#include <avr\interrupt.h>
#include "mydefs.h"


#define	XTAL	11.0592e6
#define	BAUD	57600

#define F_CPU   XTAL
#include <util\delay.h>

#define	STXD		SBIT( PORTA, PA6 )	// = OC1A
#define	STXD_DDR	SBIT( DDRA,  PA6 )

#define	SRXD_PIN	SBIT( PINA,  PA7 )	// = ICP


#endif
