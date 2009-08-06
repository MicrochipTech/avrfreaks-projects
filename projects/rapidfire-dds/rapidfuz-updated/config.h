/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */
/* vim:set ts=4:sw=4:sts=4:expandtab:cindent: */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define F_CPU 4000000UL		/* CPU clock in Hertz */
#define SOFTCLOCK_FREQ 300	/* internal software clock */
#define TIMER1_PRESCALE 255UL	/* hardware prescaler */

#define SOFT_SCALE (((F_CPU * 10) / (255UL * SOFTCLOCK_FREQ)+9)/10)

#if defined(__AVR_ATmega16__)

#	define DEBUG

#   define BUTTONDDR	DDRA
#   define BUTTONPORT	PORTA
#   define BUTTONIN	PINA
#   define PIN_MODE	PA0
#   define PIN_LTRIG	PA1
#   define PIN_RTRIG	PA2
#   define PIN_MAC0	PA3
#   define PIN_MAC1	PA4
#   define PIN_FAST	PA6
#   define PIN_SLOW	PA7

#   define LEDDDR	DDRB
#   define LEDPORT	PORTB
#   define LEDIN	PINB
#   define PIN_STATUS	PB0
#   define PIN_SCLOCK	PB1
#   define PIN_CLOCK	PB2
#   define PIN_ISR 	PB3

#   define PWMDDR	DDRD
#   define PWMPORT	PORTD
#   define PWMIN	PIND
#   define PWMA		PD5
#   define PWMB		PD4

#   define EEPROMSIZE 512

#elif defined(__AVR_ATmega13__)
#   warning "Lazy programer"
#   error "TODO"

#elif defined(__AVR_ATtiny45__)

#   define BUTTONDDR	DDRB
#   define BUTTONPORT	PORTB
#   define BUTTONIN	PINB
#   define PIN_MODE	PB4

#   define LEDDDR	DDRB
#   define LEDPORT	PORTB
#   define LEDIN	PINB
#	define PIN_STATUS PB2

#   define PWMDDR	DDRB
#   define PWMPORT	PORTB
#   define PWMIN	PINB
#   define PWMA		PB0
#   define PWMB		PB1


#   define EEPROMSIZE 256

#else
#   warning "Lazy programer"
#   error "Unsupported MCU type"
#endif

#endif
