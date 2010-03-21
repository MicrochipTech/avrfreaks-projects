//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  semaphore.h

	Description:
		Declaration of binary semaphore API for FunkOS
*/
#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#include "types.h"
#include "task.h"

//---------------------------------------------------------------------------
typedef struct
{
	USHORT usSem;			//!< Semaphore value (0 or 1 for binary semaphores)
	USHORT usMax;			//!< Maximum semaphore value
	TASK_STRUCT *pstTask;	//!< Pointer to the first task in the semaphore wait list
} SEMAPHORE_STRUCT;			//!< Semaphore object definition

//---------------------------------------------------------------------------
void Semaphore_Init(SEMAPHORE_STRUCT *pstSem_);
//---------------------------------------------------------------------------
BOOL Semaphore_Pend(SEMAPHORE_STRUCT *pstSem_, USHORT usTime_);
//---------------------------------------------------------------------------
void Semaphore_Post(SEMAPHORE_STRUCT *pstSem_);

#endif //__SEMAPHORE_H_
