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
    \file kprofile.cpp
    
    \brief ATMega328p Profiling timer implementation
*/

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "profile.h"
#include "kprofile.h"
#include "threadport.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#if KERNEL_USE_PROFILER
K_ULONG Profiler::m_ulEpoch;

//---------------------------------------------------------------------------
void Profiler::Init()
{
    
}

//---------------------------------------------------------------------------
void Profiler::Start()
{
    
}    

//---------------------------------------------------------------------------
void Profiler::Stop()
{
    
}    
//---------------------------------------------------------------------------
K_USHORT Profiler::Read()
{
    
}

//---------------------------------------------------------------------------
void Profiler::Process()
{
    
}

//---------------------------------------------------------------------------
//ISR(TIMER0_OVF_vect)
//{
//    Profiler::Process();
//}

#endif
