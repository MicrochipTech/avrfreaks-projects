//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file plumber.c

	Description:
		Implements interprocess communication using plumbing.  A table is set up
		containing a list of tasks listening on a list of ports (1-65535).
		Multiple tasks can listen to the same ports, and in this fashion, 
	    data is copied from a single writer to multiple readers, each reader 
		getting its own copy of the data.  This module is completely threadsafe
		as it is implemented as a highest-priotiy task, where the temporary buffers 
		are locked by Mutex, and the message processing itself is initialized by 
		posting a Semaphore.  
*/
//---------------------------------------------------------------------------
#include "types.h"
#include "task.h"
#include "taskport.h"
#include "plumber.h"
#include "semaphore.h"
#include "mutex.h"
#include "kernelcfg.h"

//---------------------------------------------------------------------------
static PLUMB_TABLE_ENTRY astPlumbTable[PLUMB_TABLE_SIZE];
static PLUMB_STRUCT astPipes[PIPE_POOL];

//---------------------------------------------------------------------------
// Task data
static TASK_STRUCT tskPlumber;						//! Plumber task definition
static USHORT aucPlumberStack[PLUMBER_STACK_SIZE / 2];	//! Stack for the plumbing task

//---------------------------------------------------------------------------
// Local data
static volatile UCHAR aucPlumbBuffer[PIPE_SIZE];	//! intermediate buffer for packet
static volatile USHORT usPlumbLen;					//! current packet length 
static volatile USHORT usPlumbPort;					//! current packet port
static MUTEX_STRUCT stPlumbMutex;					//! Mutex for the plumber
static SEMAPHORE_STRUCT stPlumbSem;					//! Plumbing semaphore

//---------------------------------------------------------------------------
static void Plumber_Task(TASK_STRUCT *this_);

//---------------------------------------------------------------------------
/*!
	Create the plumber task and add it to the scheduler
	\fn void Plumber_CreateTask(void)
*/
void Plumber_CreateTask(void)
{
	int i, j;
	
	usPlumbLen = 0;
	
	// Initialize the mutex and semaphore.
	Mutex_Init(&stPlumbMutex);
	Semaphore_Init(&stPlumbSem);

	// Clear the plumb buffer
	for (i = 0; i < PIPE_SIZE; i++)
	{
		aucPlumbBuffer[i] = 0;
	}

	// initialize the plumbing table
	for (i = 0; i < PLUMB_TABLE_SIZE; i++)
	{
		astPlumbTable[i].pstTask = NULL;	
		astPlumbTable[i].usPortNum = 0;
		astPlumbTable[i].pstPlumb = NULL;
		astPlumbTable[i].pstSem = NULL;
		astPlumbTable[i].eState = PLUMB_UNINIT;
	}	
	
	// initialize the pipes
	for (i = 0; i < PIPE_POOL; i++)
	{
		astPipes[i].ucLen = 0;
		astPipes[i].bInUse = 0;
		astPipes[i].pstNext = NULL;
		
		for (j = 0; j < PIPE_SIZE; j++)
		{
			astPipes[i].aucData[j] = 0;
		}
	}

	// clear the plumb port
	usPlumbPort = 0;

	// Create the task struct.
	Task_CreateTask(&tskPlumber, 
					"Plumber",
					aucPlumberStack,
					PLUMBER_STACK_SIZE,
					TASK_PRI_MAX,
					(void*)Plumber_Task);	
	
	// Add to the task list
	Task_Add(&tskPlumber);

	Task_Start(&tskPlumber);
}

//---------------------------------------------------------------------------
/*!
	Initialize a pipe used by the plumber
	\fn PLUMB_RETURN Plumber_InitPipe(PLUMB_STRUCT *pstPlumb_)
	\param pstPlumb_ - the pipe to initialize
	\return PLUMB_RETURN - returns PLUMB_OK on success
*/
PLUMB_RETURN Plumber_InitPipe(PLUMB_STRUCT *pstPlumb_)
{
	int i;
	
	for (i = 0; i < PIPE_SIZE; i++)
	{
		pstPlumb_->aucData[i] = 0;
	}
	
	pstPlumb_->ucLen = 0;	
	pstPlumb_->bInUse = FALSE;		
	pstPlumb_->pstNext = NULL;
	
	return PLUMB_OK;
}
//---------------------------------------------------------------------------
/*!
	Add a pipe to the plumbing table
	\fn PLUMB_RETURN Plumber_AddToList(TASK_STRUCT *pstTask_, USHORT usPortNum_, SEMAPHORE_STRUCT *pstSem_)
	\param pstTask_ - the task to associate with the pipe entry
	\param usPortNum_ - the port number for the pipe
	\param pstSem_ - the semaphore to pend/post on
	\return PLUMB_RETURN - returns PLUMB_OK on success, PLUMB_FAIL_LIST_FAIL on list full
*/
PLUMB_RETURN Plumber_AddToList(TASK_STRUCT *pstTask_, USHORT usPortNum_, SEMAPHORE_STRUCT *pstSem_)
{
	int i;

	for (i = 0; i < PLUMB_TABLE_SIZE; i++)
	{
		// if there's a null entry in the plumbing table
		if (astPlumbTable[i].pstTask == NULL)
		{	
			// Do this in a critical section...
			CS_ENTER();
			
			// Add the entry to the list.
			astPlumbTable[i].usPortNum = usPortNum_;
			astPlumbTable[i].pstTask = pstTask_;
			astPlumbTable[i].pstSem = pstSem_;
			astPlumbTable[i].eState = PLUMB_CLOSED;
			
			// Exit critical section
			CS_EXIT();
			
			//return OK
			return PLUMB_OK;
		}
	}	
	
	// otherwise list is full.
	return PLUMB_FAIL_LIST_FULL;
}

//---------------------------------------------------------------------------
/*!
	Open the pipe specified
	\fn PLUMB_RETURN Plumber_OpenPort(USHORT usPort_)
	\param usPort_ - the port number to open
	\return PLUMB_RETURN - returns PLUMB_OK on success, PLUMB_FAIL_* on failure
*/
PLUMB_RETURN Plumber_OpenPort(USHORT usPort_)
{
	int i;
	PLUMB_RETURN eReturn = PLUMB_FAIL_PIPE_NULL;
	
	for (i = 0; i < PLUMB_TABLE_SIZE; i++)
	{
		CS_ENTER();
		if (astPlumbTable[i].usPortNum == usPort_)
		{
			astPlumbTable[i].eState = PLUMB_OPEN;
			eReturn = PLUMB_OK;
		}
		CS_EXIT();
	}
	// return ok
	return eReturn;
}

//---------------------------------------------------------------------------
/*!
	Close the pipe specified
	\fn PLUMB_RETURN Plumber_ClosePort(USHORT usPort_)
	\param usPort_ - the port to close
	\return PLUMB_RETURN - returns PLUMB_OK on success, PLUMB_FAIL_* on failure
*/
PLUMB_RETURN Plumber_ClosePort(USHORT usPort_)
{
	int i;
	PLUMB_RETURN eReturn = PLUMB_FAIL_PIPE_NULL;
	
	for (i = 0; i < PLUMB_TABLE_SIZE; i++)
	{
		CS_ENTER();		
		if (astPlumbTable[i].usPortNum ==  usPort_)
		{
			astPlumbTable[i].eState = PLUMB_CLOSED;
			eReturn = PLUMB_OK;
		}
		CS_EXIT();
	}
	// return ok
	return eReturn;
}

//---------------------------------------------------------------------------
/*!
	Write a packet to the specified port
	\fn PLUMB_RETURN Plumber_WriteToPort(UCHAR *pucData_, UCHAR ucLen_, USHORT usPort_)
	\param pucData_ - pointer to the data to send
	\param ucLen_ - the length of the data packet
	\param usPort_ - the port to send the data to
	\return PLUMB_RETURN - returns PLUMB_OK on success, PLUMB_FAIL_* on failure
*/
PLUMB_RETURN Plumber_WriteToPort(UCHAR *pucData_, UCHAR ucLen_, USHORT usPort_)
{
	int i;

	// Claim the mutex - there can be only one
	Mutex_Claim(&stPlumbMutex, TIME_FOREVER);

	//Copy the data in
	for (i = 0; i < ucLen_; i++)
	{
		aucPlumbBuffer[i] = pucData_[i];
	}
	
	// Set the source
	usPlumbLen = ucLen_;			
	usPlumbPort = usPort_;		

	// Set the semaphore - let the plumber run (highest priority)
	Semaphore_Post((void*)&stPlumbSem);
	
	return PLUMB_OK;
}

//---------------------------------------------------------------------------
/*!
	Deallocates a previously allocated pipe for future use.
	\fn void Plumber_FreePipe(PLUMB_STRUCT *pstPipe_)
	\param pstPipe_ - free a pipe that`s been allocated.
*/
void Plumber_FreePipe(PLUMB_STRUCT *pstPipe_)
{
	CS_ENTER();
	pstPipe_->pstNext = NULL;
	pstPipe_->bInUse = FALSE;
	CS_EXIT();
}

//---------------------------------------------------------------------------
/*!
	Searches through the plumbing table for a pipe on the task or port combination
	specified.  Returns a handle to the pipe if successful, else, NULL.
	\fn PLUMB_STRUCT* Plumber_ClaimPipe(TASK_STRUCT *pstTask_, USHORT usPort_)
	\param pstTask_ - handle to the task requesting the pipe
	\param usPort_ - port number of the pipe to request
	\return PLUMB_STRUCT* - the handle of the first valid pipe
*/
PLUMB_STRUCT* Plumber_ClaimPipe(TASK_STRUCT *pstTask_, USHORT usPort_)
{
	PLUMB_STRUCT *pstTemp; 
	PLUMB_STRUCT *pstReturn = NULL;
	int i;

	// Search through the list
	for (i = 0; i < PLUMB_TABLE_SIZE; i++)
	{
		if (astPlumbTable[i].pstTask == pstTask_)
		{
			if (astPlumbTable[i].usPortNum == usPort_)
			{
				if (astPlumbTable[i].pstPlumb == NULL)
				{
					return NULL; // error - no pipes.
				}
				else
				{
					// Only the removal happens in a CS
					CS_ENTER();

					// Get the head of the LL
					pstTemp = astPlumbTable[i].pstPlumb;

					// Disconnect the entry from the list.
					pstReturn = pstTemp;
					pstTemp = pstTemp->pstNext;
					pstReturn->pstNext = NULL;
					astPlumbTable[i].pstPlumb = pstTemp;

					CS_EXIT();

					// Break out, have the pointer
					break;
				}
			}
		}
	}

	// Return the pointer.
	return pstReturn;
}

//---------------------------------------------------------------------------
/*!
	Plumber service entry function
	\fn static void Plumber_Task(TASK_STRUCT *this_)
*/
static void Plumber_Task(TASK_STRUCT *this_)
{
	USHORT i,j,k;
	
	while(1)
	{
		// Pend until there's a request for the plumber.
		Semaphore_Pend(&stPlumbSem, TIME_FOREVER);

		// Go through the plumbing table
		for (i = 0; i < PLUMB_TABLE_SIZE; i++)
		{
			// Find entries at the same port	
			if (astPlumbTable[i].usPortNum == usPlumbPort)
			{
				// Open ports only
				if (astPlumbTable[i].eState == PLUMB_OPEN)
				{
					j = 0;
					// Allocate a pipe for the entry
					while (j < PIPE_POOL)
					{
						// Found an empty pipe
						if (astPipes[j].bInUse == FALSE)
						{
							// Claim the pipe
							astPipes[j].bInUse = TRUE;
						
							// If there aren't any pipes in the list.
							if (astPlumbTable[i].pstPlumb == NULL)
							{
								astPlumbTable[i].pstPlumb = &(astPipes[j]);
							}
							// If there's already a list, iterate through.
							else
							{
								PLUMB_STRUCT *pstTemp;
								pstTemp = astPlumbTable[i].pstPlumb;
								// Find the last entry in the list
								while (pstTemp->pstNext != NULL)
								{
									pstTemp = pstTemp->pstNext;
								}
								// Connect the pipe
								pstTemp->pstNext = &(astPipes[j]);
							}
						
							// copy the data in.
							for (k = 0; k < usPlumbLen; k++)
							{
								astPipes[j].aucData[k] = aucPlumbBuffer[k];
							}
							// Set the pipe length
							astPipes[j].ucLen = usPlumbLen;
								
							// Set the next pointer to null
							astPipes[j].pstNext = NULL;
							break;				
						}
						j++;
					}
					// Return an error if there's no room
					if (j == PIPE_POOL)
					{
					//??
					}
					else if (astPlumbTable[i].pstSem != NULL)
					{
						// Post the semaphore for the task waiting on data.
						Semaphore_Post(astPlumbTable[i].pstSem);
					}
				}
			}
		}

		// Release the writer, let something else have the resource
		Mutex_Release(&stPlumbMutex);	
	}
}
