//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  watchdog.c

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
static WATCHDOG_TASK *pstWDT[NUM_WDT_ENTRIES];

//---------------------------------------------------------------------------
/*!
	Initializes the watchdog timer object list to an initialized state.
	\fn void Watchdog_Init(void)
*/
void Watchdog_Init(void)
{
	USHORT i;
	
	for (i = 0; i < NUM_WDT_ENTRIES; i++)
	{
		pstWDT[i] = NULL;
	}
}
//---------------------------------------------------------------------------
/*!
	Adds a watchdog timer object to the watchdog task list.
	\fn BOOL Watchdog_AddTask(WATCHDOG_TASK *pstTask_, USHORT usTime_)
	\param pstTask_ - pointer to the watchdog task structure
	\param usTime_ - the task expiry timeout interval
	\return BOOL - TRUE on success, FALSE on list full
*/
BOOL Watchdog_AddTask(WATCHDOG_TASK *pstTask_, USHORT usTime_)
{
	USHORT i;
	// See if there's a free entry in the table
	for (i = 0; i < NUM_WDT_ENTRIES; i++)
	{
		// Found a blank space
		if (pstWDT[i] != NULL)
		{
			// Copy the data in, reset the state
			pstWDT[i] = pstTask_;
			pstWDT[i]->eState = WDT_TASK_IDLE;
			pstWDT[i]->usTimeout = usTime_;
			pstWDT[i]->usCurrentTime = 0;
		}
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------------------------------------------
/*!
	Removes the specified watchdog task from the watch list.
	\fn void Watchdog_RemoveTask(WATCHDOG_TASK *pstTask_)
	\param pstTask_ - Pointer to the watchdog task object
*/
void Watchdog_RemoveTask(WATCHDOG_TASK *pstTask_)
{
	USHORT i;
	for (i = 0; i < NUM_WDT_ENTRIES; i++)
	{
		// if the item matches the entry in the table
		if (pstWDT[i] == pstTask_)
		{
			// reset the task to idle
			pstWDT[i]->eState = WDT_TASK_IDLE;
			
			// remove the entry from the table - set to NULL
			pstWDT[i] = NULL;
		}
	}
}
//---------------------------------------------------------------------------
/*!
	Enables a watchdog object.  Once enabled, the owner task is obliged to 
	meet the deadline it specified when it was added to the list.
	\fn void Watchdog_StartTask(WATCHDOG_TASK *pstTask_)
	\param pstTask_ - 
*/
void Watchdog_StartTask(WATCHDOG_TASK *pstTask_)
{
	pstTask_->eState = WDT_TASK_ACTIVE;
	pstTask_->usCurrentTime = 0;
}
//---------------------------------------------------------------------------
/*!
	\fn void Watchdog_IdleTask(WATCHDOG_TASK *pstTask_)
	\param pstTask_ - sets the specified watchdog object into its idle state
*/
void Watchdog_IdleTask(WATCHDOG_TASK *pstTask_)
{
	pstTask_->eState = WDT_TASK_IDLE;
	pstTask_->usCurrentTime = 0;
}
//---------------------------------------------------------------------------
/*!
	Controls the system watchdog kick, by ensuring none exceed their deadlines.
	Called at a regular frequency.  Every active timer object in the list is 
	incremented, and if any task has exceeded its deadline time, the function 
	will *not* kick the system WDT - oherwise, the watchdog is kicked.  This is
	especially useful for debugging, where details about the offending task 
	can be logged by the KernelWDT module instead of causing the WDT to expire.
	\fn void Watchdog_Tick(void)
*/
void Watchdog_Tick(void)
{
	BOOL bKickDog = TRUE;
	USHORT i;
	
	// Look through every entry in the WDT struct
	for (i = 0; i < NUM_WDT_ENTRIES; i++)
	{
		// If the entry isn't null
		if (pstWDT[i] != NULL)
		{
			// If the task is active
			if (pstWDT[i]->eState == WDT_TASK_ACTIVE)
			{
				// Has the timeout expired?
				if (pstWDT[i]->usCurrentTime < pstWDT[i]->usTimeout)
				{
					// No - increment the current task time by the granularity of the kick function
					pstWDT[i]->usCurrentTime += TIME_PER_KICK_MS;
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
		KernelWDT_Kick();
	}
	else
	{
		// insert task timeout logging function here.
	}
}

#endif //KERNEL_USE_WDT

