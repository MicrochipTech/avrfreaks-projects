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

    \file   message.cpp    

    \brief  Inter-thread communications via message passing

*/

#include "kerneltypes.h"
#include "mark3cfg.h"

#include "message.h"
#include "threadport.h"
#include "kernel_debug.h"

//---------------------------------------------------------------------------
#if defined __FILE_ID__
	#undef __FILE_ID__
#endif
#define __FILE_ID__ 	MESSAGE_CPP


#if KERNEL_USE_MESSAGE

#if KERNEL_USE_TIMEOUTS
	#include "timerlist.h"
#endif

Message GlobalMessagePool::m_aclMessagePool[8];
DoubleLinkList GlobalMessagePool::m_clList;

//---------------------------------------------------------------------------
void GlobalMessagePool::Init()
{
	K_UCHAR i;
    GlobalMessagePool::m_clList.Init();
    for (i = 0; i < GLOBAL_MESSAGE_POOL_SIZE; i++)
	{
		GlobalMessagePool::m_aclMessagePool[i].Init();
		GlobalMessagePool::m_clList.Add(&(GlobalMessagePool::m_aclMessagePool[i]));
	}
}

//---------------------------------------------------------------------------
void GlobalMessagePool::Push( Message *pclMessage_ )
{
	KERNEL_ASSERT( pclMessage_ );
	
	CS_ENTER();
		
	GlobalMessagePool::m_clList.Add(pclMessage_);
	
	CS_EXIT();
}
	
//---------------------------------------------------------------------------
Message *GlobalMessagePool::Pop()
{
	Message *pclRet;
	CS_ENTER();
	
	pclRet = static_cast<Message*>( GlobalMessagePool::m_clList.GetHead() );
    if (0 != pclRet)
    {
        GlobalMessagePool::m_clList.Remove( static_cast<LinkListNode*>( pclRet ) );
    }
	
	CS_EXIT();
	return pclRet;
}

//---------------------------------------------------------------------------
void MessageQueue::Init() 
{ 
    m_clSemaphore.Init(0, GLOBAL_MESSAGE_POOL_SIZE);
}

//---------------------------------------------------------------------------
Message *MessageQueue::Receive()
{
#if KERNEL_USE_TIMEOUTS
    return Receive_i(0);
#else
    return Receive_i();
#endif
}

//---------------------------------------------------------------------------
#if KERNEL_USE_TIMEOUTS
Message *MessageQueue::Receive( K_ULONG ulTimeWaitMS_)
{
    return Receive_i( ulTimeWaitMS_ );
}
#endif

//---------------------------------------------------------------------------
#if KERNEL_USE_TIMEOUTS
Message *MessageQueue::Receive_i( K_ULONG ulTimeWaitMS_ )
#else
Message *MessageQueue::Receive_i( void )
#endif
{
	Message *pclRet;
	
	// Block the current thread on the counting semaphore
#if KERNEL_USE_TIMEOUTS
    if (!m_clSemaphore.Pend(ulTimeWaitMS_))
    {
        return NULL;
    }
#else
    m_clSemaphore.Pend();
#endif
	
	CS_ENTER();
	
	// Pop the head of the message queue and return it
	pclRet = static_cast<Message*>( m_clLinkList.GetHead() );
	m_clLinkList.Remove(static_cast<Message*>(pclRet));
	
	CS_EXIT();
	
	return pclRet;	
}

//---------------------------------------------------------------------------
void MessageQueue::Send( Message *pclSrc_ )
{
	KERNEL_ASSERT( pclSrc_ );
	
	CS_ENTER();
	
	// Add the message to the head of the linked list
	m_clLinkList.Add( pclSrc_ );
		
	// Post the semaphore, waking the blocking thread for the queue.
	m_clSemaphore.Post();
	
	CS_EXIT();
}

//---------------------------------------------------------------------------
K_USHORT MessageQueue::GetCount()
{
	return m_clSemaphore.GetCount();
}
#endif //KERNEL_USE_MESSAGE
