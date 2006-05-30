#ifndef _AVR_UART_GUARD_H_
#define _AVR_UART_GUARD_H_

/************************************************************************
 *
 * File: uart_guard.h
 * Author: Artur Lipowski <LAL@pro.onet.pl>
 * Version: 3.0, May 27 2006
 * Copyright 2004-2006 Artur Lipowski
 * 
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ************************************************************************/


/** \file uart_guard.h
 *  \brief A header file with preprocessor magic needed to support 
 *  different types of devices.
 *
 */


#if 0
#include <avr/version.h>

#if !defined(__AVR_LIBC_VERSION__) || __AVR_LIBC_VERSION__ <  10400UL
#error At least version 1.4.0 of the avr-libc is required.
#endif
#endif

#if defined (__AVR_ATmega128__) ||	\
	defined (__AVR_ATmega161__) ||	\
	defined (__AVR_ATmega162__) ||	\
	defined (__AVR_ATmega164__) ||	\
	defined (__AVR_ATmega324__) ||	\
	defined (__AVR_ATmega640__) ||	\
	defined (__AVR_ATmega1280__) ||	\
	defined (__AVR_ATmega1281__) ||	\
	defined (__AVR_ATmega64__)

/* AVR types above have two USARTs */

#define USE_USART 1
#define HAS_2UARTS 1

#ifndef USE_UART0
#ifndef USE_UART1
#error "This AVR type has two UARTS - you must define (in Makefile) USE_UART0 and/or USE_UART1"
#endif
#endif

#elif defined (__AVR_ATmega32__) ||	\
	defined (__AVR_ATmega323__) ||	\
	defined (__AVR_ATmega325__) ||	\
	defined (__AVR_ATmega3250__) ||	\
	defined (__AVR_ATmega644__) ||	\
	defined (__AVR_ATmega645__) ||	\
	defined (__AVR_ATmega6450__) ||	\
	defined (__AVR_ATmega16__) ||	\
	defined (__AVR_ATmega169__) ||	\
	defined (__AVR_ATmega8__) ||	\
	defined (__AVR_ATmega48__) ||	\
	defined (__AVR_ATmega88__) ||	\
	defined (__AVR_ATmega165__) ||	\
	defined (__AVR_ATmega168__) ||	\
	defined (__AVR_AT90PWM2__) ||	\
	defined (__AVR_AT90PWM3__) ||	\
	defined (__AVR_ATmega8515__) ||	\
	defined (__AVR_ATmega8535__)

/* AVR types above have only one USART */

#define USE_USART 1
#undef HAS_2USARTS

#define USE_UART0 1
#undef USE_UART1

#if defined (UBRR0L)
#define UBRRL UBRR0L
#define UBRRH UBRR0H
#define UCSRA UCSR0A
#define UCSRB UCSR0B
#define UCSRC UCSR0C
#define UDR UDR0
#endif

#elif defined (__AVR_AT43USB320__) ||	\
	defined (__AVR_ATmega103__) ||	\
	defined (__AVR_AT90S8535__) ||	\
	defined (__AVR_AT90S8515__) ||	\
	defined (__AVR_AT90S4434__) ||	\
	defined (__AVR_AT90S4414__) ||	\
	defined (__AVR_AT90S2313__)

/* AVR types above have only one UART */
	
#undef USE_USART
#undef HAS_2USARTS

#define USE_UART0 1
#undef USE_UART1

#else
#error "Device type not supported"
#endif 


#if defined(SKIP_DEVNO_CHECK)
#if defined(HAS_2UARTS) && defined(USE_UART1) && defined(USE_UART0)
#undef SKIP_DEVNO_CHECK
#endif
#endif

#if defined(UPE)
#define PE UPE
#else
#if !defined(PE)
#define PE 0
#endif
#endif

#if defined (RXCIE0) || defined (RXCIE1)
#if !defined (RXCIE)
#define RXCIE RXCIE0
#endif
#endif
    
#if defined (TXCIE0) || defined (TXCIE1)
#if !defined (TXCIE)
#define TXCIE TXCIE0
#endif
#endif

#if defined (UDRIE0) || defined (UDRIE1)
#if !defined (UDRIE)
#define UDRIE UDRIE0
#endif
#endif

#if defined (UDRE0) || defined (UDRE1)
#if !defined (UDRE)
#define UDRE UDRE0
#endif
#endif

#if defined (RXEN0) || defined (RXEN1)
#if !defined (RXEN)
#define RXEN RXEN0
#endif
#endif

#if defined (TXEN0) || defined (TXEN1)
#if !defined (TXEN)
#define TXEN TXEN0
#endif
#endif

#if defined (FE0) || defined (FE1)
#if !defined (FE)
#define FE FE0
#endif
#endif

#endif /* _AVR_UART_GUARD_H_ */
