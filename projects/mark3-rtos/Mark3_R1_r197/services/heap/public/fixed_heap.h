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
    \file "fixed_heap.h"
    
    \brief Fixed-block-size heaps
*/
#ifndef __FIXED_HEAP_H__
#define __FIXED_HEAP_H__

#include "kerneltypes.h"
#include "ll.h"

//---------------------------------------------------------------------------
/*!
    Single-block-size heap
*/
class BlockHeap
{
public:
    /*!
    
        \fn void *Create( void *pvHeap_, K_USHORT usSize_, K_USHORT usBlockSize_ )
        
        Create a single list heap in the blob of memory provided, with the 
        selected heap size, and the selected number of blocks.  Will create
        as many blocks as will fit in the usSize_ parameter
        
        \param pvHeap_ Pointer to the heap data to initialize
        \param usSize_ Size of the heap range in bytes
        \param usBlockSize_ Size of each heap block in bytes
        
        \return Pointer to the next heap element to initialize
    */
    void *Create( void *pvHeap_, K_USHORT usSize_, K_USHORT usBlockSize_ );
    
    /*! 
        \fn void *Alloc()
        
        Allocate a block of memory from this heap
        
        \return pointer to a block of memory, or 0 on failure
    */
    void *Alloc();
    
    /*!
        \fn void Free( void* pvData_ )
        
        Free a previously allocated block of memory
        
        \param pvData_ Pointer to a block of data previously allocated off
               the heap.
    */
    void Free( void* pvData_ );
    
    /*!
        \fn K_BOOL IsFree() 
        
        Returns the state of a heap - whether or not it has free elements.
        
        \return true if the heap is not full, false if the heap is full
    */
    K_BOOL IsFree() { return m_usBlocksFree != 0; }
    
protected:
    K_USHORT m_usBlocksFree;    //!< Number of blocks free in the heap
    
private:    
    DoubleLinkList m_clList;    //!< Linked list used to manage the blocks
};


class FixedHeap;

//---------------------------------------------------------------------------
/*!
    Heap configuration object
*/
class HeapConfig
{
public:
    K_USHORT m_usBlockSize;        //!< Block size in bytes
    K_USHORT m_usBlockCount;    //!< Number of blocks to create @ this size
    friend class FixedHeap;
protected:
    BlockHeap m_clHeap;            //!< BlockHeap object used by the allocator
};

//---------------------------------------------------------------------------
/*!
    Fixed-size-block heap allocator with multiple block sizes.
*/
class FixedHeap
{
public:
    /*!
        \fn void Create( void *pvHeap_, HeapConfig *pclHeapConfig_ )
    
        Creates a heap in a provided blob of memory with lists of fixed-size
        blocks configured based on the associated configuration data.  A
        heap must be created before it can be allocated/freed.
    
        \param pvHeap_ Pointer to the data blob that will contain the heap
        \param pclHeapConfig_ Pointer to the array of config objects that
               define how the heap is laid out in memory, and how many
               blocks of what size are included.  The objects in the 
               array must be initialized, starting from smallest block-size
               to largest, with the final entry in the table have a 
               0-block size, indicating end-of-configuration.
    */
    void Create( void *pvHeap_, HeapConfig *pclHeapConfig_ );

    /*!
        \fn void *Alloc( K_USHORT usSize_ )
        
        Allocate a blob of memory from the heap.  If no appropriately-sized
        data block is available, will return NULL.  Note, this API is thread-
        safe, and interrupt safe.
                
        \param usSize_ Size (in bytes) to allocate from the heap
        
        \return Pointer to a block of data allocated, or 0 on error.
    */
    void *Alloc( K_USHORT usSize_ );
    
    /*!
        \fn void Free( void *pvNode_ )
        
        Free a previously-allocated block of memory to the heap it was 
        originally allocated from.  This must point to the block of
        memory at its originally-returned pointer, and not an address 
        within an allocated blob (as supported by some allocators).
        
        \param pvNode_ Pointer to the previously-allocated block of memory
        
    */
    static void Free( void *pvNode_ );
    
private:
    HeapConfig *m_paclHeaps;    //!< Pointer to the configuration data used by the heap
};

#endif

