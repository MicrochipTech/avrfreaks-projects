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

    \brief  Kernel Timer Implementation for ARM Cortex-M0
*/

#include "kerneltypes.h"
#include "kerneltimer.h"
#include "threadport.h"

//---------------------------------------------------------------------------
void KernelTimer::Config(void)
{
	   
}

//---------------------------------------------------------------------------
void KernelTimer::Start(void)
{	
	SysTick_Config(SYSTEM_FREQ / 1000); // 1KHz fixed clock...
	NVIC_EnableIRQ(SysTick_IRQn);
}

//---------------------------------------------------------------------------
void KernelTimer::Stop(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

//---------------------------------------------------------------------------
K_USHORT KernelTimer::Read(void)
{
	// Not implemented in this port
	return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SubtractExpiry(K_ULONG ulInterval_)
{
	return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::TimeToExpiry(void)
{
	return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::GetOvertime(void)
{
    return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SetExpiry(K_ULONG ulInterval_)
{	
	return 0;
}

//---------------------------------------------------------------------------
void KernelTimer::ClearExpiry(void)
{
}

//-------------------------------------------------------------------------
K_UCHAR KernelTimer::DI(void)
{
	return 0;
}

//---------------------------------------------------------------------------
void KernelTimer::EI(void)
{	
    KernelTimer::RI(0);
}

//---------------------------------------------------------------------------
void KernelTimer::RI(bool bEnable_)
{
}

//---------------------------------------------------------------------------
