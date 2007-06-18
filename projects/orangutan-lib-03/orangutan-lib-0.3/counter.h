#ifndef _COUNTERS_
#define _COUNTERS_

// How many counters of each kind are we willing to service?  Really 
// the user should set this since we can handle way more than the 
// default of four.  But if you're only using one counter, no point 
// in servicing four of them.

#define	COUNTER_MAX		4

// What fraction of a second do we want our counters to increment?
// A value of 1000 means 1000 interrupts per second, and our counters
// count in milliseconds.  A value of 100 means 100 interrupts per
// second and our counters count in hundredths of a second.  You get
// the idea.
//
// WARNING:  You need the value of F_CPU / COUNTER_DIVISOR to be an
// integer, or you will have inaccurate timing!  It's the integer
// portion of that expression that determines how many clock cycles
// to wait between interrupts.

#define COUNTER_DIVISOR	1000

// A routine to initialize the counter subsystem
void counter_init(void);


// Register a count-down timer in the first available slot
void counter_define_down(uint16_t *timer);


// Register a count-up timer in the first available slot
void counter_define_up(uint16_t *timer);

#endif // _COUNTERS_
