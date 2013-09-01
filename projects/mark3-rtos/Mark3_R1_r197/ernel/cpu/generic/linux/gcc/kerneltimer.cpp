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

    \brief  Kernel Timer Implementation for Linux
*/

#include "kerneltypes.h"
#include "kerneltimer.h"
#include "thread.h"
#include "threadport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>


volatile bool bEnabled;
volatile bool bStarted;

//---------------------------------------------------------------------------
static void sig_handler( int signal )
{    
    fprintf(stderr, "SigHandler\n");
    if (!(bEnabled && bStarted))
    {
        return;
    }

    if (usIntFlags & FLAG_TIMER)
    {
        return;
    }

    usIntFlags |= FLAG_TIMER;

    if (bIntEnabled)
    {
        fprintf(stderr, "Trigger Timer Interrupt\n");
        sem_post(&stKSem);
    }
    else
    {
        fprintf(stderr, "Trigger generic interrupt\n");
        bTriggerInt = true;
    }
}

//---------------------------------------------------------------------------
void KernelTimer::Config(void)
{
    struct itimerval new_val;
    struct sigaction sa;

    fprintf(stderr, "Installed sighandler\n");
    memset( &sa, 0, sizeof(sa) );
    sa.sa_handler = sig_handler;
    sigaction(SIGVTALRM, &sa, NULL);
}

//---------------------------------------------------------------------------
void KernelTimer::Start(void)
{
    bStarted = true;
    bEnabled = true;
    fprintf(stderr, "Timer Start\n");
}

//---------------------------------------------------------------------------
void KernelTimer::Stop(void)
{
    bStarted = false;
    bEnabled = false;
    fprintf(stderr, "Timer Stop\n");
}

//---------------------------------------------------------------------------
K_USHORT KernelTimer::Read(void)
{
    return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SubtractExpiry(K_ULONG ulInterval_)
{
    K_ULONG ulTempInt = TimeToExpiry();

    if (ulTempInt >= ulInterval_)
    {
        ulTempInt -= ulInterval_;
    }

    SetExpiry(ulTempInt);
    return ulTempInt;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::TimeToExpiry(void)
{
    struct itimerval curr_time;
    K_ULONG ulTime;
    getitimer(ITIMER_VIRTUAL, &curr_time);

    ulTime = curr_time.it_value.tv_sec * 1000000;
    ulTime += curr_time.it_value.tv_usec;
    return ulTime;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::GetOvertime(void)
{
    // assume infinitely-fast timer callback processing.  We're using LINUX
    // so our realtime processing is kind of approximate anyway...
    return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SetExpiry(K_ULONG ulInterval_)
{
    //!! Todo - support for limited timers (to 0x7FFFFFFF)
    struct itimerval new_timer;
    new_timer.it_value.tv_sec = ulInterval_ / 1000000;
    new_timer.it_value.tv_usec = ulInterval_ % 1000000;
    new_timer.it_interval.tv_sec = ulInterval_ / 1000000;
    new_timer.it_interval.tv_usec = ulInterval_ / 1000000;

    fprintf(stderr, "set expiry %d\n", ulInterval_);

    int err = setitimer(ITIMER_VIRTUAL, &new_timer, NULL );
    if (0 != err)
    {
        fprintf(stderr, "Timer error: %d\n", errno);
    }
    return ulInterval_;
}

//---------------------------------------------------------------------------
void KernelTimer::ClearExpiry(void)
{
    struct itimerval new_timer;
    memset(&new_timer, 0, sizeof(new_timer) );

    setitimer(ITIMER_VIRTUAL, &new_timer, NULL);
    fprintf(stderr, "Clear Expiry\n");
}

//---------------------------------------------------------------------------
K_UCHAR KernelTimer::DI(void)
{    
    KernelTimer::RI(0);
}

//---------------------------------------------------------------------------
void KernelTimer::EI(void)
{
    KernelTimer::RI(1);
}

//---------------------------------------------------------------------------
void KernelTimer::RI(bool bEnable_)
{    
    bEnabled = bEnable_;
}
