/*
 * defines.h
 *
 * Created: 9/2/2012 8:58:38 PM
 *  Author: daniel
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

#include <inttypes.h>

#ifndef boolean
typedef uint8_t boolean;
#endif
#ifndef byte
typedef uint8_t byte;
#endif
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define F_CPU 1000000UL
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)
#define bit_get(p,m1) ((p) & (m1))
#define bit_set(p,m1) ((p) |= (m1))
#define bit_clear(p,m1) ((p) &= ~(m1))
#define bit_write(c,p,m1) (c ? bit_set(p,m1) : bit_clear(p,m1))
#define bit(x) (0x01 << (x))
#define true 0==0
#define false 0!=0


#endif /* DEFINES_H_ */