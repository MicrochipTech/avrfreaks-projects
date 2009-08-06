/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define F_CPU 4000000UL	/* CPU clock in Hertz */

#define SOFTCLOCK_FREQ 2500	/* internal software clock */

#define SOFT_SCALE ((F_CPU * 10) / (2UL * SOFTCLOCK_FREQ) + 9) / 10

#if defined(__AVR_ATtiny45__)

#   define PWMDDR	DDRB
#   define PWMPORT	PORTB
#   define PWMIN	PINB
#   define PWMA		PB0
#   define PWMB		PB1

#elif defined(__AVR_ATmega13__)
#   warning "Lazy programer"
#   error "TODO"

#else
#   warning "Lazy programer"
#   error "Unsupported MCU type"
#endif

#endif
