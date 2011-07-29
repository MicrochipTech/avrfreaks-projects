/* 
 * Lars-Lib, quick library for common atmel functions
 * 
 * V1.0  
 * 
 * By: Lars Ivar Miljeteig, August 2009
 * Platform: AVR ATmega8 (avr-gcc)
 * Dependencies: avr/io avr/interrupt delay.h stdint.h
 *
 * A small, simple library for second delays, blinking etc. 
 */



#ifndef LARSLIB_H
#define LARSLIB_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#define LI_MHZ F_CPU/1000000UL
#define LI_PRESCALER 16000000UL/F_CPU

// The prescaler ensures similar behaviour
//  for 8Mhz and 16Mhz operating mode


// Blinking pin definitions
#define BLINKPIN PB1
#define BLINKPORT PORTB
#define BLINKDDR DDRB

void blink(uint8_t bl);

void _delay_s(uint8_t sec);

void scopeout(uint8_t byte);

#endif // LARSLIB_H
