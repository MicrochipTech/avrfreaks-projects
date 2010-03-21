#ifndef __KERNELWDT_H_
#define __KERNELWDT_H_

class KernelWDT
{
public:
	static void Config(void);
	static void Start(void);
	static void Stop(void);
	static void Kick(void);
};

#endif //__KERNELWDT_H_
