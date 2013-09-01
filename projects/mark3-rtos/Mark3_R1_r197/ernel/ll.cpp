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

    \file   ll.cpp   

    \brief  Core Linked-List implementation, from which all kernel objects
			are derived
*/

#include "kerneltypes.h"
#include "kernel.h"
#include "ll.h"
#include "kernel_debug.h"

//---------------------------------------------------------------------------
#if defined __FILE_ID__
	#undef __FILE_ID__
#endif
#define __FILE_ID__ 	LL_CPP

//---------------------------------------------------------------------------
void LinkListNode::ClearNode()
{
    next = NULL;
    prev = NULL;
}

//---------------------------------------------------------------------------
void DoubleLinkList::Add(LinkListNode *node_)
{
	KERNEL_ASSERT( node_ );
	
    // Add a node to the end of the linked list.
    if (!m_pstHead)
    {
        // If the list is empty, initilize the nodes
        m_pstHead = node_;          
        m_pstTail = node_;

        m_pstHead->prev = NULL;
        m_pstTail->next = NULL;
        return;
    }
    
    // Move the tail node, and assign it to the new node just passed in
    m_pstTail->next = node_;
    node_->prev = m_pstTail;
    node_->next = NULL;    
    m_pstTail = node_;
}

//---------------------------------------------------------------------------
void DoubleLinkList::Remove(LinkListNode *node_)
{
	KERNEL_ASSERT( node_ );

    if (node_->prev)
    {
#if SAFE_UNLINK
        if (node_->prev->next != node_)
        {
            Kernel::Panic(PANIC_LIST_UNLINK_FAILED);
        }
#endif
        node_->prev->next = node_->next;
    }
    if (node_->next)
    {
#if SAFE_UNLINK
        if (node_->next->prev != node_)
        {
            Kernel::Panic(PANIC_LIST_UNLINK_FAILED);
        }
#endif
        node_->next->prev = node_->prev;
    }
    if (node_ == m_pstHead)
    {        
        m_pstHead = node_->next;
    }
    if (node_ == m_pstTail)
    {
        m_pstTail = node_->prev;
    }
    
    node_->ClearNode();
}

//---------------------------------------------------------------------------
void CircularLinkList::Add(LinkListNode *node_)
{
	KERNEL_ASSERT( node_ );

    // Add a node to the end of the linked list.
    if (!m_pstHead)
    {
        // If the list is empty, initilize the nodes
        m_pstHead = node_;          
        m_pstTail = node_;

        m_pstHead->prev = m_pstHead;
        m_pstHead->next = m_pstHead;
        return;
    }
    
    // Move the tail node, and assign it to the new node just passed in
    m_pstTail->next = node_;
    node_->prev = m_pstTail;
    node_->next = m_pstHead;    
    m_pstTail = node_;
    m_pstHead->prev = node_;
}

//---------------------------------------------------------------------------
void CircularLinkList::Remove(LinkListNode *node_)
{
	KERNEL_ASSERT( node_ );
	
    // Check to see if this is the head of the list...
    if ((node_ == m_pstHead) && (m_pstHead == m_pstTail)) 
    {
        // Clear the head and tail pointers - nothing else left.
        m_pstHead = NULL;
        m_pstTail = NULL;
        return;
    }
    
#if SAFE_UNLINK
    // Verify that all nodes are properly connected
    if ((node_->prev->next != node_) || (node_->next->prev != node_))
    {
        Kernel::Panic(PANIC_LIST_UNLINK_FAILED);
    }
#endif

    // This is a circularly linked list - no need to check for connection,
    // just remove the node.
    node_->next->prev = node_->prev;
    node_->prev->next = node_->next;
    
    if (node_ == m_pstHead)
    {
        m_pstHead = m_pstHead->next;
    }
    if (node_ == m_pstTail)
    {
        m_pstTail = m_pstTail->prev;
    }
    node_->ClearNode();
}

//---------------------------------------------------------------------------
void CircularLinkList::PivotForward()
{
    if (m_pstHead)
    {
        m_pstHead = m_pstHead->next;
        m_pstTail = m_pstTail->next;
    }
}

//---------------------------------------------------------------------------
void CircularLinkList::PivotBackward()
{
    if (m_pstHead)
    {
        m_pstHead = m_pstHead->prev;
        m_pstTail = m_pstTail->prev;
    }
}
