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

    \file   kerneltimer.h    

    \brief  Kernel Timer Class declaration
*/

#include "kerneltypes.h"
#ifndef __KERNELTIMER_H_
#define __KERNELTIMER_H_

//---------------------------------------------------------------------------
#define SYSTEM_FREQ        ((K_ULONG)16000000)
#define TIMER_FREQ        ((K_ULONG)(SYSTEM_FREQ / 256)) // Timer ticks per second...

//---------------------------------------------------------------------------
/*!
    Hardware timer interface, used by all scheduling/timer subsystems.
*/
class KernelTimer
{
public:
    /*!
        \fn void Config(void)
        
        Initializes the kernel timer before use
    */
    static void Config(void);
    
    /*!
        \fn void Start(void)
        
        Starts the kernel time (must be configured first)
    */
    static void Start(void);
    
    /*!
        \fn void Stop(void)
        
        Shut down the kernel timer, used when no timers are scheduled
    */
    static void Stop(void);
    
    /*!
        \fn K_UCHAR DI(void)
        
        Disable the kernel timer's expiry interrupt
    */
    static K_UCHAR DI(void);
    
    /*!
        \fn void RI(bool bEnable_)
        
        Retstore the state of the kernel timer's expiry interrupt.
        
        \param bEnable_ 1 enable, 0 disable
    */
    static void RI(bool bEnable_);
    
    /*!
        \fn void EI(void)
        
        Enable the kernel timer's expiry interrupt
    */
    static void EI(void);

    /*!
        \fn K_ULONG SubtractExpiry(K_ULONG ulInterval_)
        
        Subtract the specified number of ticks from the timer's 
        expiry count register.  Returns the new expiry value stored in 
        the register.
        
        \param ulInterval_ Time (in HW-specific) ticks to subtract
        \return Value in ticks stored in the timer's expiry register
    */
    static K_ULONG SubtractExpiry(K_ULONG ulInterval_);
    
    /*!
        \fn K_ULONG TimeToExpiry(void)
        
        Returns the number of ticks remaining before the next timer 
        expiry.
        
        \param Time before next expiry in platform-specific ticks
    */
    static K_ULONG TimeToExpiry(void);
    
    /*!
        \fn K_ULONG SetExpiry(K_ULONG ulInterval_)
        
        Resets the kernel timer's expiry interval to the specified value
        
        \param ulInterval_ Desired interval in ticks to set the timer for
        \return Actual number of ticks set (may be less than desired)        
    */
    static K_ULONG SetExpiry(K_ULONG ulInterval_);
    
    /*!
        \fn K_ULONG GetOvertime(void)
        
        Return the number of ticks that have elapsed since the last
        expiry.
        
        \return Number of ticks that have elapsed after last timer expiration
    */
    static K_ULONG GetOvertime(void);
    
    /*!
        \fn void ClearExpiry(void)
        
        Clear the hardware timer expiry register
    */
    static void ClearExpiry(void);
    
private:
    /*!
        \fn K_USHORT Read(void)
        
        Safely read the current value in the timer register
        
        \return Value held in the timer register
    */
    static K_USHORT Read(void);
    
};

#endif //__KERNELTIMER_H_
