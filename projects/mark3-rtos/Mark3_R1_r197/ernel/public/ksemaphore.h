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

    \file   ksemaphore.h    

    \brief  Semaphore Blocking Object class declarations

*/

#ifndef __KSEMAPHORE_H__
#define __KSEMAPHORE_H__

#include "kerneltypes.h"
#include "mark3cfg.h"

#include "blocking.h"
#include "threadlist.h"

#if KERNEL_USE_SEMAPHORE

//---------------------------------------------------------------------------
/*!
    Counting semaphore, based on BlockingObject base class.
*/
class Semaphore : public BlockingObject
{
public:
	/*!
        \fn void Init(K_USHORT usInitVal_, K_USHORT usMaxVal_)

        Initialize a semaphore before use.  Must be called
        before post/pend operations.
        
        \param usInitVal_ Initial value held by the semaphore
        \param usMaxVal_ Maximum value for the semaphore
	*/
    void Init(K_USHORT usInitVal_, K_USHORT usMaxVal_);
    
	/*!
        \fn void Post();
        
        Increment the semaphore count.

        \return true if the semaphore was posted, false if the count
                is already maxed out.
	*/
    bool Post();
    
	/*!
        \fn void Pend();
        
        Decrement the semaphore count.  If the count is zero,
        the thread will block until the semaphore is pended.        
	*/
	void Pend();
    
	
	/*!
		\fn K_USHORT GetCount()
		
		Return the current semaphore counter. This can be 
		used by a thread to bypass blocking on a semaphore -
		allowing it to do other things until a non-zero count
		is returned, instead of blocking until the semaphore
		is posted.
		
		\return The current semaphore counter value.
	*/
	K_USHORT GetCount();
	
#if KERNEL_USE_TIMEOUTS
	/*!
        \fn bool Pend( K_ULONG ulWaitTimeMS_ );
        
        Decrement the semaphore count.  If the count is zero,
        the thread will block until the semaphore is pended.
		If the specified interval expires before the thread is
		unblocked, then the status is returned back to the user.
		
		\return true - semaphore was acquired before the timeout
				false - timeout occurred before the semaphore was claimed.
	*/
	bool Pend( K_ULONG ulWaitTimeMS_);
	
	/*!
		\fn void WakeMe(Thread *pclChosenOne_)
		
		Wake a thread blocked on the semaphore.  This is an
		internal function used for implementing timed semaphores
		relying on timer callbacks.  Since these do not have
		access to the private data of the semaphore and its base
		classes, we have to wrap this as a public method - do not
		use this for any other purposes.
	*/
	void WakeMe(Thread *pclChosenOne_);
#endif	
	
private:

	/*!
        \fn K_UCHAR WakeNext();
        
        Wake the next thread waiting on the semaphore.
	*/
    K_UCHAR WakeNext();


#if KERNEL_USE_TIMEOUTS
    bool Pend_i( K_ULONG ulWaitTimeMS_ );
#else
    void Pend_i( void );
#endif
    
    K_USHORT m_usValue;
    K_USHORT m_usMaxValue;
	
	
};

#endif //KERNEL_USE_SEMAPHORE

#endif
