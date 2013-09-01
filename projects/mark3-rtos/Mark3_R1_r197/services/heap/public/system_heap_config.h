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
    \file "system_heap_config.h"
    
    \brief System heap configuration - defines the block sizes and counts 
           used to fulfill system/service allocations.
*/
#ifndef __SYSTEM_HEAP_CONFIG_H__
#define __SYSTEM_HEAP_CONFIG_H__

#include "kerneltypes.h"

//---------------------------------------------------------------------------
/*!
    Set this to "1" if you want the system heap to be built as part of this
    library.
*/
#define USE_SYSTEM_HEAP        (1)

//---------------------------------------------------------------------------
/*!
    Define the number of heap block sizes that we want to have attached to
    our system heap.
*/
#define HEAP_NUM_SIZES        (3)

//---------------------------------------------------------------------------
/*!
    Define the block sizes for each of the fixed-size blocks that will be 
    managed by our heaps.  Must be defined in incrementing order.
*/
#define HEAP_BLOCK_SIZE_1        ((K_USHORT) 8)
#define HEAP_BLOCK_SIZE_2        ((K_USHORT) 16)
#define HEAP_BLOCK_SIZE_3        ((K_USHORT) 24)
#define HEAP_BLOCK_SIZE_4        ((K_USHORT) 32)
#define HEAP_BLOCK_SIZE_5        ((K_USHORT) 48)
#define HEAP_BLOCK_SIZE_6        ((K_USHORT) 64)
#define HEAP_BLOCK_SIZE_7        ((K_USHORT) 96)
#define HEAP_BLOCK_SIZE_8        ((K_USHORT) 128)
#define HEAP_BLOCK_SIZE_9        ((K_USHORT) 192)
#define HEAP_BLOCK_SIZE_10        ((K_USHORT) 256)

//---------------------------------------------------------------------------
/*!
    Define the number of blocks in each bin, tailored for a particular 
    application
*/
#define HEAP_BLOCK_COUNT_1        ((K_USHORT) 4)
#define HEAP_BLOCK_COUNT_2        ((K_USHORT) 4)
#define HEAP_BLOCK_COUNT_3        ((K_USHORT) 2)
#define HEAP_BLOCK_COUNT_4        ((K_USHORT) 2)
#define HEAP_BLOCK_COUNT_5        ((K_USHORT) 2)
#define HEAP_BLOCK_COUNT_6        ((K_USHORT) 2)
#define HEAP_BLOCK_COUNT_7        ((K_USHORT) 1)
#define HEAP_BLOCK_COUNT_8        ((K_USHORT) 1)
#define HEAP_BLOCK_COUNT_9        ((K_USHORT) 1)
#define HEAP_BLOCK_COUNT_10        ((K_USHORT) 1)

#endif

