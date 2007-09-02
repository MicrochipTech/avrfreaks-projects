#ifndef TIMER_H
#define TIMER_H

#include "timer_asm.h"

// typical tickfrequency in Hz of the software-timer
// Software-timer will be incremented when timer0 overruns, so
// timer0-overrun is 256, timer0 prescaler is set to 64, results in 16384UL
// use other prescalers to get finer timing granularitys.
	// #define TIMER_TICKFREQ 450L 	// @ 7,3728 Mhz
	// #define TIMER_TICKFREQ 225L	// @ 3,6864 MHz
	// #define TIMER_TICKFREQ 675L 	// @ 11 Mhz
	#define TIMER_TICKFREQ	(F_CPU / 16384UL) // (calculated using F_CPU from makefile)

// activates timing system (you need to enable interrupts as well!)	
void timer_init( void );

// Retrieves the time-difference between two timestamps.
// The first_timepoint must always be older than the second_timepoint to
// get correct return values. 
// t = timer_diff_time(old_timepoint, timer_getvalue());    // OK
// t = timer_diff_time(future_timepoint, timer_getvalue()); // Will not work!
// t = timer_diff_time(timer_getvalue(), old_timepoint);    // Will not work!
inline unsigned long timer_diff_time( unsigned long first_timepoint, unsigned long second_timepoint);

// Gibt die aktuelle, seit einem Zeitpunkt timepoint vergangenen timer-ticks zurück 
// Use this to retrieve the actual time elapsed since timepoint in timer-tick units
#define	timer_elapsed_time(timepoint)	timer_diff_time( (timepoint), timer_getvalue())

// use this, if you want a specific action delayed at least "time" timer-ticks.
// this returns true, if the delay is reached. At the same time as you
// retrieve true, the delay-handle will be reinitialized to use the function
// again. *handle must point to a variable initialized with 0 for proper operation.
// Please mind, that if you check this function not often enough, the delay can
// be longer than set by time, but it will never be shorter.
// Example to blink a LED with 1 Hz on PORTB0:
//      unsigned long thandle = 0;
//      while(1) { if(timer_autodelay(&thandle,TIMER_MS(500))) PORTB ^= 1; } 
unsigned char timer_autodelay(unsigned long *handle, unsigned long time);

// Returns value in timer_ticks for given time in ms
// Example: TIMER_MS(1000) => 450 , because 1s = 450 Ticks @ 7,3728 MHz
#define TIMER_MS(ms)	((ms * (F_CPU / 16384UL)) / 1000UL)  


#endif
// EOF


