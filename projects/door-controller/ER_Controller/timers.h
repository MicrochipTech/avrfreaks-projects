/*
 * myStuff.h
 *
 * Created: 7/25/2012 2:07:38 PM
 *  Author: dmh
 */ 

#ifndef TIMERS_H_
#define TIMERS_H_

#include <avr/interrupt.h>

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

#define nop __asm__("nop\n\t");
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
#define BIT(x) (0x01 << (x))

volatile unsigned long dr1_timer0_millis = 0, dr2_timer0_millis = 0;
static unsigned char dr1_timer0_fract = 0, dr2_timer0_fract = 0;

// Timer0 overflow interrupt
ISR(TIMER0_OVF_vect)
{
	unsigned long m1 = dr1_timer0_millis, m2 = dr2_timer0_millis;
	unsigned char f1 = dr1_timer0_fract, f2 = dr2_timer0_fract;
	
	m1 += MILLIS_INC; m2 += MILLIS_INC;
	f1 += FRACT_INC; f2 += FRACT_INC;

	if (f1 >= FRACT_MAX) {
		f1 -= FRACT_MAX;
		m1 += 1;
	}
	if (f2 >= FRACT_MAX) {
		f2 -= FRACT_MAX;
		m2 += 1;
	}
	dr1_timer0_fract = f1; dr2_timer0_fract = f2;
	dr1_timer0_millis = m1; dr2_timer0_millis = m2;
}

unsigned long door1_timer(boolean reset) {
	if(reset) { // Reset the Door 1 timer
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			dr1_timer0_millis = 0;
			dr1_timer0_fract = 0;
		}		
		return 0;
	}
	unsigned long m1;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		m1 = dr1_timer0_millis;
	}
	return m1;
}

unsigned long door2_timer(boolean reset) {
	if(reset) { // Reset the Door 2 timer
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			dr2_timer0_millis = 0;
			dr2_timer0_fract = 0;
		}		
		return 0;
	}
	unsigned long m2;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		m2 = dr2_timer0_millis;
	}
	return m2;
}

#endif /* TIMERS_H_ */