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

    \file   blocking.cpp

    \brief  Implementation of base class for blocking objects
*/

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "kernel_debug.h"

#include "blocking.h"
#include "thread.h"

//---------------------------------------------------------------------------
#if defined __FILE_ID__
	#undef __FILE_ID__
#endif
#define __FILE_ID__ 	BLOCKING_CPP

#if KERNEL_USE_SEMAPHORE || KERNEL_USE_MUTEX
//---------------------------------------------------------------------------
void BlockingObject::Block(Thread *pclThread_)
{
	KERNEL_ASSERT( pclThread_ );
	KERNEL_TRACE_1( STR_THREAD_BLOCK_1, (K_USHORT)pclThread_->GetID() );
	
    // Remove the thread from its current thread list (the "owner" list)
    // ... And add the thread to this object's block list    
    Scheduler::Remove(pclThread_);
    m_clBlockList.Add(pclThread_);
    
    // Set the "current" list location to the blocklist for this thread
    pclThread_->SetCurrent(&m_clBlockList);
	
}

//---------------------------------------------------------------------------
void BlockingObject::UnBlock(Thread *pclThread_)
{
	KERNEL_ASSERT( pclThread_ );
	KERNEL_TRACE_1( STR_THREAD_UNBLOCK_1, (K_USHORT)pclThread_->GetID() );
    
	// Remove the thread from its current thread list (the "owner" list)
    pclThread_->GetCurrent()->Remove(pclThread_);
    
    // Put the thread back in its active owner's list.  This is usually
    // the ready-queue at the thread's original priority.    
    Scheduler::Add(pclThread_);
    
    // Tag the thread's current list location to its owner
    pclThread_->SetCurrent(pclThread_->GetOwner());
}

#endif
