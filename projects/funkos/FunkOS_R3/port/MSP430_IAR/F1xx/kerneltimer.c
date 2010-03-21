#include "types.h"
#include "kernelcfg.h"

#include <io430.h>
#include <in430.h>

//---------------------------------------------------------------------------
void KernelTimer_Config(void)
{	
    TACTL = 0;                        // Reset the register
    TAR = 0;                          // Clear Timer A
    TACTL |= TACLR;                   // Reset the clock divider, etc.
    TACTL |= TASSEL_2;                // Set the timer to use SMCLK
    TACTL &= ~TAIFG;                  // Clear any pending interrupts
    TACCTL0 &= ~CCIFG;                // Clear pending interrupts
    TACCR0 = SYSTEM_FREQ / TICK_FREQ; // Set interrupts to occur at tick freq.
}

//---------------------------------------------------------------------------
void KernelTimer_Start(void)
{
    TACTL |= MC_1;                  // Set timer mode to count to TACCR0
    TACTL &= ~TAIFG;                // Clear any pending interrupt on timer A
    TACCTL0 &= ~CCIFG;              // Clear pending interrupts on the 
    TACCTL0 |= CCIE;                // Enable interrupts on TimerA0 CCR
}

//---------------------------------------------------------------------------
void KernelTimer_Stop(void)
{
    TACCTL0 &= ~CCIE;               // Disable TA0 CCR
    TACTL &= ~TAIE;                 // Disable TimerA interrupts.
}
