//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
#ifndef __KERNELWDT_H_
#define __KERNELWDT_H_

#include "kernelcfg.h"

#if KERNEL_USE_WDT

void KernelWDT_Config(void);
void KernelWDT_Start(void);
void KernelWDT_Stop(void);
void KernelWDT_Kick(void);

#endif //KERNEL_USE_WDT

#endif //__KERNELWDT_H_
