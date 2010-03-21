//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  mutex.h

	Description:
		Declaration of the mutual exclusion semaphore API for FunkOS
*/
#ifndef __MUTEX_H_
#define __MUTEX_H_

#include "types.h"
#include "task.h"
#include "taskport.h"

//---------------------------------------------------------------------------
typedef struct
{
	USHORT usSem;
	UCHAR ucMaxPri;
	TASK_STRUCT *pstTask;
} MUTEX_STRUCT;

//---------------------------------------------------------------------------
void Mutex_Init(MUTEX_STRUCT *pstMutex_);
//---------------------------------------------------------------------------
BOOL Mutex_Claim(MUTEX_STRUCT *pstMutex_, USHORT usTime_);
//---------------------------------------------------------------------------
void Mutex_Release(MUTEX_STRUCT *pstMutex_);

#endif //__MUTEX_H_
