/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!

    \file   quantum.h    

    \brief  Thread Quantum declarations for Round-Robin Scheduling

*/

#ifndef __KQUANTUM_H__
#define __KQUANTUM_H__

#include "kerneltypes.h"
#include "mark3cfg.h"

#include "thread.h"
#include "timerlist.h"

#if KERNEL_USE_QUANTUM
class Timer;

/*!
	Static-class used to implement Thread quantum functionality, which is 
	a key part of round-robin scheduling.
*/

class Quantum
{
public:
	/*!
		\fn void UpdateTimer()
		
		This function is called to update the thread quantum timer whenever
		something in the scheduler has changed.  This can result in the timer
		being re-loaded or started.  The timer is never stopped, but if may
		be ignored on expiry.
	*/
    static void UpdateTimer();
	
	/*!
		\fn void AddThread( Thread *pclThread_ )
	
		Add the thread to the quantum timer.  Only one thread can own the quantum,
		since only one thread can be running on a core at a time.
	*/
    static void AddThread( Thread *pclThread_ );
    
	/*!
		\fn void RemoveThread()
		
		Remove the thread from the quantum timer.  This will cancel the timer.
	*/
	static void RemoveThread();
	
    /*!
     * \brief SetInTimer
     *
     * Set a flag to indicate that the CPU is currently running within the
     * timer-callback routine.  This prevents the Quantum timer from being
     * updated in the middle of a callback cycle, potentially resulting in
     * the kernel timer becoming disabled.
     */
	static void SetInTimer(void)   { m_bInTimer = true; }
		
    /*!
     * \brief ClearInTimer
     *
     * Clear the flag once the timer callback function has been completed.
     */
	static void ClearInTimer(void) { m_bInTimer = false; }
	
private:
	/*!
		\fn void SetTimer( Thread *pclThread_ )
	
        Set up the quantum timer in the timer scheduler.  This creates a 
		one-shot timer, which calls a static callback in quantum.cpp that 
		on expiry will pivot the head of the threadlist for the thread's
		priority.  This is the mechanism that provides round-robin 
		scheduling in the system.
		
		\param pclThread_ Pointer to the thread to set the Quantum timer on
	*/
	static void SetTimer( Thread *pclThread_ );
	
	static Timer m_clQuantumTimer;
	static K_UCHAR m_bActive;
	static K_UCHAR m_bInTimer;
};

#endif //KERNEL_USE_QUANTUM

#endif
