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
    \file "system_heap.h"
    
    \brief Global system-heap implmentation.  Provides a basic malloc()/free()
           allocation scheme.
*/

#ifndef __SYSTEM_HEAP_H__
#define __SYSTEM_HEAP_H__

#include "system_heap_config.h"
#include "fixed_heap.h"

#if USE_SYSTEM_HEAP

//---------------------------------------------------------------------------
/*!
    Really ugly computations used to auto-size the heap footprint based on 
    the user-configuration data. (don't touch this!!!)
*/
#if HEAP_NUM_SIZES > 0
    #define HEAP_RAW_SIZE_1 ((HEAP_BLOCK_SIZE_1 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_1 )
#else
    #define HEAP_RAW_SIZE_1 0
#endif

#if HEAP_NUM_SIZES > 1
    #define HEAP_RAW_SIZE_2 ((HEAP_BLOCK_SIZE_2 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_2 )
#else
    #define HEAP_RAW_SIZE_2 0
#endif

#if HEAP_NUM_SIZES > 2
    #define HEAP_RAW_SIZE_3 ((HEAP_BLOCK_SIZE_3 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_3 )
#else
    #define HEAP_RAW_SIZE_3 0
#endif

#if HEAP_NUM_SIZES > 3
    #define HEAP_RAW_SIZE_4 ((HEAP_BLOCK_SIZE_4 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_4 )
#else
    #define HEAP_RAW_SIZE_4 0
#endif

#if HEAP_NUM_SIZES > 4
    #define HEAP_RAW_SIZE_5 ((HEAP_BLOCK_SIZE_5 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_5 )
#else
    #define HEAP_RAW_SIZE_5 0
#endif

#if HEAP_NUM_SIZES > 5
    #define HEAP_RAW_SIZE_6 ((HEAP_BLOCK_SIZE_6 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_6 )
#else
    #define HEAP_RAW_SIZE_6 0
#endif

#if HEAP_NUM_SIZES > 6
    #define HEAP_RAW_SIZE_7 ((HEAP_BLOCK_SIZE_7 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_7 )
#else
    #define HEAP_RAW_SIZE_7 0
#endif

#if HEAP_NUM_SIZES > 7
    #define HEAP_RAW_SIZE_8 ((HEAP_BLOCK_SIZE_8 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_8 )
#else
    #define HEAP_RAW_SIZE_8 0
#endif

#if HEAP_NUM_SIZES > 8
    #define HEAP_RAW_SIZE_9 ((HEAP_BLOCK_SIZE_9 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_9 )
#else
    #define HEAP_RAW_SIZE_9 0
#endif

#if HEAP_NUM_SIZES > 9
    #define HEAP_RAW_SIZE_10 ((HEAP_BLOCK_SIZE_10 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_10 )
#else
    #define HEAP_RAW_SIZE_10 0
#endif

#if HEAP_NUM_SIZES > 10
    #define HEAP_RAW_SIZE_11 ((HEAP_BLOCK_SIZE_11 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_11 )
#else
    #define HEAP_RAW_SIZE_11 0
#endif

#if HEAP_NUM_SIZES > 11
    #define HEAP_RAW_SIZE_12 ((HEAP_BLOCK_SIZE_12 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_12 )
#else
    #define HEAP_RAW_SIZE_12 0
#endif

#if HEAP_NUM_SIZES > 12
    #define HEAP_RAW_SIZE_13 ((HEAP_BLOCK_SIZE_13 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_13 )
#else
    #define HEAP_RAW_SIZE_13 0
#endif

#if HEAP_NUM_SIZES > 13
    #define HEAP_RAW_SIZE_14 ((HEAP_BLOCK_SIZE_14 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_14 )
#else
    #define HEAP_RAW_SIZE_14 0
#endif

#if HEAP_NUM_SIZES > 14
    #define HEAP_RAW_SIZE_15 ((HEAP_BLOCK_SIZE_15 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_15 )
#else
    #define HEAP_RAW_SIZE_15 0
#endif

#if HEAP_NUM_SIZES > 15
    #define HEAP_RAW_SIZE_16 ((HEAP_BLOCK_SIZE_16 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_16 )
#else
    #define HEAP_RAW_SIZE_16 0
#endif

#if HEAP_NUM_SIZES > 16
    #define HEAP_RAW_SIZE_17 ((HEAP_BLOCK_SIZE_17 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_17 )
#else
    #define HEAP_RAW_SIZE_17 0
#endif

#if HEAP_NUM_SIZES > 17
    #define HEAP_RAW_SIZE_18 ((HEAP_BLOCK_SIZE_18 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_18 )
#else
    #define HEAP_RAW_SIZE_18 0
#endif

#if HEAP_NUM_SIZES > 18
    #define HEAP_RAW_SIZE_19 ((HEAP_BLOCK_SIZE_19 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_19 )
#else
    #define HEAP_RAW_SIZE_19 0
#endif

#if HEAP_NUM_SIZES > 19
    #define HEAP_RAW_SIZE_20 ((HEAP_BLOCK_SIZE_20 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_20 )
#else
    #define HEAP_RAW_SIZE_20 0
#endif

#if HEAP_NUM_SIZES > 20
    #define HEAP_RAW_SIZE_21 ((HEAP_BLOCK_SIZE_21 + sizeof(LinkListNode) + sizeof(void*)) * HEAP_BLOCK_COUNT_21 )
#else
    #define HEAP_RAW_SIZE_21 0
#endif

//---------------------------------------------------------------------------
#define HEAP_RAW_SIZE    \
HEAP_RAW_SIZE_1 + \
HEAP_RAW_SIZE_2 + \
HEAP_RAW_SIZE_3 + \
HEAP_RAW_SIZE_4 + \
HEAP_RAW_SIZE_5 + \
HEAP_RAW_SIZE_6 + \
HEAP_RAW_SIZE_7 + \
HEAP_RAW_SIZE_8 + \
HEAP_RAW_SIZE_9 + \
HEAP_RAW_SIZE_10 + \
HEAP_RAW_SIZE_11 + \
HEAP_RAW_SIZE_12 + \
HEAP_RAW_SIZE_13 + \
HEAP_RAW_SIZE_14 + \
HEAP_RAW_SIZE_15 + \
HEAP_RAW_SIZE_16 + \
HEAP_RAW_SIZE_17 + \
HEAP_RAW_SIZE_18 + \
HEAP_RAW_SIZE_19 + \
HEAP_RAW_SIZE_20 + \
HEAP_RAW_SIZE_21 
    
//---------------------------------------------------------------------------
/*!
 * \brief The SystemHeap class implements a heap which is accessible from
 * all components in the system.
 */
class SystemHeap
{
public:    
    /*!
     * \brief Init Initialize the system heap prior to usage.
     */
    static void  Init(void);

    /*!
     * \brief Alloc allocate a block of data from the heap
     * \param usSize_ size of the block (in bytes) to allocate
     * \return pointer to a block of data allocated from the heap, or NULL on
     *         failure.
     */
    static void* Alloc(K_USHORT usSize_);

    /*!
     * \brief Free free a block of data previously allocated from the heap
     * \param pvData_ Pointer to a block of data allocated from the system heap
     */
    static void  Free(void *pvData_);

private:
    static K_UCHAR m_pucRawHeap[ HEAP_RAW_SIZE ]; //!< Raw heap buffer
    static HeapConfig m_pclSystemHeapConfig[ HEAP_NUM_SIZES + 1 ]; //!< Heap configuration metadata
    static FixedHeap m_clSystemHeap; //!< Heap management object
    static bool m_bInit; //!< True if initialized, false if uninitialized
};

#endif // USE_SYSTEM_HEAP

#endif // __SYSTEM_HEAP_H__
