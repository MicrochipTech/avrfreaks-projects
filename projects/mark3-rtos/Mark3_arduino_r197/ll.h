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

    \file   ll.h    

    \brief  Core linked-list declarations, used by all kernel list types
    
    At the heart of RTOS data structures are linked lists.  Having a robust
    and efficient set of linked-list types that we can use as a foundation for 
    building the rest of our kernel types allows us to keep our RTOS code 
    efficient and logically-separated.

    So what data types rely on these linked-list classes?
    
    -Threads
    -ThreadLists
    -The Scheduler
    -Timers,
    -The Timer Scheduler
    -Blocking objects (Semaphores, Mutexes, etc...)
    
    Pretty much everything in the kernel uses these linked lists.  By 
    having objects inherit from the base linked-list node type, we're able
    to leverage the double and circular linked-list classes to manager
    virtually every object type in the system without duplicating code.
    These functions are very efficient as well, allowing for very deterministic
    behavior in our code.
    
*/

#ifndef __LL_H__
#define __LL_H__

#include "kerneltypes.h"

//---------------------------------------------------------------------------
#ifndef NULL
#define NULL        (0)
#endif

//---------------------------------------------------------------------------
/*!
    Forward declarations of linked-list classes that are used further on in
    this module.  This allows us to also specify which friend classes can 
    access the LinkListNode type.
*/
class LinkList;
class DoubleLinkList;
class CircularLinkList;

//---------------------------------------------------------------------------
/*!
    Basic linked-list node data structure.  This data is managed by the 
    linked-list class types, and can be used transparently between them.
*/
class LinkListNode
{
protected:
	
    LinkListNode *next;     //!< Pointer to the next node in the list
    LinkListNode *prev;     //!< Pointer to the previous node in the list

    LinkListNode() { }

    /*!
        \fn void ClearNode()
        
        Initialize the linked list node, clearing its next and previous node.
    */
    void ClearNode();

public:
    /*!
        \fn LinkListNode *GetNext();
        
        Returns a pointer to the next node in the list.
        
        \return a pointer to the next node in the list.
    */
    LinkListNode *GetNext(void) { return next; }
    
    /*!
        \fn LinkListNode *GetPrev();
        
        Returns a pointer to the previous node in the list.
        
        \return a pointer to the previous node in the list.
    */
    LinkListNode *GetPrev(void) { return prev; }
		
    friend class LinkList;  
    friend class DoubleLinkList;  
    friend class CircularLinkList;  
};

//---------------------------------------------------------------------------
/*!
    Abstract-data-type from which all other linked-lists are derived
*/
class LinkList
{  
protected:
    LinkListNode *m_pstHead;    //!< Pointer to the head node in the list
    LinkListNode *m_pstTail;    //!< Pointer to the tail node in the list
    
public:
    /*!
        Clear the linked list.
    */
    void Init(){ m_pstHead = NULL; m_pstTail = NULL; }
    
    /*!
        \fn void Add(LinkListNode *node_)
        
        Add the linked list node to this linked list
        
        \param node_ Pointer to the node to add
    */
    virtual void Add(LinkListNode *node_) = 0;
    
    /*!
        \fn void Remove(LinkListNode *node_)
        
        Add the linked list node to this linked list
        
        \param node_ Pointer to the node to remove
    */
    virtual void Remove(LinkListNode *node_) = 0;
    
    /*!
        \fn LinkListNode *GetHead()
        
        Get the head node in the linked list
        
        \return Pointer to the head node in the list
    */
    LinkListNode *GetHead() { return m_pstHead; }
    
	/*!
        \fn LinkListNode *GetTail()
        
        Get the tail node of the linked list
        
        \return Pointer to the tail node in the list
    */
    LinkListNode *GetTail() { return m_pstTail; }
};

//---------------------------------------------------------------------------
/*!
    Doubly-linked-list data type, inherited from the base LinkList type.
*/
class DoubleLinkList : public LinkList
{
public:
    /*!
        Default constructor - initializes the head/tail nodes to NULL
    */
    DoubleLinkList() { m_pstHead = NULL; m_pstTail = NULL; }
    
    /*!
        \fn void Add(LinkListNode *node_)
        
        Add the linked list node to this linked list
        
        \param node_ Pointer to the node to add
    */
    virtual void Add(LinkListNode *node_);
    
    /*!
        \fn void Remove(LinkListNode *node_)
        
        Add the linked list node to this linked list
        
        \param node_ Pointer to the node to remove
    */
    virtual void Remove(LinkListNode *node_);
};

//---------------------------------------------------------------------------
/*!
    Circular-linked-list data type, inherited from the base LinkList type.
*/
class CircularLinkList : public LinkList
{
public:
    CircularLinkList() { m_pstHead = NULL; m_pstTail = NULL; }
    
    /*!
        \fn void Add(LinkListNode *node_)
        
        Add the linked list node to this linked list
        
        \param node_ Pointer to the node to add
    */
    virtual void Add(LinkListNode *node_);
    
    /*!
        \fn void Remove(LinkListNode *node_)
        
        Add the linked list node to this linked list
        
        \param node_ Pointer to the node to remove
    */    
    virtual void Remove(LinkListNode *node_);

    /*!
        \fn void PivotForward()
        
        Pivot the head of the circularly linked list forward
        ( Head = Head->next, Tail = Tail->next )                
    */
    void PivotForward();
    
    /*!
        \fn void PivotBackward()
        
        Pivot the head of the circularly linked list backward
        ( Head = Head->prev, Tail = Tail->prev )        
    */
    void PivotBackward();
};

#endif
