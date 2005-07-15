
#ifndef NTC_TEMP_H
#define NTC_TEMP_H

#define DEBUG_MODE 1 

#ifndef DEBUG_MODE 
#define DEBUG_MODE 1 
#endif 

#if DEBUG_MODE 
#define dbg_printf printf 
#else 
#define dbg_printf 1>0 ? (void)0 : (void)printf 
#endif 

#ifndef P
#define P(s) ({static const uint8_t c[] __attribute__ ((progmem)) = s;c;})
#endif

// Some cool bitrelated stuff from Colin
#define bit_get(p,m) ((p) & (m)) 
#define bit_set(p,m) ((p) |= (m)) 
#define bit_clear(p,m) ((p) &= ~(m)) 
#define bit_flip(p,m) ((p) ^= (m)) 
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m)) 
#define BIT(x) (0x01 << (x)) 
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))
#define GET_BITS(num, from, to) (((num) >> (from)) & ((0x1 << ((to) - (from) + 1)) - 1)) 

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

//#define		STK500							// Set if STK500 is used (LED inverting)
#define		PGM_VERSION			"SMSC V0.96"
//#define		XTAL				11.0592e6	/* The cpu clock frequency in Hertz */
//#define		XTAL				14.7603e6	/* The cpu clock frequency in Hertz */
#undef		F_CPU
#define		F_CPU				XTAL   		/* The cpu clock frequency in Hertz */
// 
// Timer Related Timing
//
#define		CYCLES				1				// NUmber of 8 bit Timer Cycles to run before an TOIE0 IRQ 
#define		PRESCALER			1024			// Timer Prescaler
#define		CYCLETIME			(CYCLES * PRESCALER)

// Number of Timer0 IRQ's loops to get a 10ms Tick (the 1000/10) is 1000 to get 1 ms and there are 10 of them 
#define		LOOP_10MS			(uint8_t )(((F_CPU/PRESCALER) / (1000/10)) + 0.5)


#endif 		// #ifndef NTC_TEMP_H
