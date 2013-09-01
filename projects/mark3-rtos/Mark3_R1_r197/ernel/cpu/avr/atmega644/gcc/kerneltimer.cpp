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

#define TCCR1B_INIT        ((1 << WGM12) | (1 << CS12))
#define TIMER_IMSK        (1 << OCIE1A)
#define TIMER_IFR        (1 << OCF1A)

//---------------------------------------------------------------------------
void KernelTimer::Config(void)
{        
    TCCR1B = TCCR1B_INIT;
}

//---------------------------------------------------------------------------
void KernelTimer::Start(void)
{
    TCNT1 = 0;
    TIFR1 &= ~TIMER_IFR;
    TIMSK1 |= TIMER_IMSK;
    TCCR1B |= (1 << CS12);        // Enable count...
}

//---------------------------------------------------------------------------
void KernelTimer::Stop(void)
{
    TIFR1 &= ~TIMER_IFR;
    TIMSK1 &= ~TIMER_IMSK;
    TCCR1B &= ~(1 << CS12);      // Disable count...    
    TCNT1 = 0;
    OCR1A = 0;
}

//---------------------------------------------------------------------------
K_USHORT KernelTimer::Read(void)
{
    volatile K_USHORT usRead1;
    volatile K_USHORT usRead2;
    
    do {
        usRead1 = TCNT1;
        usRead2 = TCNT1;            
    } while (usRead1 != usRead2);
    
    return usRead1;    
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SubtractExpiry(K_ULONG ulInterval_)
{
    OCR1A -= (K_USHORT)ulInterval_;        
    return (K_ULONG)OCR1A;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::TimeToExpiry(void)
{
    K_USHORT usRead = KernelTimer::Read();
    K_USHORT usOCR1A = OCR1A;

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
    OCR1A = usSetInterval;
    return (K_ULONG)usSetInterval;
}

//---------------------------------------------------------------------------
void KernelTimer::ClearExpiry(void)
{
    OCR1A = 65535;                    // Clear the compare value
}

//---------------------------------------------------------------------------
K_UCHAR KernelTimer::DI(void)
{
    bool bEnabled = ((TIMSK1 & (TIMER_IMSK)) != 0);
    TIFR1 &= ~TIMER_IFR;      // Clear interrupt flags
    TIMSK1 &= ~TIMER_IMSK;    // Disable interrupt
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
        TIMSK1 |= (1 << OCIE1A);    // Enable interrupt
    }
    else
    {
        TIMSK1 &= ~(1 << OCIE1A);
    }    
}
