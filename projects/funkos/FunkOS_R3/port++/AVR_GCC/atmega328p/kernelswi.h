#ifndef __KERNELSWI_H_
#define __KERNELSWI_H_

#include "types.h"

class KernelSWI
{

public:
	static void Config(void);
	static void Start(void);
	static void Stop(void);
	static void Clear(void);
	static void Trigger(void);
};


#endif // __KERNELSIW_H_
