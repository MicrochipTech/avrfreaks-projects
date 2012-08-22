
// file serial_simulation.c
//
//	xmega 32A4 port, VG Oct 2010
//
#include "all.h"


// The following code is used to simulate data coming in over a serial line.
// This simulation allows the project to be tested in the simulator.
//
#ifdef SERIAL_SIMULATION


#define portCLOCK_PRESCALER_TIMER1				( ( unsigned portLONG ) 64 )


// timer overflow interrupt service routine
//
ISR(TCC1_OVF_vect, ISR_NAKED)
{
	portSTART_ISR();

	portEND_SWITCHING_ISR( USART_RXComplete(&USARTE0_data) );
}

// Setup timer C1 to generate to simulate serial input
//
void setupTimer1Interrupt( void )
{

	unsigned portLONG ulOvfMatch;

	u16 baudrate = 38400;

	ulOvfMatch = configCPU_CLOCK_HZ / (baudrate / 10); //data rate for usart simulation

	/* We only have 16bits so have to scale to get our required tick rate. */
	ulOvfMatch /= portCLOCK_PRESCALER_TIMER1;

	/* Adjust for correct value. (because overflow clear if CNT > PER) */
	ulOvfMatch -= ( unsigned portLONG ) 1;

	/* Setup overflow value for TCC1.  Interrupts are disabled 
	before this is called so we need not worry here. */
	TC_SetPeriod( &TCC1, ulOvfMatch);
		
	/* Setup clock source and compare match behaviour. */
	TC1_ConfigClockSource( &TCC1, TC_CLKSEL_DIV64_gc);

	/* Enable the interrupt - this is okay as interrupts are currently globally
	disabled. */
	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_LO_gc);	//low interrupt priority
}

#endif // SERIAL_SIMULATION 

// eof serial_simulation.c
