
// file serial_simulation.c
//
//	atxmega 32A4/128A1 port, VG Feb 2011
//
#include "all.h"


// The following code is used to simulate data coming in over a serial line.
// This simulation allows the project to be tested in the simulator.
//
#ifdef SERIAL_SIMULATION


#define portCLOCK_PRESCALER_TIMER1				( ( unsigned portLONG ) 64 )

// timerCC1 overflow interrupt service routine
//
ISR(TCC1_OVF_vect, ISR_NAKED)
{
	portSTART_ISR();

	// local variables cannot be used here,
	// because the framepointer (Y-reg) is not set!

	portEND_SWITCHING_ISR( USART_RXComplete(&USARTE0_data) );
}

// Setup timer CC1 to simulate serial input with 19200 baud
//
void setupTimerCC1Interrupt( void )
{

	unsigned portLONG ulOvfMatch;

	u16 baudrate = 19200;

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
//	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_LO_gc);	//low-level interrupt
	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_MED_gc);	//medium-level interrupt 
//	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_HI_gc);	//high-level interrupt
}

//-----------------------------------------------------------

// timerCE0 overflow interrupt service routine
//
ISR(TCE0_OVF_vect, ISR_NAKED)
{
	portSTART_ISR();

	// local variables cannot be used here,
	// because the framepointer (Y-reg) is not set!

	portEND_SWITCHING_ISR( USART_RXComplete(&USARTC1_data) );
}

// Setup timer counter E0 to simulate serial input 38400 baud
//
void setupTimerCE0Interrupt( void )
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
	TC_SetPeriod( &TCE0, ulOvfMatch);
		
	/* Setup clock source and compare match behaviour. */
	TC0_ConfigClockSource( &TCE0, TC_CLKSEL_DIV64_gc);

	/* Enable the interrupt - this is okay as interrupts are currently globally
	disabled. */
//	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_LO_gc);	//low-level interrupt
	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_MED_gc);	//medium-level interrupt 
//	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_HI_gc);	//high-level interrupt
}

#endif // SERIAL_SIMULATION 

// eof serial_simulation.c
