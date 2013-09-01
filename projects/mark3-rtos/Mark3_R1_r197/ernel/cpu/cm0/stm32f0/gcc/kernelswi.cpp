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

    \brief  Kernel Software interrupt implementation for ARM Cortex-M0

*/

#include "kerneltypes.h"
#include "kernelswi.h"
#include "threadport.h"

//---------------------------------------------------------------------------
void KernelSWI::Config(void)
{
	NVIC_SetPriority(SVCall_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	NVIC_SetPriority(PendSV_IRQn, (1<<__NVIC_PRIO_BITS) - 1);	
}

//---------------------------------------------------------------------------	
void KernelSWI::Start(void)
{        
	// Nothing to do...
}

//---------------------------------------------------------------------------
void KernelSWI::Stop(void)
{
	// Nothing to do...
}

//---------------------------------------------------------------------------
K_UCHAR KernelSWI::DI()
{
	// Not implemented
	return 0;
}

//---------------------------------------------------------------------------
void KernelSWI::RI(bool bEnable_)
{
	// Not implemented
}

//---------------------------------------------------------------------------
void KernelSWI::Clear(void)
{	
	// There's no convenient CMSIS function call for PendSV set/clear,
	// But we do at least have some structs/macros.
	
	// Note that set/clear each have their own bits in the same register.
	// Setting the "set" or "clear" bit results in the desired operation.
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
}

//---------------------------------------------------------------------------
void KernelSWI::Trigger(void)
{	
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
