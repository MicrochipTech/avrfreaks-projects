//
// file nested_isr.c
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
#include "all.h"

#define portCLOCK_PRESCALER_TIMER	( ( unsigned portLONG ) 64 )



// timer/counter-D0 overflow interrupt service routine
// This medium-level isr shall never call the FreeRTOS api!
//
u32 medIsrCnt;

ISR(TCD0_OVF_vect)
{
	// This non-naked isr has a framepointer and local variables
	// my be used.

	medIsrCnt++;
}

// Setup timer/counter-D0 to test nested medium-level interrupts
//
void setupTimerCD0Interrupt( void )
{

	u32 ulOvfMatch;

	const u32 timerPeriod = 200; // usec
	
	// Adjust for correct value. (because overflow clear if CNT > PER)
	ulOvfMatch = configCPU_CLOCK_HZ / 1000000 * timerPeriod / portCLOCK_PRESCALER_TIMER - 1;

	// Setup overflow value for TCD0
	TC_SetPeriod( &TCD0, ulOvfMatch);
		
	// Setup clock source and compare match behaviour.
	TC0_ConfigClockSource( &TCD0, TC_CLKSEL_DIV64_gc);

	// Enable the medium-level interrupt.
	TC0_SetOverflowIntLevel( &TCD0, TC_OVFINTLVL_MED_gc);
}

// Timer/counter-D1 overflow interrupt service routine.
// This high-level isr shall never call the FreeRTOS api!
//
u32 highIsrCnt;


ISR(TCD1_OVF_vect)
{
	// This non-naked isr has a framepointer and local variables
	// my be used.

	highIsrCnt++;
}

// Setup timer/counter-D1 to to test nested high-level interrupts
//
void setupTimerCD1Interrupt( void )
{

	u32 ulOvfMatch;

	const u32 timerPeriod = 110; // usec
	
	// Adjust for correct value. (because overflow clear if CNT > PER)
	ulOvfMatch = configCPU_CLOCK_HZ / 1000000 * timerPeriod / portCLOCK_PRESCALER_TIMER - 1;

	// Setup overflow value for TCD1.
	TC_SetPeriod( &TCD1, ulOvfMatch);
		
	// Setup clock source and compare match behaviour.
	TC1_ConfigClockSource( &TCD1, TC_CLKSEL_DIV64_gc);

	// Enable the high-level interrupt.
	TC1_SetOverflowIntLevel( &TCD1, TC_OVFINTLVL_HI_gc);
}

// eof nested_isr.c
