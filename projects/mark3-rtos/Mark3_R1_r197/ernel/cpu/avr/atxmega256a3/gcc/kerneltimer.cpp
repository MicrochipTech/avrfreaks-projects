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

    \file   kerneltimer.cpp

    \brief  Kernel Timer Implementation for ATMega328p
*/

#include "kerneltypes.h"
#include "kerneltimer.h"

#include <avr/io.h>
#include <avr/interrupt.h>


//---------------------------------------------------------------------------
void KernelTimer::Config(void)
{        
    TCC1.CTRLA = 0;    
    TCC1.CTRLB = 0;     // Disable individual capture/compares on this timer since all we care about is overflow.
    TCC1.CTRLC = 0;     
    TCC1.CTRLD = 0x20;
            
    TCC1.CTRLE = 0x00;    // using word mode (not byte mode)

    TCC1.CTRLFCLR |= 0x0F;    // Reset the controller for upcount, no controller update
    TCC1.INTFLAGS |= 0x01;    // Clear interrupt on overflow

    TCC1.PER = 0; 
    TCC1.CNT = 0;
    
    TCC1.INTCTRLB = 0; // Disable ABCD Capture compare interrupts.
}

//---------------------------------------------------------------------------
void KernelTimer::Start(void)
{    
    TCC1.CTRLA = 0x05;    // Clock / 64
    TCC1.INTFLAGS |= 0x01;    // Clear interrupt on overflow
    TCC1.INTCTRLA |= 0x01;
}

//---------------------------------------------------------------------------
void KernelTimer::Stop(void)
{
    TCC1.INTFLAGS |= 0x01;    // Clear interrupt on overflow
    TCC1.INTCTRLA &= ~(0x01); // Disable the interrupt
    TCC1.CTRLA = 0;    // Clock source disable.
}

//---------------------------------------------------------------------------
K_USHORT KernelTimer::Read(void)
{
    volatile K_USHORT usRead1;
    volatile K_USHORT usRead2;
    
    do {
        usRead1 = TCC1.CNT;
        usRead2 = TCC1.CNT;            
    } while (usRead1 != usRead2);
    
    return usRead1;    
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SubtractExpiry(K_ULONG ulInterval_)
{
    TCC1.PER -= (K_USHORT)ulInterval_;        
    return (K_ULONG)TCC1.PER;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::TimeToExpiry(void)
{
    K_USHORT usRead = KernelTimer::Read();
    K_USHORT usOCR1A = TCC1.PER;

    if (usRead >= usOCR1A)
    {
        return 0;
    }
    else
    {
        return (K_ULONG)(usOCR1A - usRead);    
    }
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::GetOvertime(void)
{
    return KernelTimer::Read();
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SetExpiry(K_ULONG ulInterval_)
{
    K_USHORT usSetInterval;    
    if (ulInterval_ > 65535)
    {
        usSetInterval = 65535;
    } 
    else 
    {
        usSetInterval = (K_USHORT)ulInterval_ ;        
    }    
    TCC1.PER = usSetInterval;
    return (K_ULONG)usSetInterval;
}

//---------------------------------------------------------------------------
void KernelTimer::ClearExpiry(void)
{
    TCC1.PER = 65535; 
}

//---------------------------------------------------------------------------
K_UCHAR KernelTimer::DI(void)
{
    bool bEnabled = ((TCC1.INTCTRLA & 0X01) != 0);
    TCC1.INTCTRLA &= ~(0x01); // Disable the interrupt
    return bEnabled;
}

//---------------------------------------------------------------------------
void KernelTimer::EI(void)
{
    KernelTimer::RI(0);
}

//---------------------------------------------------------------------------
void KernelTimer::RI(bool bEnable_)
{
    if (bEnable_)    
    {
        TCC1.INTCTRLA |= (0x01); // Disable the interrupt
    }
    else
    {
        TCC1.INTCTRLA &= ~(0x01); // Disable the interrupt
    }    
}
