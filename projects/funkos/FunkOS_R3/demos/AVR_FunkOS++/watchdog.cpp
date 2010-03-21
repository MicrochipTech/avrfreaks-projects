//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  watchdog.cpp

	Description:
		Portable multitasking watchdog timer implementation.  Allows for 
		the system watchdog kick to only trigger when every task meets its 
		deadlines.  Can be configured to 
*/
//---------------------------------------------------------------------------
#include "types.h"
#include "watchdog.h"
#include "kernelwdt.h"
#include "kernelcfg.h"

#if KERNEL_USE_WDT

//---------------------------------------------------------------------------
static FunkOS_Watchdog *pclWDT[NUM_WDT_ENTRIES];

//---------------------------------------------------------------------------
/*!
	Initializes the watchdog timer object list to an initialized state.
	\fn void FunkOS_Watchdog::Init(void)
*/
void FunkOS_Watchdog::Init(void)
{
	USHORT i;
	
	for (i = 0; i < NUM_WDT_ENTRIES; i++)
	{
		pclWDT[i] = (FunkOS_Watchdog*)NULL;
	}
}
//---------------------------------------------------------------------------
/*!
	Adds a watchdog timer object to the watchdog task list.
	\fn BOOL FunkOS_Watchdog::AddTask(USHORT usTime_)
	\param usTime_ - the task expiry timeout interval
	\return BOOL - TRUE on success, FALSE on list full
*/
BOOL FunkOS_Watchdog::AddTask(USHORT usTime_)
{
	USHORT i;
	// See if there's a free entry in the table
	for (i = 0; i < NUM_WDT_ENTRIES; i++)
	{
		// Found a blank space
		if (pclWDT[i] != NULL)
		{
			// Copy the data in, reset the state
			pclWDT[i] = this;
			m_eState = WDT_TASK_IDLE;
			m_usTimeout = usTime_;
			m_usCurrentTime = 0;
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
/*!
	Removes the watchdog task from the global watch list.
	\fn void FunkOS_Watchdog::RemoveTask(void)
*/
void FunkOS_Watchdog::RemoveTask(void)
{
	USHORT i;
	for (i = 0; i < NUM_WDT_ENTRIES; i++)
	{
		// if the item matches the entry in the table
		if (pclWDT[i] == this)
		{
			// reset the task to idle
			m_eState = WDT_TASK_IDLE;
			
			// remove the entry from the table - set to NULL
			pclWDT[i] = (FunkOS_Watchdog*)NULL;
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Enables a watchdog object.  Once enabled, the owner task is obliged to 
	meet the deadline it specified when it was added to the list.
	\fn void FunkOS_Watchdog::Start(void)
*/
void FunkOS_Watchdog::Start(void)
{
	m_eState = WDT_TASK_ACTIVE;
	m_usCurrentTime = 0;
}
//---------------------------------------------------------------------------
/*!
	\fn void FunkOS_Watchdog::Idle(void)
*/
void FunkOS_Watchdog::Idle(void)
{
	m_eState = WDT_TASK_IDLE;
	m_usCurrentTime = 0;
}

//---------------------------------------------------------------------------
/*!
	\fn void FunkOS_Watchdog::AddTime(USHORT usTime_)
*/
void FunkOS_Watchdog::AddTime(USHORT usTime_)
{
	m_usCurrentTime	+= usTime_;
}

//---------------------------------------------------------------------------
/*!
	\fn USHORT FunkOS_Watchdog::GetTime(void)
*/
USHORT FunkOS_Watchdog::GetTime(void)
{
	return m_usCurrentTime;
}

//---------------------------------------------------------------------------
/*!
	\fn USHORT FunkOS_Watchdog::GetTimeout(void)
*/
USHORT FunkOS_Watchdog::GetTimeout(void)
{
	return m_usTimeout;
}

//---------------------------------------------------------------------------
/*!
	\fn WATCHDOG_STATE FunkOS_Watchdog::GetState(void)
*/
WATCHDOG_STATE FunkOS_Watchdog::GetState(void)
{
	return m_eState;
}

//---------------------------------------------------------------------------
/*!
	Controls the system watchdog kick, by ensuring none exceed their deadlines.
	Called at a regular frequency.  Every active timer object in the list is 
	incremented, and if any task has exceeded its deadline time, the function 
	will *not* kick the system WDT - oherwise, the watchdog is kicked.  This is
	especially useful for debugging, where details about the offending task 
	can be logged by the KernelWDT module instead of causing the WDT to expire.
	\fn void FunkOS_Watchdog::Tick(void)
*/
void FunkOS_Watchdog::Tick(void)
{
	BOOL bKickDog = TRUE;
	USHORT i;
	
	// Look through every entry in the WDT struct
	for (i = 0; i < NUM_WDT_ENTRIES; i++)
	{
		// If the entry isn't null
		if (pclWDT[i] != NULL)
		{
			// If the task is active
			if (pclWDT[i]->GetState() == WDT_TASK_ACTIVE)
			{
				// Has the timeout expired?
				if (pclWDT[i]->GetTime() < pclWDT[i]->GetTimeout())
				{
					// No - increment the current task time by the granularity of the kick function
					pclWDT[i]->AddTime(TIME_PER_KICK_MS);
				}
				else
				{
					// Yes - refuse to kick the watchdog
					bKickDog = FALSE;
				}
			}
		}
	}
	
	if (bKickDog == TRUE) 
	{
		// Kick the watchdog
		//KernelWDT::Kick();
	}
	else
	{
		// insert task timeout logging function here.
	}
}


#endif //KERNEL_USE_WDT

