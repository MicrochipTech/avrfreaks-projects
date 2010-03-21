#ifndef __KERNELSWI_H_
#define __KERNELSWI_H_

#include "types.h"

void KernelSWI_Config(void);
void KernelSWI_Start(void);
void KernelSWI_Stop(void);
void KernelSWI_Clear(void);
void KernelSWI_Trigger(void);

#endif // __KERNELSIW_H_
