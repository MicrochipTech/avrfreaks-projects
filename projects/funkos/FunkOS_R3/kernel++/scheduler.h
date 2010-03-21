//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  scheduler.h

	Description:
        Definition of the scheduler class, along with the task class,
    this forms the basis of the RTOS kernel.
*/
//---------------------------------------------------------------------------

#ifndef __SCHEDULER_HPP_
#define __SCHEDULER_HPP_

#include "types.h"
#include "task.h"

extern FunkOS_Task *m_pclCurrentTask;

class FunkOS_Scheduler
{
public:
	static void Init(void);
	static void Switch(void);
	static BOOL Tick(void);
	static BOOL IsEnabled(void);
	static void Set(BOOL bEnable_);
	static BOOL Add(FunkOS_Task *pclTask_);
	static BOOL Remove(FunkOS_Task *pclTask_);
	static BOOL Disable(void);
	static void Restore(BOOL bState_);
	static FunkOS_Task* GetCurrentTask(void);
	static void StartTasks(void);
 };

#endif
