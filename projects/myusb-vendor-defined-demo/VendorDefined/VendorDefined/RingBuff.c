/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

#include "RingBuff.h"

void Buffer_Initialize(RingBuff_t* Buffer)
{
	BUFF_ATOMIC_BLOCK
	{
		Buffer->InPtr    = (RingBuff_Data_t*)&Buffer->Buffer;
		Buffer->OutPtr   = (RingBuff_Data_t*)&Buffer->Buffer;
		Buffer->Elements = 0;
	}
}

void Buffer_StoreElement(RingBuff_t* Buffer, RingBuff_Data_t Data)
{
	BUFF_ATOMIC_BLOCK
	{
	#if defined(BUFF_DROPOLD)
		if (Buffer->Elements == BUFF_LENGTH)
		{
			Buffer->OutPtr++;
	
			if (Buffer->OutPtr == &Buffer->Buffer[BUFF_LENGTH])
			  Buffer->OutPtr = (RingBuff_Data_t*)&Buffer->Buffer;
		}
		else
		{
			Buffer->Elements++;
		}
	#elif defined(BUFF_DROPNEW)
		if (Buffer->Elements == BUFF_LENGTH)
		  return;
	
		  Buffer->Elements++;
	#elif defined(BUFF_NODROPCHECK)
		  Buffer->Elements++;
	#endif
	
		*(Buffer->InPtr) = Data;
		Buffer->InPtr++;
	
		if (Buffer->InPtr == &Buffer->Buffer[BUFF_LENGTH])
		  Buffer->InPtr = (RingBuff_Data_t*)&Buffer->Buffer;
	}
}

RingBuff_Data_t Buffer_GetElement(RingBuff_t* Buffer)
{
	RingBuff_Data_t BuffData;
	
	BUFF_ATOMIC_BLOCK
	{
#if defined(BUFF_EMPTYRETURNSZERO)
		if (!(Buffer->Elements))
		  return 0;
#elif !defined(BUFF_NOEMPTYCHECK)
	#error No empty buffer check behaviour specified.
#endif

		BuffData = *(Buffer->OutPtr);
	
		Buffer->OutPtr++;
		Buffer->Elements--;
	
		if (Buffer->OutPtr == &Buffer->Buffer[BUFF_LENGTH])
		  Buffer->OutPtr = (RingBuff_Data_t*)&Buffer->Buffer;
	}
	
	return BuffData;
}

#if defined(BUFF_USEPEEK)
RingBuff_Data_t Buffer_PeekElement(const RingBuff_t* Buffer)
{
	RingBuff_Data_t BuffData;

	BUFF_ATOMIC_BLOCK
	{
#if defined(BUFF_EMPTYRETURNSZERO)
		if (!(Buffer->Elements))
		  return 0;
#elif !defined(BUFF_NOEMPTYCHECK)
	#error No empty buffer check behaviour specified.
#endif

		BuffData = *(Buffer->OutPtr);
	}

	return BuffData;
}
#endif
