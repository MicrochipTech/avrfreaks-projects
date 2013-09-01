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

    \file   profile.cpp

    \brief  Code profiling utilities
*/

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "profile.h"
#include "kprofile.h"
#include "threadport.h"
#include "kernel_debug.h"
//---------------------------------------------------------------------------
#if defined __FILE_ID__
	#undef __FILE_ID__
#endif
#define __FILE_ID__ 	PROFILE_CPP


#if KERNEL_USE_PROFILER

//---------------------------------------------------------------------------
void ProfileTimer::Init()
{
    m_ulCumulative = 0;
    m_ulCurrentIteration = 0;
    m_usIterations = 0;
    m_bActive = 0;
}

//---------------------------------------------------------------------------    
void ProfileTimer::Start()
{
    if (!m_bActive)
    {
        CS_ENTER();
        m_ulCurrentIteration = 0;
		m_ulInitialEpoch = Profiler::GetEpoch();		
        m_usInitial = Profiler::Read();        
        CS_EXIT();
        m_bActive = 1;
    }
}

//---------------------------------------------------------------------------    
void ProfileTimer::Stop()
{
    if (m_bActive)
    {
        K_USHORT usFinal;
        K_ULONG ulEpoch;
        CS_ENTER();
        usFinal = Profiler::Read();
        ulEpoch = Profiler::GetEpoch();
        // Compute total for current iteration...
        m_ulCurrentIteration = ComputeCurrentTicks(usFinal, ulEpoch);
        m_ulCumulative += m_ulCurrentIteration;
        m_usIterations++;        
        CS_EXIT();
        m_bActive = 0;
    }
}

//---------------------------------------------------------------------------    
K_ULONG ProfileTimer::GetAverage()
{
    if (m_usIterations)
    {
        return m_ulCumulative / (K_ULONG)m_usIterations;
    }
    return 0;
}
 
//---------------------------------------------------------------------------     
K_ULONG ProfileTimer::GetCurrent()
{
	
    if (m_bActive)
    {
		K_USHORT usCurrent;
		K_ULONG ulEpoch;
		CS_ENTER();
		usCurrent = Profiler::Read();
		ulEpoch = Profiler::GetEpoch();
		CS_EXIT();
        return ComputeCurrentTicks(usCurrent, ulEpoch);
    }
	return m_ulCurrentIteration;
}

//---------------------------------------------------------------------------
K_ULONG ProfileTimer::ComputeCurrentTicks(K_USHORT usCurrent_, K_ULONG ulEpoch_)
{    
    K_ULONG ulTotal;
	K_ULONG ulOverflows;
	
	ulOverflows = ulEpoch_ - m_ulInitialEpoch;
	
	// More than one overflow...
	if (ulOverflows > 1)
	{
        ulTotal = ((K_ULONG)(ulOverflows-1) * TICKS_PER_OVERFLOW)
				+ (K_ULONG)(TICKS_PER_OVERFLOW - m_usInitial) +
				(K_ULONG)usCurrent_;		
	}
	// Only one overflow, or one overflow that has yet to be processed
	else if (ulOverflows || (usCurrent_ < m_usInitial))
	{
		ulTotal = (K_ULONG)(TICKS_PER_OVERFLOW - m_usInitial) +
				(K_ULONG)usCurrent_;		
	}
	// No overflows, none pending.
	else
	{
		ulTotal = (K_ULONG)(usCurrent_ - m_usInitial);					
	}

    return ulTotal;
}

#endif
