//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
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

class FunkOS_Semaphore
{
public:
	FunkOS_Semaphore();
	FunkOS_Semaphore(USHORT usMaxVal_);

	BOOL Pend(USHORT usTime_);
	void Post(void);
	
private:

	void WakeNextPending(void);
	void DeleteFromList(FunkOS_Task *pclTask_);
	void AddToList(FunkOS_Task *pclTask_);

	USHORT m_usSem;			//!< Semaphore value (0 or 1 for binary semaphores)
	USHORT m_usMax;			//!< Maximum semaphore value
	FunkOS_Task *m_pclTask;	//!< Pointer to the first task in the semaphore wait list	
};

#endif //__SEMAPHORE_H_
