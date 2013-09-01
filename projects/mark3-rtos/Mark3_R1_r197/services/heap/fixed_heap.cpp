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
    \file fixed_heap.cpp
    
    Fixed-block-size memory management.  This allows a user to create heaps
    containing multiple lists, each list containing a linked-list of blocks
    that are each the same size.  As a result of the linked-list format, 
    these heaps are very fast - requiring only a linked list pop/push to 
    allocated/free memory.  Array traversal is required to allow for the optimal
    heap to be used.  Blocks are chosen from the first heap with free blocks
    large enough to fulfill the request.
    
    Only simple malloc/free functionlality is supported in this implementation,
    no complex vector-allocate or reallocation functions are supported.
    
    Heaps are protected by critical section, and are thus thread-safe.
    
    When creating a heap, a user supplies an array of heap configuration objects,
    which determines how many objects of what size are available.
        
    The configuration objects are defined from smallest list to largest, 
    the memory to back the heap is supplied as a pointer to a "blob" of memory
    which will be used to create the underlying heap objects that make up the
    heap internal data structures.  This blob must be large enough to contain
    all of the requested heap objects, with all of the additional metadata 
    required to manage the objects.    
    
    Multiple heaps can be created using this library (heaps are not singleton). 
*/

#include "kerneltypes.h"
#include "fixed_heap.h"
#include "threadport.h"

//---------------------------------------------------------------------------
void *BlockHeap::Create( void *pvHeap_, K_USHORT usSize_, K_USHORT usBlockSize_ )
{
    K_USHORT usNodeCount = usSize_ / 
                                 (usBlockSize_ + sizeof(LinkListNode) + sizeof(void*));
    K_ADDR adNode = (K_ADDR)pvHeap_;
    K_ADDR adMaxNode = (K_ADDR)((K_ADDR)pvHeap_ + (K_ADDR)usSize_);
    m_clList.Init();
    
    // Create a heap (linked-list nodes + byte pool) in the middle of
    // the data blob
    for (K_USHORT i = 0; i < usNodeCount; i++ )
    {
        // Create a pointer back to the source list.  
        BlockHeap **pclTemp = (BlockHeap**)(adNode + sizeof(LinkListNode));
        *pclTemp = (BlockHeap*)(this);
        
        // Add the node to the block list
        m_clList.Add( (LinkListNode*)adNode );
        
        // Move the pointer in the pool to point to the next block to allocate
        adNode += (usBlockSize_ + sizeof(LinkListNode) + sizeof(BlockHeap*));
        
        // Bail if we would be going past the end of the allocated space...
        if ((K_ULONG)adNode >= (K_ULONG)adMaxNode)
        {
            break;
        }
    }
    m_usBlocksFree = usNodeCount;
    
    // Return pointer to end of heap (used for heap-chaining)
    return (void*)adNode;
}

//---------------------------------------------------------------------------
void *BlockHeap::Alloc()
{
    LinkListNode *pclNode = m_clList.GetHead();

    // Return the first node from the head of the list
    if (pclNode)
    {
        m_clList.Remove( pclNode );
        m_usBlocksFree--;
        
        // Account for block-management metadata
        return (void*)((K_ADDR)pclNode + sizeof(LinkListNode) + sizeof(void*));
    }
    
    // Or null, if the heap is empty.
    return 0;
}

//---------------------------------------------------------------------------
void BlockHeap::Free( void* pvData_ )
{
    // Compute the address of the original object (class metadata included)
    LinkListNode *pclNode = (LinkListNode*)((K_ADDR)pvData_ - sizeof(LinkListNode) - sizeof(void*));
    
    // Add the object back to the block data pool
    m_clList.Add(pclNode);
    m_usBlocksFree++;
}

//---------------------------------------------------------------------------
void FixedHeap::Create( void *pvHeap_, HeapConfig *pclHeapConfig_ )
{
    K_USHORT i = 0;
    void *pvTemp = pvHeap_;
    while( pclHeapConfig_[i].m_usBlockSize != 0)
    {
        pvTemp = pclHeapConfig_[i].m_clHeap.Create
                    (pvTemp, 
                     (pclHeapConfig_[i].m_usBlockSize +sizeof(LinkListNode) + sizeof(void*)) *
                     pclHeapConfig_[i].m_usBlockCount,
                     pclHeapConfig_[i].m_usBlockSize );
        i++;
    }
    m_paclHeaps = pclHeapConfig_;
}

//---------------------------------------------------------------------------
void *FixedHeap::Alloc( K_USHORT usSize_ )
{
    void *pvRet = 0;
    K_USHORT i = 0;
    
    // Go through all heaps, trying to find the smallest one that 
    // has a free item to satisfy the allocation
    while (m_paclHeaps[i].m_usBlockSize != 0)
    {
        CS_ENTER();        
        if ((m_paclHeaps[i].m_usBlockSize >= usSize_) && m_paclHeaps[i].m_clHeap.IsFree() )
        {
            // Found a match            
            pvRet = m_paclHeaps[i].m_clHeap.Alloc();        
        }
        CS_EXIT();

        // Return an object if found
        if (pvRet)
        {
            return pvRet;
        }
        i++;
    }
    
    // Or null on no-match
    return pvRet;
}

//---------------------------------------------------------------------------
void FixedHeap::Free( void *pvNode_ )
{
    // Compute the pointer to the block-heap this block belongs to, and
    // return it.
    CS_ENTER();
    BlockHeap **pclHeap = (BlockHeap**)((K_ADDR)pvNode_ - sizeof(BlockHeap*));
    (*pclHeap)->Free(pvNode_);
    CS_EXIT();
}


