#ifndef main_h_
#define main_h_

#include "sbit.h"

// Target: ATmega48
// 			hardware definitions:

#define XTAL            	1e6             	// 1MHz

#define KEY_PIN			PIND
#define KEY_ON_OFF 		0
#define	KEY_ON_OFF_PCI		PCINT16			// = PD0
#define	KEY_ON_OFF_PCI_REG	PCMSK2			// for PCINT16..23
#define	KEY_ON_OFF_ENABLE	PCIE2			// for PCINT16..23
#define	WAKEUP_vect		PCINT2_vect		// wakeup interrupt

#define LED_0			SBIT( PORTB, 0 )
#define LED_0_DDR		SBIT( DDRB,  0 )

#define LED_1			SBIT( PORTB, 1 )
#define LED_1_DDR		SBIT( DDRB,  1 )

#define LED_ON			0 			// low active
#define	LED_OFF			(!LED_ON)

#endif

