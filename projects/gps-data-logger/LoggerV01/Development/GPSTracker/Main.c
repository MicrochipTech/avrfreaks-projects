
#define ENABLE_BIT_DEFINITIONS
#include <inttypes.h>
#include <avr\io.h>
#include <avr\sfr_defs.h>
#include <avr\pgmspace.h>

#include <..\libraries\avrx\avrx\avrx-io.h>
#include <..\libraries\avrx\avrx\avrx-signal.h>
#include <..\libraries\avrx\avrx\AvrX.h>

#include "PCSerialIO.h"
#include "ProjectCommon.h"
#include "..\libraries\SPI\MultiThread\SPILib.h"
							

// Peripheral initialization (see header for numbers)
 
#if (CPUCLK/1024/TICKRATE >= 254)
	clock rate error, need to adjust pre scalers /clock divisors etc
	this value must be (well) below 255,
#endif
#define TCNT0_INIT (0xFF-CPUCLK/1024/TICKRATE)				// tick rates >=~100, @ >14Mhz clk
#define TMC1024_CK ((1<<CS02) | (1<<CS01) | (1<<CS00))		// div 1024


uint8_t gResetCause;			//cause of last reset


void InitSerialIO(unsigned char); 

//AVRX timer signal handler
//See AVRX doco

AVRX_SIGINT(SIG_OVERFLOW0) 
{
    IntProlog();                // Switch to kernel stack/context
	TCNT0 = TCNT0_INIT;
    AvrXTimerHandler();         // Call Time queue manager
    Epilog();                   // Return to tasks
}

// The two tasks used by this application

AVRX_GCC_TASK(TaskUserInterface,260, TSKPRI_taskUserInterface);
AVRX_GCC_TASK(TaskLogData,      160, TSKPRI_taskLogData);
									// BUG: dont use AVRX_EXTERNTASK, as Y reg aint set 
									// in called fn

extern TimerControlBlock UIdelay;	// temp control block for any delay routines

int main(void)
{
		
	gResetCause = MCUCSR;		//get the reset cause
	MCUCSR=0;
	
	AvrXSetKernelStack(0);
	
	
	MCUCR = BV(SE);       		// Enable "Sleep" instruction
    TCNT0 = TCNT0_INIT;
    TCCR0 = TMC1024_CK;   		// Set up Timer0 for CLK/1024 rate(
    TIMSK = BV(TOIE0);    		// Enable Timer0 overflow interrupt
	
	SPIInit();					// Multithreaded SPI initialisation for all threads		

	PCSIOInitSerialIO();		// init PC serial comms
								// see libc manual

//	PCSIOPutStr_P(PSTR("Build: "));
//	PCSIOPutStr_P(PSTR(__DATE__));
//	PCSIOPutChar(' ');
//	PCSIOPutStr_P(PSTR(__TIME__));
//	PCSIOPutChar('\n');

	AvrXDelay(&UIdelay,MStoTickRate(1000));		//delay, allow everything to settle

    AvrXRunTask(TCB(TaskUserInterface)); 
    AvrXRunTask(TCB(TaskLogData)); 

	Epilog();                   				// Switch from AvrX Stack to first task
	return 0; 

}
