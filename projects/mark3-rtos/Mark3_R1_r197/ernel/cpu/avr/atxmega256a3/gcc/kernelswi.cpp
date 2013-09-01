/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!

    \file   kernelswi.cpp

    \brief  Kernel Software interrupt implementation for ATMega328p

*/

#include "kerneltypes.h"
#include "kernelswi.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define TIMER_SWI_EVENT_CHANNEL        (0x00)
#define TIMER_SWI_EVENT_FLAG        (0x08)

//---------------------------------------------------------------------------
void KernelSWI::Config(void)
{
    TCC0.CTRLA = 0;
    TCC0.CTRLB = 0;         // Disable individual capture/compares on this timer since all we care about is overflow.
    TCC0.CTRLC = 0;     
    TCC0.CTRLD = 0x20 | (TIMER_SWI_EVENT_CHANNEL | TIMER_SWI_EVENT_FLAG); // capture compare enabled
            
    TCC0.CTRLE = 0x00;        // using word mode (not byte mode)

    TCC0.CTRLFCLR |= 0x0F;    // Reset the controller for upcount, no controller update

    TCC0.INTFLAGS |= 0x01;    // Clear interrupt on overflow

    TCC0.PER = 1;            //!! ToDo - check to see if we can get away with using a period of 0...
    
    TCC0.INTCTRLB = 0;         // Disable ABCD Capture compare interrupts.
    
    EVSYS.CH0MUX = 0;        // Manual event generation
    EVSYS.CH0CTRL = 0;      // Single-sample events.
}

//---------------------------------------------------------------------------
void KernelSWI::Start(void)
{        
    PMIC.CTRL |= 0x07;        // Enable all priority levels of interrupt.
    TCC0.CTRLA = (TIMER_SWI_EVENT_CHANNEL | TIMER_SWI_EVENT_FLAG);    // Clock source:  SWI Event 
    TCC0.INTFLAGS |= 0x01;    // Clear interrupt on overflow
    TCC0.INTCTRLA |= 0x03;
}

//---------------------------------------------------------------------------
void KernelSWI::Stop(void)
{
       TCC0.INTFLAGS |= 0x01;        // Clear interrupt on overflow
    TCC0.INTCTRLA &= ~(0x03);     // Disable the interrupt
    TCC0.CTRLA = 0;                // Clock source:  SWI Event 
}

//---------------------------------------------------------------------------
K_UCHAR KernelSWI::DI()
{
    bool bEnabled = ((TCC0.INTCTRLA & 0x03) != 0);
    TCC0.INTCTRLA &= ~0x03;
    return bEnabled;
}

//---------------------------------------------------------------------------
void KernelSWI::RI(bool bEnable_)
{
    if (bEnable_)
    {
        TCC0.INTCTRLA |= 0x03;
    }
    else
    {
        TCC0.INTCTRLA &= ~0x03;
    }
}

//---------------------------------------------------------------------------
void KernelSWI::Clear(void)
{
    TCC0.INTFLAGS |= 0x01;        // Clear interrupt on overflow
}

//---------------------------------------------------------------------------
void KernelSWI::Trigger(void)
{
    //if(Thread_IsSchedulerEnabled())
    {
        // Sending two events on channel 0 sets the interrupt condition.
        // Test to see if we can generate that condition using 1 event strobe
        // and a timer period of zero.  This works though.
        EVSYS.DATA |= 0x00;
        EVSYS.STROBE |= 0x01;

        EVSYS.DATA |= 0x00;
        EVSYS.STROBE |= 0x01;  
    }    
}
