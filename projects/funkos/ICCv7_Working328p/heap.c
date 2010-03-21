//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//--------------------------------------------------------------------------- 
/*!
	\file:  heap.c
	
	Description:
		Static block-size heap implementation
*/

#include "types.h"
#include "taskport.h"
#include "heap.h"

#if KERNEL_USE_HEAP
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
	\fn void Heap_Init(void)
*/
void Heap_Init(void)
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
	\fn UCHAR *Heap_Alloc(USHORT usSize_)
	\param usSize_ - the size of the block to allocate
	\return pointer to the block of newly-allocated memory, or NULL on failure
*/
UCHAR *Heap_Alloc(USHORT usSize_)
{
	USHORT i;
	BOOL bDone = FALSE;
	UCHAR *pucReturn = NULL;
	
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
	\fn void Heap_Free(void *pvData_)
	\param pvData_ - pointer to the previously-allocated memory block
*/
void Heap_Free(void *pvData_)
{
	USHORT i;
	BOOL bDone = FALSE;
	UCHAR *pucData = NULL;
	UCHAR *pucTemp = NULL;
	
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
