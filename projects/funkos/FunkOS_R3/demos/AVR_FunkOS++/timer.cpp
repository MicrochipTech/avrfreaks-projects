//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  timer.cpp

	Description:
		fixed-frequency event callback implementation for FunkOS.
*/
#include "types.h"
#include "timer.h"
#include "taskport.h"
#include "kernelcfg.h"

#if KERNEL_USE_TIMERS

static FunkOS_Timer	m_aclTimers[NUM_TIMERS];

//---------------------------------------------------------------------------
/*!
	Initialize the timer event table to default values.
	\fn FunkOS_Timer::FunkOS_Timer()
*/

FunkOS_Timer::FunkOS_Timer()
{
	m_usTicks = 0;
	m_usOffset = 0;
	m_eState = TIMER_STATE_UNINIT;
	m_pfCallback = (TIMER_CALLBACK)NULL;
}

//---------------------------------------------------------------------------
/*!
	Sets the timer event to execute at a specified interval.  Must be added to
    the timer list before it will be executed.
	\fn void FunkOS_Timer::Create(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_)
	\param usTicks_ - the time (in system ticks) between triggering the callback
	\param usOffset_ - the phase offset of the event (in system ticks)
	\param pfCallback_ - function pointer to the lighweight thread
*/
void FunkOS_Timer::Create(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_)
{
	// Go through the list of timers
	CS_ENTER();
	// See if there's a free one
	if (m_eState == TIMER_STATE_UNINIT)
	{
		// Initialize the timer
		m_eState = TIMER_STATE_CLOSED;
		m_usTicks = usTicks_;
		m_usTickCount = 0;
		m_usOffset = usOffset_;
		m_pfCallback = pfCallback_;
	}	
	CS_EXIT();	
}

//---------------------------------------------------------------------------
/*!
	Start the timer event specified
	\fn void FunkOS_Timer::Start(void)
*/
void FunkOS_Timer::Start(void)
{
	m_usTickCount = 0;
	m_eState = TIMER_STATE_OPEN;
}
//---------------------------------------------------------------------------
/*!
	\fn void FunkOS_Timer::Stop(void)
*/
void FunkOS_Timer::Stop(void)
{
	m_eState = TIMER_STATE_CLOSED;
}

//---------------------------------------------------------------------------
/*!
	\fn void FunkOS_Timer::Tick(void)
*/
void FunkOS_Timer::Tick(void)
{
	m_usTickCount++;
	
	// If the tick count == ticks + offset time
	if (m_usTickCount == (m_usTicks + m_usOffset))
	{	
		// Reset the timer's offset time
		m_usTickCount = m_usOffset;

		// Run the timer callback
		if (m_pfCallback != NULL)
		{
			m_pfCallback();
		}
	}
}

//---------------------------------------------------------------------------
/*!
	\fn TIMER_STATE FunkOS_Timer::GetState(void)
*/
TIMER_STATE FunkOS_Timer::GetState(void)
{
	return m_eState;
}
//---------------------------------------------------------------------------
/*!
	Increment the timer associated with each event in the table.  If any timer
	has reached its specified epoch, it executes the associated lightweight 
	thread, and resets the time interval.  Execution is staggered based on
	the specified phase offset for each task.
	\fn void FunkOS_TimerList::Tick(void)
*/
void FunkOS_TimerList::Tick(void)
{
	int i;

	// Run through the timers and see if something needs to be run
	for (i = 0; i < NUM_TIMERS; i++)
	{
		// If the timer's open...
		if (m_aclTimers[i].GetState() == TIMER_STATE_OPEN)
		{
			// Tick each 
			m_aclTimers[i].Tick();
		}
	}
}
//---------------------------------------------------------------------------
/*!
    Adds a timer object to the managed list of timers, which will execute at
    a fixed interval.  If the timer list is full, the function returns NULL, 
    indicating failure, otherwise a pointer to the timer object is returned
    to the caller.
	\fn FunkOS_Timer* FunkOS_TimerList::Add(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_)
	\param usTicks_ - the time (in system ticks) between triggering the callback
	\param usOffset_ - the phase offset of the event (in system ticks)
	\param pfCallback_ - function pointer to the lighweight thread
*/
FunkOS_Timer* FunkOS_TimerList::Add(USHORT usTicks_, USHORT usOffset_, TIMER_CALLBACK pfCallback_)
{
	UCHAR i;
	for (i = 0; i < NUM_TIMERS; i++)
	{
		if (m_aclTimers[i].GetState() == TIMER_STATE_UNINIT)	
		{
			m_aclTimers[i].Create(usTicks_, usOffset_, pfCallback_);
			return (&m_aclTimers[i]);
		}
	}
	return (FunkOS_Timer*)NULL;
}

#endif //KERNEL_USE_TIMERS
