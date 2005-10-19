//Main.c
//Dual (bike) lamp controller.

#define ENABLE_BIT_DEFINITIONS
#include <io.h>
#include <..\libraries\avrx\avrx\avrx-io.h>
#include <..\libraries\avrx\avrx\avrx-signal.h>
#include <..\libraries\avrx\avrx\AvrX.h>

#include <avr\wdt.h>

#include <..\libraries\avrx\cserialio\serialio.h>
#include "stdio.h"
#include "ControlLamp.h"

#define CPUCLK   8000000L     // CPU xtal
#define TICKRATE 1000         // AvrX timer queue Tick rate

#define BAUDRATE 19200L       // Debug monitor baudrate

// Peripheral initialization 

#define TCNT0_INIT (0xFF-CPUCLK/256/TICKRATE)

// NB: Clock divisor bits are different between classic and mega103 chips!
// NB: IAR chooses to use different CPU identifiers for their C compiler

#if defined (__AVR_ATmega103__) || defined (__ATmega103__)

#define TMC8_CK256 ((1<<CS02) | (1<<CS01))

#elif defined (__AVR_AT90S8535__) || defined (__AVR_AT90S8515__) ||  \
	  defined (__AVR_ATmega163__) || defined (__AT90S8535__)     ||  \
	  defined (__AVR_ATmega161__) || \
	  defined (__AT90S8515__)     || defined (__ATmega163__)     ||  \
	  defined (__AVR_ATmega32__)  || defined (__AVR_ATmega32__)  ||  \
  	  defined (__AVR_ATmega8__)   || defined (__AVR_ATmega8__)  ||  \
	  defined (__AVR_ATmega323__) || defined (__ATmega323__)
	               
#define TMC8_CK256 (1<<CS02)

#endif


#define UBRR_INIT (CPUCLK/(16*BAUDRATE)-1)


#include <avr\sfr_defs.h>
#include <io.h>


void InitSerialIO(unsigned char);       // From Avrx.a (debug monitor)


AVRX_SIGINT(SIG_OVERFLOW0)
{
    IntProlog();                // Switch to kernel stack/context
    outp(TCNT0_INIT, TCNT0);
    AvrXTimerHandler();         // Call Time queue manager
    Epilog();                   // Return to tasks
}



AVRX_EXTERNTASK(taskLamp);

void StartupDelay(void)
{
	for (int i=0;i<2000;i++)
		for (int j=0;j<255;j++)
			asm("nop");
}


//------------------------------------------------------------------------------

int main(void)
{
	StartupDelay();
			
	
	wdt_enable(WDTO_250MS);

    AvrXSetKernelStack(0);
	
	fdevopen(&AVRXPutChar,&AVRXGetChar,0);	//printf supported by avrx fucntions 
											// see libc manual
	
	ACSR = BV(ACD);				// turn off ADC
	MCUCR = BV(SE);       		// Enable "Sleep" instruction
    TCNT0 = TCNT0_INIT;
    TCCR0 = TMC8_CK256;   		// Set up Timer0 for CLK/256 rate() 
    TIMSK = BV(TOIE0);    		// Enable Timer0 overflow interrupt
	
 	//InitSerialIO(UBRR_INIT);  // Initialize USART baud rate generator
 
	wdt_reset();
 	
    AvrXRunTask(TCB(taskLamp)); 

	Epilog();                   // Switch from AvrX Stack to first task
	return 0;

}
