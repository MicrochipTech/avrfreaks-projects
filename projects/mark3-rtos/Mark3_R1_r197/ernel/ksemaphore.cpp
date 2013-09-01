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

    \file   ksemaphore.cpp

    \brief  Semaphore Blocking-Object Implemenation 

*/

#include "kerneltypes.h"
#include "mark3cfg.h"

#include "ksemaphore.h"
#include "blocking.h"   
#include "kernel_debug.h"
//---------------------------------------------------------------------------
#if defined __FILE_ID__
	#undef __FILE_ID__
#endif
#define __FILE_ID__ 	SEMAPHORE_CPP

#if KERNEL_USE_SEMAPHORE

#if KERNEL_USE_TIMEOUTS
#include "timerlist.h"

//---------------------------------------------------------------------------
void TimedSemaphore_Callback(Thread *pclOwner_, void *pvData_)
{
	Semaphore *pclSemaphore = static_cast<Semaphore*>(pvData_);
	
	// Indicate that the semaphore has expired on the thread
	pclOwner_->SetExpired(true);
	
	// Wake up the thread that was blocked on this semaphore.
	pclSemaphore->WakeMe(pclOwner_);
	
	if (pclOwner_->GetPriority() > Scheduler::GetCurrentThread()->GetPriority())
	{
		Thread::Yield();
	}	
}

//---------------------------------------------------------------------------
void Semaphore::WakeMe(Thread *pclChosenOne_)
{ 
    // Remove from the semaphore waitlist and back to its ready list.
    UnBlock(pclChosenOne_);
}

#endif // KERNEL_USE_TIMEOUTS

//---------------------------------------------------------------------------
K_UCHAR Semaphore::WakeNext()
{
    Thread *pclChosenOne;
    
    pclChosenOne = m_clBlockList.HighestWaiter();
    
    // Remove from the semaphore waitlist and back to its ready list.
    UnBlock(pclChosenOne);

    // Call a task switch only if higher priority thread
    if (pclChosenOne->GetPriority() > Scheduler::GetCurrentThread()->GetPriority())
    {
        return 1;
    }
    return 0;
}

//---------------------------------------------------------------------------
void Semaphore::Init(K_USHORT usInitVal_, K_USHORT usMaxVal_)
{
    // Copy the paramters into the object - set the maximum value for this
    // semaphore to implement either binary or counting semaphores, and set
    // the initial count.  Clear the wait list for this object.
    m_usValue = usInitVal_;
    m_usMaxValue = usMaxVal_;    

    m_clBlockList.Init();
}

//---------------------------------------------------------------------------
bool Semaphore::Post()
{
	KERNEL_TRACE_1( STR_SEMAPHORE_POST_1, (K_USHORT)g_pstCurrent->GetID() );
	
    bool bThreadWake = 0;
    K_BOOL bBail = false;
    // Increment the semaphore count - we can mess with threads so ensure this
    // is in a critical section.  We don't just disable the scheudler since
    // we want to be able to do this from within an interrupt context as well.
    CS_ENTER();

    // If nothing is waiting for the semaphore
    if (m_clBlockList.GetHead() == NULL)
    {
        // Check so see if we've reached the maximum value in the semaphore
        if (m_usValue < m_usMaxValue)
        {
            // Increment the count value
            m_usValue++;
        }
        else
        {
            // Maximum value has been reached, bail out.
            bBail = true;
        }
    }
    else
    {
        // Otherwise, there are threads waiting for the semaphore to be
        // posted, so wake the next one (highest priority goes first).
        bThreadWake = WakeNext();
    }

    CS_EXIT();

    // If we weren't able to increment the semaphore count, fail out.
    if (bBail)
    {
        return false;
    }

    // if bThreadWake was set, it means that a higher-priority thread was
    // woken.  Trigger a context switch to ensure that this thread gets
    // to execute next.
    if (bThreadWake)
    {
        Thread::Yield();
    }
    return true;
}

//---------------------------------------------------------------------------
#if KERNEL_USE_TIMEOUTS
bool Semaphore::Pend_i( K_ULONG ulWaitTimeMS_ )
#else
void Semaphore::Pend_i( void )
#endif
{
    KERNEL_TRACE_1( STR_SEMAPHORE_PEND_1, (K_USHORT)g_pstCurrent->GetID() );

#if KERNEL_USE_TIMEOUTS
    Timer clSemTimer;
    bool bUseTimer = false;
#endif

    // Once again, messing with thread data - ensure
    // we're doing all of these operations from within a thread-safe context.
    CS_ENTER();

    // Check to see if we need to take any action based on the semaphore count
    if (m_usValue != 0)
    {
        // The semaphore count is non-zero, we can just decrement the count
        // and go along our merry way.
        m_usValue--;
    }
    else
    {
        // The semaphore count is zero - we need to block the current thread
        // and wait until the semaphore is posted from elsewhere.
#if KERNEL_USE_TIMEOUTS        
        if (ulWaitTimeMS_)
        {
            g_pstCurrent->SetExpired(false);
            clSemTimer.Init();
            clSemTimer.Start(0, ulWaitTimeMS_, TimedSemaphore_Callback, (void*)this);
            bUseTimer = true;
        }
#endif
        Block(g_pstCurrent);

        // Switch Threads immediately
        Thread::Yield();
    }

    CS_EXIT();

#if KERNEL_USE_TIMEOUTS
    if (bUseTimer)
    {
        clSemTimer.Stop();
        return (g_pstCurrent->GetExpired() == 0);
    }
    return true;
#endif
}

//---------------------------------------------------------------------------
// Redirect the untimed pend API to the timed pend, with a null timeout.
void Semaphore::Pend()
{
#if KERNEL_USE_TIMEOUTS
    Pend_i(0);
#else
    Pend_i();
#endif
}

#if KERNEL_USE_TIMEOUTS
//---------------------------------------------------------------------------	
bool Semaphore::Pend( K_ULONG ulWaitTimeMS_ )
{
    return Pend_i( ulWaitTimeMS_ );
}
#endif

//---------------------------------------------------------------------------
K_USHORT Semaphore::GetCount()
{
	K_USHORT usRet;
	CS_ENTER();
	usRet = m_usValue;
	CS_EXIT();
	return usRet;
}

#endif
