//
// file: initializations.h
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
#ifndef _INITIALIZATIONS_H_
#define _INITIALIZATIONS_H_

#include "freertos_usart_driver.h"

void vInitClock(void);
void initUsarts(void);
void vPortPreparation(void);
void disableUnusedModules(void);

void setupTimerCC1Interrupt( void );
void setupTimerCD0Interrupt( void );
void setupTimerCD1Interrupt( void );
void setupTimerCE0Interrupt( void );



// USART data structs
extern USART_data_t USARTC0_data;
extern USART_data_t USARTC1_data;
extern USART_data_t USARTE0_data;


// Convenience macro for disabling the JTAG interface.
#define DISABLE_JTAG( ) { \
	AVR_ENTER_CRITICAL_REGION(); \
	CCP = 0xD8; \
	MCU_MCUCR = MCU_JTAGD_bm; \
	AVR_LEAVE_CRITICAL_REGION(); \
}

// Convenience macro for enabling pull-ups on specified pins on any port.
#define __PORT_PULLUP(port, mask) { \
	PORTCFG.MPCMASK = mask ; \
	port.PIN0CTRL = PORT_OPC_PULLUP_gc; \
}

#endif // initializations.h
