//
// file serial_simulation.c
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
#include "all.h"

// The atxmega 32A4 cpu has 5 timers:
//
// TCC0		this one is used for the tick interrupt.
// TCC1		used for serial simulation
// TCD0		nested interrupt test (med level)
// TCD1		nested interrupt test (high level)
// TCE0		used for serial simulation

// The following code is used to simulate data coming in over a serial line.
// This simulation allows the project to be tested in the simulator.
//

#define portCLOCK_PRESCALER_TIMER1				( ( unsigned portLONG ) 64 )

#if SERIAL_SIMULATION_C1 == 1
//
// timer/counter-C1 overflow interrupt service routine
//
ISR(TCC1_OVF_vect, ISR_NAKED)
{
	portSTART_ISR();

	// local variables cannot be used here,
	// because the framepointer (Y-reg) is not set!

	portEND_SWITCHING_ISR( USART_RXComplete_sim(&USARTC1_data) );
}

// Setup timer/counter-C1 to simulate serial input
//
void setupTimerCC1Interrupt( void )
{

	unsigned portLONG ulOvfMatch;

	u16 baudrate = USARTC1_BAUDRATE;

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

	// Enable the interrupt - this is ok as interrupts are currently globally disabled.
    // The interrupt level used here MUST NOT be higher than configMAX_SYSCALL_INTERRUPT_PRIORITY !
    //
#if configMAX_SYSCALL_INTERRUPT_PRIORITY == 0
//  only one option:
	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_LO_gc);	//low-level interrupt
#elif configMAX_SYSCALL_INTERRUPT_PRIORITY == 1
//  two possible options:
	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_LO_gc);	//low-level interrupt
//	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_MED_gc);	//medium-level interrupt 
#elif configMAX_SYSCALL_INTERRUPT_PRIORITY == 2
//  three possible options:
//	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_LO_gc);	//low-level interrupt
	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_MED_gc);	//medium-level interrupt 
//	TC1_SetOverflowIntLevel( &TCC1, TC_OVFINTLVL_HI_gc);	//high-level interrupt
#endif
}
#endif

//-----------------------------------------------------------

#if SERIAL_SIMULATION_E0 == 1
//
// timer/counter E0 overflow interrupt service routine
//
ISR(TCE0_OVF_vect, ISR_NAKED)
{
	portSTART_ISR();

	// local variables cannot be used here,
	// because the framepointer (Y-reg) is not set!

	portEND_SWITCHING_ISR( USART_RXComplete_sim(&USARTE0_data) );
}

// Setup timer counter E0 to simulate serial input 38400 baud
//
void setupTimerCE0Interrupt( void )
{

	unsigned portLONG ulOvfMatch;

	u16 baudrate = USARTE0_BAUDRATE;

	ulOvfMatch = configCPU_CLOCK_HZ / (baudrate / 10); //data rate for usart simulation

	/* We only have 16bits so have to scale to get our required tick rate. */
	ulOvfMatch /= portCLOCK_PRESCALER_TIMER1;

	/* Adjust for correct value. (because overflow clear if CNT > PER) */
	ulOvfMatch -= ( unsigned portLONG ) 1;

	/* Setup overflow value for TCC1.  Interrupts are disabled 
	before this is called so we need not worry here. */
	TC_SetPeriod( &TCE0, ulOvfMatch);
		
	/* Setup clock source and compare match behavior. */
	TC0_ConfigClockSource( &TCE0, TC_CLKSEL_DIV64_gc);

	// Enable the interrupt - this is ok as interrupts are currently globally disabled.
    // The interrupt level used here MUST NOT be higher than configMAX_SYSCALL_INTERRUPT_PRIORITY !
    //
#if configMAX_SYSCALL_INTERRUPT_PRIORITY == 0
//  only one option:
	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_LO_gc);	//low-level interrupt
#elif configMAX_SYSCALL_INTERRUPT_PRIORITY == 1
//  two possible options:
//	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_LO_gc);	//low-level interrupt
	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_MED_gc);	//medium-level interrupt 
#elif configMAX_SYSCALL_INTERRUPT_PRIORITY == 2
//  three possible options:
//	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_LO_gc);	//low-level interrupt
//	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_MED_gc);	//medium-level interrupt 
	TC0_SetOverflowIntLevel( &TCE0, TC_OVFINTLVL_HI_gc);	//high-level interrupt
#endif
}

#endif // SERIAL_SIMULATION 

// eof serial_simulation.c
