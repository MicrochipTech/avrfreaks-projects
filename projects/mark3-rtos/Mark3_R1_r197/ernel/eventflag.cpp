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
    \file eventflag.cpp
    \brief Event Flag Blocking Object/IPC-Object implementation.
*/

#include "mark3cfg.h"
#include "blocking.h"
#include "kernel.h"
#include "thread.h"
#include "eventflag.h"

#if KERNEL_USE_EVENTFLAG

#if KERNEL_USE_TIMEOUTS
#include "timerlist.h"
//---------------------------------------------------------------------------
void TimedEventFlag_Callback(Thread *pclOwner_, void *pvData_)
{
    EventFlag *pclEventFlag = static_cast<EventFlag*>(pvData_);

    pclEventFlag->WakeMe(pclOwner_);
    pclOwner_->SetExpired(true);
    pclOwner_->SetEventFlagMask(0);

    if (pclOwner_->GetPriority() > Scheduler::GetCurrentThread()->GetPriority())
    {
        Thread::Yield();
    }
}

//---------------------------------------------------------------------------
void EventFlag::WakeMe(Thread *pclChosenOne_)
{
    UnBlock(pclChosenOne_);
}
#endif

//---------------------------------------------------------------------------
#if KERNEL_USE_TIMEOUTS
    K_USHORT EventFlag::Wait_i(K_USHORT usMask_, EventFlagOperation_t eMode_, K_ULONG ulTimeMS_)
#else
    K_USHORT EventFlag::Wait_i(K_USHORT usMask_, EventFlagOperation_t eMode_)
#endif
{
    bool bThreadYield = false;
    bool bMatch = false;

#if KERNEL_USE_TIMEOUTS
    Timer clEventTimer;
    bool bUseTimer = false;
#endif

    // Ensure we're operating in a critical section while we determine
    // whether or not we need to block the current thread on this object.
    CS_ENTER();

    // Check to see whether or not the current mask matches any of the
    // desired bits.
    g_pstCurrent->SetEventFlagMask(usMask_);

    if ((eMode_ == EVENT_FLAG_ALL) || (eMode_ == EVENT_FLAG_ALL_CLEAR))
    {
        // Check to see if the flags in their current state match all of
        // the set flags in the event flag group, with this mask.
        if ((m_usSetMask & usMask_) == usMask_)
        {
            bMatch = true;
            g_pstCurrent->SetEventFlagMask(usMask_);
        }
    }
    else if ((eMode_ == EVENT_FLAG_ANY) || (eMode_ == EVENT_FLAG_ANY_CLEAR))
    {
        // Check to see if the existing flags match any of the set flags in
        // the event flag group  with this mask
        if (m_usSetMask & usMask_)
        {
            bMatch = true;
            g_pstCurrent->SetEventFlagMask(m_usSetMask & usMask_);
        }
    }

    // We're unable to match this pattern as-is, so we must block.
    if (!bMatch)
    {
        // Reset the current thread's event flag mask & mode
        g_pstCurrent->SetEventFlagMask(usMask_);
        g_pstCurrent->SetEventFlagMode(eMode_);

#if KERNEL_USE_TIMEOUTS
        if (ulTimeMS_)
        {
            g_pstCurrent->SetExpired(false);
            clEventTimer.Init();
            clEventTimer.Start(0, ulTimeMS_, TimedEventFlag_Callback, (void*)this);
            bUseTimer = true;
        }
#endif

        // Add the thread to the object's block-list.
        Block(g_pstCurrent);

        // Trigger that
        bThreadYield = true;
    }

    // If bThreadYield is set, it means that we've blocked the current thread,
    // and must therefore rerun the scheduler to determine what thread to
    // switch to.
    if (bThreadYield)
    {
        // Switch threads immediately
        Thread::Yield();
    }

    // Exit the critical section and return back to normal execution
    CS_EXIT();

    //!! If the Yield operation causes a new thread to be chosen, there will
    //!! Be a context switch at the above CS_EXIT().  The original calling
    //!! thread will not return back until a matching SetFlags call is made
    //!! or a timeout occurs.
#if KERNEL_USE_TIMEOUTS
    if (bUseTimer && bThreadYield)
    {
        clEventTimer.Stop();
    }
#endif

    return g_pstCurrent->GetEventFlagMask();
}

//---------------------------------------------------------------------------
K_USHORT EventFlag::Wait(K_USHORT usMask_, EventFlagOperation_t eMode_)
{
#if KERNEL_USE_TIMEOUTS
    return Wait_i(usMask_, eMode_, 0);
#else
    return Wait_i(usMask_, eMode_);
#endif
}

#if KERNEL_USE_TIMEOUTS
//---------------------------------------------------------------------------
K_USHORT EventFlag::Wait(K_USHORT usMask_, EventFlagOperation_t eMode_, K_ULONG ulTimeMS_)
{
    return Wait_i(usMask_, eMode_, ulTimeMS_);
}
#endif

//---------------------------------------------------------------------------
void EventFlag::Set(K_USHORT usMask_)
{
    Thread *pclPrev;
    Thread *pclCurrent;
    bool bReschedule = false;
    K_USHORT usNewMask;

    CS_ENTER();

    // Walk through the whole block list, checking to see whether or not
    // the current flag set now matches any/all of the masks and modes of
    // the threads involved.

    m_usSetMask |= usMask_;
    usNewMask = m_usSetMask;

    // Start at the head of the list, and iterate through until we hit the
    // "head" element in the list again.  Ensure that we handle the case where
    // we remove the first or last elements in the list, or if there's only
    // one element in the list.
    pclCurrent = static_cast<Thread*>(m_clBlockList.GetHead());

    // Do nothing when there are no objects blocking.
    if (pclCurrent)
    {
        // First loop - process every thread in the block-list and check to
        // see whether or not the current flags match the event-flag conditions
        // on the thread.
        do
        {
            pclPrev = pclCurrent;
            pclCurrent = static_cast<Thread*>(pclCurrent->GetNext());

            // Read the thread's event mask/mode
            K_USHORT usThreadMask = pclPrev->GetEventFlagMask();
            EventFlagOperation_t eThreadMode = pclPrev->GetEventFlagMode();

            // For the "any" mode - unblock the blocked threads if one or more bits
            // in the thread's bitmask match the object's bitmask
            if ((EVENT_FLAG_ANY == eThreadMode) || (EVENT_FLAG_ANY_CLEAR == eThreadMode))
            {
                if (usThreadMask & m_usSetMask)
                {
                    pclPrev->SetEventFlagMode(EVENT_FLAG_PENDING_UNBLOCK);
                    pclPrev->SetEventFlagMask(m_usSetMask & usThreadMask);
                    bReschedule = true;

                    // If the "clear" variant is set, then clear the bits in the mask
                    // that caused the thread to unblock.
                    if (EVENT_FLAG_ANY_CLEAR == eThreadMode)
                    {
                        usNewMask &=~ (usThreadMask & usMask_);
                    }
                }
            }
            // For the "all" mode, every set bit in the thread's requested bitmask must
            // match the object's flag mask.
            else if ((EVENT_FLAG_ALL == eThreadMode) || (EVENT_FLAG_ALL_CLEAR == eThreadMode))
            {
                if ((usThreadMask & m_usSetMask) == usThreadMask)
                {
                    pclPrev->SetEventFlagMode(EVENT_FLAG_PENDING_UNBLOCK);
                    pclPrev->SetEventFlagMask(usThreadMask);
                    bReschedule = true;

                    // If the "clear" variant is set, then clear the bits in the mask
                    // that caused the thread to unblock.
                    if (EVENT_FLAG_ALL_CLEAR == eThreadMode)
                    {
                        usNewMask &=~ (usThreadMask & usMask_);
                    }
                }
            }
        }
        // To keep looping, ensure that there's something in the list, and
        // that the next item isn't the head of the list.
        while (pclPrev != m_clBlockList.GetTail());

        // Second loop - go through and unblock all of the threads that
        // were tagged for unblocking.
        pclCurrent = static_cast<Thread*>(m_clBlockList.GetHead());
        bool bIsTail = false;
        do
        {
            pclPrev = pclCurrent;
            pclCurrent = static_cast<Thread*>(pclCurrent->GetNext());

            // Check to see if this is the condition to terminate the loop
            if (pclPrev == m_clBlockList.GetTail())
            {
                bIsTail = true;
            }

            // If the first pass indicated that this thread should be
            // unblocked, then unblock the thread
            if (pclPrev->GetEventFlagMode() == EVENT_FLAG_PENDING_UNBLOCK)
            {
                UnBlock(pclPrev);
            }
        }
        while (!bIsTail);
    }

    // If we awoke any threads, re-run the scheduler
    if (bReschedule)
    {
        Thread::Yield();
    }

    // Update the bitmask based on any "clear" operations performed along
    // the way
    m_usSetMask = usNewMask;

    // Restore interrupts - will potentially cause a context switch if a
    // thread is unblocked.
    CS_EXIT();
}

//---------------------------------------------------------------------------
void EventFlag::Clear(K_USHORT usMask_)
{
    // Just clear the bitfields in the local object.
    CS_ENTER();
    m_usSetMask &= ~usMask_;
    CS_EXIT();
}

//---------------------------------------------------------------------------
K_USHORT EventFlag::GetMask()
{
    // Return the presently held event flag values in this object.  Ensure
    // we get this within a critical section to guarantee atomicity.
    K_USHORT usReturn;
    CS_ENTER();
    usReturn = m_usSetMask;
    CS_EXIT();
    return usReturn;
}

#endif // KERNEL_USE_EVENTFLAG
