#ifndef __KERNELTIMER_H_
#define __KERNELTIMER_H_

#include "types.h"

class KernelTimer
{
public:
	static void Config(void);
	static void Start(void);
	static void Stop(void);
	static void DI(void);
	static void EI(void);

};

//---------------------------------------------------------------------------
#endif //__KERNELTIMER_H_
