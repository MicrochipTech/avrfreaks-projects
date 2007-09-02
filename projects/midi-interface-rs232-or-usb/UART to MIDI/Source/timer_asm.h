#ifndef TIMER_ASM_H
#define TIMER_ASM_H

//#include "timer.h"
//#include "hardware.h"

// access this timer only with timer_getvalue().
extern unsigned long    	timer_value;

// use this to retrieve the actual software timer value
unsigned long timer_getvalue( void );


#endif

// EOF

