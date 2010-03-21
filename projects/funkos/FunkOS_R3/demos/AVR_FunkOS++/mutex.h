//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
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


class FunkOS_Mutex
{
public:
	void Init(void);
	BOOL Claim(USHORT usTime_);
	void Release(void);
	
private:
	USHORT m_usSem;
	UCHAR m_ucMaxPri;
	FunkOS_Task *m_pclTask;
	
	void WakeNextPending(void);
	void DeleteFromList(FunkOS_Task *pclTask_);
	void AddToList(FunkOS_Task *pclTask_);
	void InheritPriority(UCHAR ucPriority_);	
};

#endif //__MUTEX_H_
