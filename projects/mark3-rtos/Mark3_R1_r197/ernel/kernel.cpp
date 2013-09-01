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

    \file   kernel.cpp

    \brief  Kernel initialization and startup code
*/

#include "kerneltypes.h"
#include "mark3cfg.h"

#include "kernel.h"
#include "scheduler.h"
#include "thread.h"
#include "threadport.h"
#include "timerlist.h"
#include "message.h"
#include "driver.h"
#include "profile.h"
#include "kprofile.h"
#include "tracebuffer.h"
#include "kernel_debug.h"

bool Kernel::m_bIsStarted;
bool Kernel::m_bIsPanic;
panic_func_t Kernel::m_pfPanic;

//---------------------------------------------------------------------------
#if defined __FILE_ID__
	#undef __FILE_ID__
#endif
#define __FILE_ID__ 	KERNEL_CPP

//---------------------------------------------------------------------------
void Kernel::Init(void)
{
    m_bIsStarted = false;
    m_bIsPanic = false;
    m_pfPanic = 0;

#if KERNEL_USE_DEBUG
	TraceBuffer::Init();
#endif
	KERNEL_TRACE( STR_MARK3_INIT );

    // Initialize the global kernel data - scheduler, timer-scheduler, and
    // the global message pool.	
    Scheduler::Init();
#if KERNEL_USE_DRIVER
	DriverList::Init();
#endif	
#if KERNEL_USE_TIMERS    
    TimerScheduler::Init();
#endif
#if KERNEL_USE_MESSAGE    
    GlobalMessagePool::Init();
#endif
#if KERNEL_USE_PROFILER
	Profiler::Init();
#endif
}
    
//---------------------------------------------------------------------------
void Kernel::Start(void)
{
	KERNEL_TRACE( STR_THREAD_START );    
    m_bIsStarted = true;
    ThreadPort::StartThreads();
	KERNEL_TRACE( STR_START_ERROR );
}

//---------------------------------------------------------------------------
void Kernel::Panic(K_USHORT usCause_)
{
    m_bIsPanic = true;
    if (m_pfPanic)
    {
        m_pfPanic(usCause_);
    }
    else
    {
#if KERNEL_AWARE_SIMULATION
        Kernel_Aware::Exit_Simulator();
#endif
        while(1);
    }
}
