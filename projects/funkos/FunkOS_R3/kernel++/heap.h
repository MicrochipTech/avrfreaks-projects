//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//--------------------------------------------------------------------------- 
/*!
	\file:  heap.h
	
	Description:
		static block-size heap configuration
*/

#ifndef __HEAP_H_
#define __HEAP_H_

#if KERNEL_USE_HEAP

#include "kernelcfg.h"
#include "types.h"

//---------------------------------------------------------------------------
class FunkOS_Heap
{
public:
	static void Init(void);
	static UCHAR *Alloc(USHORT usSize_);
	static void Free(void *pvData_);
private:
};

#endif //KERNEL_USE_HEAP

#endif //__HEAP_H_
