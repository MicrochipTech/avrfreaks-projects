
#ifndef _delay_h_
#define _delay_h_

#include <inttypes.h>
#include <avr/io.h>

/* delay function for microsec
   4 cpu cycles per loop + 1 cycles(?) overhead 
   when a constant is passed. */
static inline void delayloop16(uint16_t count)
{
	asm volatile (  "cp  %A0,__zero_reg__ \n\t"  \
                     "cpc %B0,__zero_reg__ \n\t"  \
                     "breq L_Exit_%=       \n\t"  \
                     "L_LOOP_%=:           \n\t"  \
                     "sbiw %0,1            \n\t"  \
                     "brne L_LOOP_%=       \n\t"  \
                     "L_Exit_%=:           \n\t"  \
                     : "=w" (count)
					 : "0"  (count)
                   );                            
}
// delayloop16(x) eats 4 cycles per x
#define DELAY_US_CONV(us) ((uint16_t)(((((us)*1000L)/(1000000000/F_OSC))-1)/4))
#define delay_us(us)	  delayloop16(DELAY_US_CONV(us))

/* delay function for millisec
  (6 cycles per x + 20(?) overhead) */
void delayloop32( uint32_t l); // not inline
#define DELAY_MS_CONV(ms) ( (uint32_t) (ms*(F_OSC/6000L)) ) 
#define delay_ms(ms)  delayloop32(DELAY_MS_CONV(ms))

/* mth 9/04:
   Remark uSeconds:
   Main Oscillator Clock given by F_OSC (makefile) in Hz
   one CPU-Cycle takes 1/F_OSC seconds => 1000000/F_OSC uSeconds
   so: 1 uSecond takes F_OSC/1000000 CPU-Cyles. The following code
   is inspired by the avr-libc delay_loop2 function.
   This it not "that precise" since it takes at least 4 cycles
   but should be o.k. with any parameter (even 0).
   Call function with delayloop(DELAYUS(dt [in uSeconds])).
*/ 

#endif
