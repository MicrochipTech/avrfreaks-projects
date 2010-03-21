//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//--------------------------------------------------------------------------- 
/*!
	\file:  heap.h
	
	Description:
		static block-size heap configuration
*/

#ifndef __HEAP_H_
#define __HEAP_H_

#include "types.h"

//---------------------------------------------------------------------------
// If you change the number of block sizes, you must change this value, as well as 
// add the additional information for the new sizes to the table below
#define NUM_BLOCK_SIZES		(5)

//---------------------------------------------------------------------------
// Define the pool element sizes - this is arbitrary and can be set by the user.
// Just make sure that these are defined in smallest-largest order.
#define ELEMENT_SIZE_1		(8)
#define ELEMENT_SIZE_2		(12)
#define ELEMENT_SIZE_3		(16)
#define ELEMENT_SIZE_4		(32)
#define ELEMENT_SIZE_5		(64)

//---------------------------------------------------------------------------
// Define the block sizes - this is also arbitrary.
#define NUM_BLOCK_1			(10)
#define NUM_BLOCK_2			(6)
#define NUM_BLOCK_3			(4)
#define NUM_BLOCK_4			(2)
#define NUM_BLOCK_5			(1)

//---------------------------------------------------------------------------
// Define the number of elements in the heap
#define HEAP_NUM_ELEMENTS	(NUM_BLOCK_1 + NUM_BLOCK_2 + NUM_BLOCK_3 + NUM_BLOCK_4 + NUM_BLOCK_5)

//---------------------------------------------------------------------------
// Compute the block sizes
#define SIZE_BLOCK_1		(ELEMENT_SIZE_1 * NUM_BLOCK_1)
#define SIZE_BLOCK_2		(ELEMENT_SIZE_2 * NUM_BLOCK_2)
#define SIZE_BLOCK_3		(ELEMENT_SIZE_3 * NUM_BLOCK_3)
#define SIZE_BLOCK_4		(ELEMENT_SIZE_4 * NUM_BLOCK_4)
#define SIZE_BLOCK_5		(ELEMENT_SIZE_5 * NUM_BLOCK_5)

//---------------------------------------------------------------------------
// Define the size of the heap pool (in bytes)
#define HEAP_SIZE_BYTES		(SIZE_BLOCK_1 + SIZE_BLOCK_2 + SIZE_BLOCK_3 +SIZE_BLOCK_4 + SIZE_BLOCK_5)

//---------------------------------------------------------------------------
//!< Defines the structure of a heap block
typedef struct
{
	USHORT usSize;		//!< Size of the heap block in bytes
	USHORT usOffset;	//!< Offset of the heap block in the array
	BOOL bInUse;		//!< In-use flag
} HEAP_BLOCK_STRUCT;

//---------------------------------------------------------------------------
//!< Defines the structure of the heap as a whole
typedef struct
{
	HEAP_BLOCK_STRUCT astHeapElements[HEAP_NUM_ELEMENTS];	//!< Array of heap block elements	
	UCHAR aucPool[HEAP_SIZE_BYTES];	 //!< Raw data pool
} HEAP_STRUCT;

//---------------------------------------------------------------------------
typedef struct
{
	USHORT usNumBlocks;	//!< Number of elements in this block
	USHORT usBlockSize; //!< Size of the elements in this block
} HEAP_INIT_STRUCT;

//---------------------------------------------------------------------------
void Heap_Init(void);
UCHAR *Heap_Alloc(USHORT usSize_);
void Heap_Free(void *pvData_);

#endif 
