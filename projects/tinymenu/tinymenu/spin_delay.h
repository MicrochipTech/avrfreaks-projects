/*
 * spin_delay.h -- static inline functions for relatively precise timing
 *  Currently only AVR code; other processors' timing loops may be
 *  added as I have time & need for them.
 *
 * (c) 2004 Tymm Twillman (tymm@booyaka.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#ifndef __SPIN_DELAY_H__
#define __SPIN_DELAY_H__

#include "stddef.h"

#ifdef AVR

/*
 * Basic spin delay functions
 */
 
# define LOOPS_PER_MS ((MCU_CLK/1000)/4)

# if (MCU_CLK >= 4000000)

# define LOOPS_PER_US ((MCU_CLK/1000000)/4)

/* Spin for about (us) microseconds 
 * The inner loop takes 4 cycles per iteration 
 */ 

static inline void us_spin(unsigned short us) 
{
	if (us) {                                   
		__asm__ __volatile__ (                          
			"outer_%=: "
			"	ldi r26, %2     \n"   
			"inner_%=: "
			"	dec r26         \n"     
			"	brne inner_%=   \n"     
			"	sbiw %0, 1      \n"     
			"	brne outer_%=   \n"     
 			: "=w" (us)                     
			: "w" (us), "i" (LOOPS_PER_US)  
			: "r26"                        
		);                                      
	}                                           
}

# else // MCU_CLK < 4000000

# define LOOPS_PER_8US (MCU_CLK / 500000)

/* Spin for about (us) microseconds (slow CPU clock version) 
 *    -- This actually does it in 8 us increments 	
 * the inner loop takes 4 cycles per iteration 
 *
 * The us divide will actually add a few cycles extra...
 */


static inline void us_spin(unsigned short us) {
	if (us/=8) { 
		__asm__ __volatile__ (
			"outer_%=: "
			"	ldi r26, %2     \n"   
			"inner_%=: "
			"	dec r26         \n"    
			"	brne inner_%=   \n"     
			"	sbiw %0, 1      \n"     
			"	brne outer_%=   \n"     
			: "=w" (us)
			: "w" (us), "i" (LOOPS_PER_8US)
			: "r26"
		);
	} 
}	

# endif // if (MCU_CLK >= 4000000)


/*
 * Spin for about (ms) milliseconds 
 *
 */


static inline void ms_spin(unsigned short ms) {
	if (ms) {
		__asm__ __volatile__ (
			"outer_%=:" 
			"	ldi r26, %3     \n"
			"	ldi r27, %2     \n"
			"inner_%=:"
			"	sbiw r26, 1     \n"
			"	brne inner_%=   \n"
			"	sbiw %0, 1      \n"
			"	brne outer_%=   \n"
			: "=w" (ms)
			: "w" (ms), 
			  "i" (LOOPS_PER_MS >> 8), 
			  "i" (0xff & LOOPS_PER_MS)
			: "r26", "r27"
		);
	}
}

#endif // #ifdef AVR

#endif // #ifndef __SPIN_DELAY_H__
