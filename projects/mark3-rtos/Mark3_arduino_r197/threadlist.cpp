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

    \file   threadlist.cpp

    \brief  Thread linked-list definitions

*/

#include "kerneltypes.h"
#include "ll.h"
#include "threadlist.h"
#include "thread.h"
#include "kernel_debug.h"
//---------------------------------------------------------------------------
#if defined __FILE_ID__
	#undef __FILE_ID__
#endif
#define __FILE_ID__ 	THREADLIST_CPP

//---------------------------------------------------------------------------
void ThreadList::SetPriority(K_UCHAR ucPriority_)
{
    m_ucPriority = ucPriority_;
}

//---------------------------------------------------------------------------
void ThreadList::SetFlagPointer( K_UCHAR *pucFlag_)
{
    m_pucFlag = pucFlag_;
}

//---------------------------------------------------------------------------
void ThreadList::Add(LinkListNode *node_) { 
    CircularLinkList::Add(node_);
    
    // If the head of the list isn't empty, 
    if (m_pstHead != NULL)
    {
        // We've specified a bitmap for this threadlist
        if (m_pucFlag)
        {
            // Set the flag for this priority level
            *m_pucFlag |= (1 << m_ucPriority);
        }
    }
}

//---------------------------------------------------------------------------
void ThreadList::Add(LinkListNode *node_, K_UCHAR *pucFlag_, K_UCHAR ucPriority_) {
    // Set the threadlist's priority level, flag pointer, and then add the
    // thread to the threadlist
    SetPriority(ucPriority_);
    SetFlagPointer(pucFlag_);
    Add(node_);
}

//---------------------------------------------------------------------------
void ThreadList::Remove(LinkListNode *node_) {
    // Remove the thread from the list
    CircularLinkList::Remove(node_);
    
    // If the list is empty...
    if (!m_pstHead)
    {
        // Clear the bit in the bitmap at this priority level
        if (m_pucFlag)
        {
            *m_pucFlag &= ~(1 << m_ucPriority);
        }
    }
}

//---------------------------------------------------------------------------
Thread *ThreadList::HighestWaiter()
{
	Thread *pclTemp = static_cast<Thread*>(GetHead());
	Thread *pclChosen = pclTemp;
	
	K_UCHAR ucMaxPri = 0;
    
    // Go through the list, return the highest-priority thread in this list.
	while(1)
	{
        // Compare against current max-priority thread
		if (pclTemp->GetPriority() >= ucMaxPri)
		{
			ucMaxPri = pclTemp->GetPriority();
			pclChosen = pclTemp;
		}
        
        // Break out if this is the last thread in the list
		if (pclTemp == static_cast<Thread*>(GetTail()))
		{
			break;
		}
        
		pclTemp = static_cast<Thread*>(pclTemp->GetNext());		
	} 
	return pclChosen;
}
