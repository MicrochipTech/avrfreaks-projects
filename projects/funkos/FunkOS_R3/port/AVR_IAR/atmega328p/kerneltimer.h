#ifndef __KERNELTIMER_H_
#define __KERNELTIMER_H_

#include "types.h"

//---------------------------------------------------------------------------
void KernelTimer_Config(void);
void KernelTimer_Start(void);
void KernelTimer_Stop(void);
void KernelTimer_DI(void);
void KernelTimer_EI(void);
#endif //__KERNELTIMER_H_
