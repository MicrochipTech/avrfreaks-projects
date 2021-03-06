//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//--------------------------------------------------------------------------- 
/*!
	\file:  heap.cpp
	
	Description:
		Static block-size heap implementation
*/

#include "types.h"
#include "taskport.h"
#include "heap.h"

#if KERNEL_USE_HEAP


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
static const HEAP_INIT_STRUCT astHeapInit[NUM_BLOCK_SIZES] = 
{
	{NUM_BLOCK_1, ELEMENT_SIZE_1},
	{NUM_BLOCK_2, ELEMENT_SIZE_2},
	{NUM_BLOCK_3, ELEMENT_SIZE_3},
	{NUM_BLOCK_4, ELEMENT_SIZE_4},
	{NUM_BLOCK_5, ELEMENT_SIZE_5}
};
//---------------------------------------------------------------------------
static HEAP_STRUCT stHeap;

//---------------------------------------------------------------------------
/*!
	Initializes each heap block header  and zeros out the memory pool used
	for dynamic memory allocation.
	\fn void FunkOS_Heap::Init(void)
*/
void FunkOS_Heap::Init(void)
{
	USHORT i, j;
	USHORT usBlockOffset = 0;
	USHORT usHeapBlock = 0;
	
	// For each size group in the table
	for (i = 0; i < NUM_BLOCK_SIZES; i++)
	{
		// For each element in the size group
		for (j = 0; j < astHeapInit[i].usNumBlocks; j++)
		{
			// Set up the heap variables
			stHeap.astHeapElements[usHeapBlock].usSize = astHeapInit[i].usBlockSize;
			stHeap.astHeapElements[usHeapBlock].usOffset = usBlockOffset; 
			stHeap.astHeapElements[usHeapBlock].bInUse = FALSE;
			
			// Update the block offset
			usBlockOffset += astHeapInit[i].usBlockSize;
			usHeapBlock++;
		}
	}
	
	// Clear the heap
	for (i = 0; i < HEAP_SIZE_BYTES; i++)
	{
		stHeap.aucPool[i] = 0;
	}
}
//---------------------------------------------------------------------------
/*!
	Allocates a single block of memory from the heap based on the required 
	size (if one is available).
	\fn UCHAR *FunkOS_Heap::Alloc(USHORT usSize_)
	\param usSize_ - the size of the block to allocate
	\return pointer to the block of newly-allocated memory, or NULL on failure
*/
UCHAR *FunkOS_Heap::Alloc(USHORT usSize_)
{
	USHORT i;
	BOOL bDone = FALSE;
	UCHAR *pucReturn = (UCHAR*)NULL;
	
	// Try to allocate the smallest possible block
	for(i = 0; i < HEAP_NUM_ELEMENTS; i++)
	{
		if (stHeap.astHeapElements[i].usSize > usSize_)
		{
			CS_ENTER();
			if (stHeap.astHeapElements[i].bInUse == FALSE)
			{
				// Found smallest suitable block that isn't in use.
				
				// Set the in-use flag to true
				stHeap.astHeapElements[i].bInUse = TRUE;
				// Set the return value
				pucReturn = &(stHeap.aucPool[stHeap.astHeapElements[i].usOffset]);
				// Found something, bail.
				bDone = TRUE;
			}
			CS_EXIT();

			// break on success.
			if (bDone)
			{
				break;
			}
		}
	}
	
	// Return the address of the 
	return pucReturn;
}

//---------------------------------------------------------------------------
/*!
	De-allocates a previously allocated block of memory based on the address
	given.
	\fn void FunkOS_Heap::Free(void *pvData_)
	\param pvData_ - pointer to the previously-allocated memory block
*/
void FunkOS_Heap::Free(void *pvData_)
{
	USHORT i;
	BOOL bDone = FALSE;
	UCHAR *pucData = (UCHAR*)NULL;
	UCHAR *pucTemp = (UCHAR*)NULL;
	
	// Set the UCHAR* version of the input void parameter
	pucData = (UCHAR*)pvData_;
	
	// Go through the whole list trying to find the matching element allocated
	for (i = 0; i < HEAP_NUM_ELEMENTS; i++)
	{
		// Only looking at allocated elements
		if (stHeap.astHeapElements[i].bInUse == TRUE)
		{
			CS_ENTER();
			// Get the address for the element's array
			pucTemp = &(stHeap.aucPool[stHeap.astHeapElements[i].usOffset]);
			// Element addresses match, this is the match
			if (pucTemp == pucData)
			{
				// Deallocate the element and exit
				stHeap.astHeapElements[i].bInUse = FALSE;
				bDone = TRUE;
			}
			CS_EXIT();
			
			// Break on success
			if (bDone)
			{
				break;
			}
		}
	}
}

#endif //KERNEL_USE_HEAP
