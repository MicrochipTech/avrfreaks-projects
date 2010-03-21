//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  timer.c

	Description:
		fixed-frequency event callback implementation for FunkOS.
*/
#include "types.h"
#include "timer.h"
#include "taskport.h"
#include "kernelcfg.h"

#if KERNEL_USE_TIMERS

//---------------------------------------------------------------------------
static TIMER_STRUCT astTimers[NUM_TIMERS];

//---------------------------------------------------------------------------
/*!
	Initialize the timer event table to default values.
	\fn void Timer_Init(void)
*/

void Timer_Init(void)
{
	int i;
	// Clear the list of timers
	for (i = 0; i < NUM_TIMERS; i++)
	{
		astTimers[i].usTicks = 0;
		astTimers[i].usOffset = 0;
		astTimers[i].eState = TIMER_STATE_UNINIT;
		astTimers[i].pfCallback = NULL;
	}
}

//---------------------------------------------------------------------------
/*!
	Add a new timer event to execute continually at a specified interval.
	\fn TIMER_STRUCT* Timer_Add(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_)
	\param usTicks_ - the time (in system ticks) between triggering the callback
	\param usOffset_ - the phase offset of the event (in system ticks)
	\param pfCallback_ - function pointer to the lighweight thread
	\return TIMER_STRUCT* - pointer to the entry in the table
*/
TIMER_STRUCT* Timer_Add(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_)
{
	int i;
	TIMER_STRUCT *pstReturn = NULL;
	
	i = 0;
	
	// Go through the list of timers
	CS_ENTER();
	while (i < NUM_TIMERS)
	{
		// See if there's a free one
		if (astTimers[i].eState == TIMER_STATE_UNINIT)
		{
			// Initialize the timer
			astTimers[i].eState = TIMER_STATE_CLOSED;
			astTimers[i].usTicks = usTicks_;
			astTimers[i].usTickCount = 0;
			astTimers[i].usOffset = usOffset_;
			astTimers[i].pfCallback = pfCallback_;
			pstReturn = &astTimers[i];
			break;
		}
		i++;
	}
	CS_EXIT();
	
	return pstReturn;
}

//---------------------------------------------------------------------------
/*!
	Start the timer event specified
	\fn void Timer_Start(TIMER_STRUCT *pstTimer_)
	\param pstTimer_ - pointer to the timer entry to start
*/
void Timer_Start(TIMER_STRUCT *pstTimer_)
{
	pstTimer_->usTickCount = 0;
	pstTimer_->eState = TIMER_STATE_OPEN;
}
//---------------------------------------------------------------------------
/*!
	\fn void Timer_Stop(TIMER_STRUCT *pstTimer_)
	\param pstTimer_ - timer object to disable
*/
void Timer_Stop(TIMER_STRUCT *pstTimer_)
{
	pstTimer_->eState = TIMER_STATE_CLOSED;
}
//---------------------------------------------------------------------------
/*!
	Increment the timer associated with each event in the table.  If any timer
	has reached its specified epoch, it executes the associated lightweight 
	thread, and resets the time interval.  Execution is staggered based on
	the specified phase offset for each task.
	\fn void Timer_Tick(void)
*/
void Timer_Tick(void)
{
	int i;

	// Run through the timers and see if something needs to be run
	for (i = 0; i < NUM_TIMERS; i++)
	{
		// If the timer's open...
		if (astTimers[i].eState == TIMER_STATE_OPEN)
		{
			// Increment the tick
			astTimers[i].usTickCount++;

			// If the tick count == ticks + offset time
			if (astTimers[i].usTickCount == (astTimers[i].usTicks + astTimers[i].usOffset))
			{	
				// Reset the timer's offset time
				astTimers[i].usTickCount = astTimers[i].usOffset;

				// Run the timer callback
				if (astTimers[i].pfCallback != NULL)
				{
					astTimers[i].pfCallback();
				}
			}
		}
	}
}

#endif //KERNEL_USE_TIMERS
