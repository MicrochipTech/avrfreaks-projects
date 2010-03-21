//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
#include "types.h"
#include "kernelcfg.h"
#include "kernelwdt.h"
#include "taskport.h"
#include <msp430.h>

#if KERNEL_USE_WDT
#define WDT_PERIOD		(0x05)		// ~0.5 seconds

//---------------------------------------------------------------------------
void KernelWDT_Config(void)
{
}

//---------------------------------------------------------------------------
void KernelWDT_Start(void)
{
}

//---------------------------------------------------------------------------
void KernelWDT_Stop(void)
{
}

//---------------------------------------------------------------------------
void KernelWDT_Kick(void)
{
}

#endif //KERNERL_USE_WDT
