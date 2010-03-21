//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  ptask.c
	
	Description:
		Cooperative mode scheduler.  Each task consists of a handler function
		which contains its own event queue.  

		Series of tasks are grouped together in task lists, which are associated
		to eachother for scheduling.  

		When the task loop is run, all pending events for tasks in the given
		task list are executed in priority order.
*/
//---------------------------------------------------------------------------
#include "types.h"
#include "ptask.h"

#include "protect.h"

//---------------------------------------------------------------------------
/*!
	Initialize a task structure by assigning all struct elements.
	\fn void PTask_InitTask(PTASK_STRUCT *pstTask_, PTASK_EVENT_STRUCT *pstEvent_, PTASK_FUNCTION pfHandler_, UCHAR ucPriority_)
	\param pstTask_ - pointer to the task structure being initialized
	\param pstEvent_ - Pointer to the event structure to associate with this task
	\param pfHandler_ - pointer to the task's handler function
	\param ucPriority_ - priority of the task (0-255)
*/
//---------------------------------------------------------------------------
void PTask_InitTask(PTASK_STRUCT *pstTask_, PTASK_EVENT_STRUCT *pstEvent_, PTASK_FUNCTION pfHandler_, UCHAR ucPriority_)
{
	pstTask_->ucPriority = ucPriority_;
	pstTask_->pstEvents = pstEvent_;
	pstTask_->pfHandler = pfHandler_;	
}
//---------------------------------------------------------------------------
/*!
	Initialize a task's event queue.	
	\fn void PTask_InitEventQueue(PTASK_EVENT_STRUCT *pstEvent_, USHORT *pusEventID_, USHORT *pusEventData_, UCHAR ucSize_)
	\param pstEvent_ - pointer to the event queue to initialize
	\param pusEventID_ - pointer to the event ID array to assign to the event queue
	\param pusEventData_ - pointer to the event Data array to assign to the event queue
	\param ucSize_ - number of elements in the event queue
*/
//---------------------------------------------------------------------------
void PTask_InitEventQueue(PTASK_EVENT_STRUCT *pstEvent_, USHORT *pusEventID_, USHORT *pusEventData_, UCHAR ucSize_)
{
	UCHAR i;
	
	// Assign the event queue buffers and size
	pstEvent_->pusEventID = pusEventID_;
	pstEvent_->pusEventData = pusEventData_;
	pstEvent_->ucNumEvents = ucSize_;

	// Reset the circular buffer index
	pstEvent_->ucHead = 0;
	pstEvent_->ucTail = 0;
	
	// Clear the event data
	for (i = 0; i < ucSize_; i++)
	{
		pstEvent_->pusEventID[i] = 0;
		pstEvent_->pusEventData[i] = 0;
	}
	
}
//---------------------------------------------------------------------------
/*!
	\fn void PTask_InitTaskList(PTASK_LIST_STRUCT *pstList_, PTASK_STRUCT **pstTask_, UCHAR ucSize_)
	\param pstList_ - pointer to the task list function 
	\param pstTask_ - pointer to the array of task pointers
	\param ucSize_ - the number of elements in the task pointer array
*/
//---------------------------------------------------------------------------
void PTask_InitTaskList(PTASK_LIST_STRUCT *pstList_, PTASK_STRUCT **pstTask_, UCHAR ucSize_)
{
	UCHAR i;
	pstList_->ucSize = ucSize_;
	pstList_->ucCurrentTask = 0;
	pstList_->pstTasks = pstTask_;
}

//---------------------------------------------------------------------------
/*!
	Add a cooperative task to a given task list.
	\fn BOOL PTask_AddTaskToList(PTASK_LIST_STRUCT *pstList_, PTASK_STRUCT *pstTask_)
	\param pstList_ - pointer to the task list  
	\param pstTask_ - pointer to the task pointer
*/
//---------------------------------------------------------------------------
BOOL PTask_AddTaskToList(PTASK_LIST_STRUCT *pstList_, PTASK_STRUCT *pstTask_)
{
	// find a spot for the task in the list
	UCHAR i;
	BOOL bFound = FALSE;
	for (i = 0; i < pstList_->ucSize; i++)
	{
		if (pstList_->pstTasks[i] == NULL)
		{
			pstList_->pstTasks[i] = pstTask_;
			bFound = TRUE;
			break;
		}
	}
	
	return bFound;
}

//---------------------------------------------------------------------------
/*!
	Execute all pending events for all tasks in the task list, in priority
	order (high to low)
	\fn void PTask_Loop(PTASK_LIST_STRUCT *pstList_)
	\param pstList_ - pointer to the task list function 
*/
//---------------------------------------------------------------------------
void PTask_Loop(PTASK_LIST_STRUCT *pstList_)
{
	UCHAR i,j,k;
	UCHAR ucMaxPri;
	USHORT usEventID;
	USHORT usEventData;
	BOOL bExecute;
	BOOL bEvent;

    bExecute = TRUE;
	while (bExecute == TRUE)
	{
		ucMaxPri = 0;
		bExecute = FALSE;

		// Loop 1 - find the priority to execute
		for (i = 0; i < pstList_->ucSize; i++)
		{
			if (pstList_->pstTasks[i] != NULL)
			{
				if (pstList_->pstTasks[i]->ucPriority > ucMaxPri)
				{
                    if (PTask_AreEventsPending(pstList_->pstTasks[i]))
                    {
    					ucMaxPri = pstList_->pstTasks[i]->ucPriority;
                    }
				}
			}
		}
		// Loop 2 - execute all events for tasks at this priority
		for (i = 0; i < pstList_->ucSize; i++)
		{
			if (pstList_->pstTasks[i] != NULL)
			{
				// Is it the right priority?
				if (pstList_->pstTasks[i]->ucPriority == ucMaxPri)
				{
					// Try to run all of the events for this task
					bEvent = TRUE;
					while (bEvent == TRUE)
					{
						// Check to see if there are more events for this task
						bEvent = PTask_ReadEvent(pstList_->pstTasks[i], &usEventID, &usEventData);
						
						// Yes, there is something to do here.
						if (bEvent == TRUE)
						{
							// Run the task with the event
							pstList_->pstTasks[i]->pfHandler(pstList_->pstTasks[i], usEventID, usEventData);
							// We found an event - we need to iterate through the loop again
							bExecute = TRUE;
						}
					}
				}
			}
		}
	}
	// No events found the last time through the list, we're done for now.
}

//---------------------------------------------------------------------------
/*!
	Adds an event to a task's event queue
	\fn BOOL PTask_QueueEvent(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_)
	\param pstTask_ - pointer to the task
	\param usEventID_ - The event ID to queue
	\param usEventData_ - The event data to queue
	\return TRUE on success, FALSE if the queue is full.
*/
//---------------------------------------------------------------------------
BOOL PTask_QueueEvent(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_)
{
	UCHAR ucNextHead;
	BOOL bReturn = FALSE;
	// Protected block
	CS_ENTER();

	// If there's only one element in the queue
	if (pstTask_->pstEvents->ucNumEvents == 1)
	{
		// No buffering, always overwrite.
		pstTask_->pstEvents->pusEventID[0] = usEventID_;
		pstTask_->pstEvents->pusEventData[0] = usEventData_;
		pstTask_->pstEvents->ucHead = 0;
		pstTask_->pstEvents->ucTail = 1;
		bReturn = TRUE;
	}
	else
	{
		// What's the next head index?
		ucNextHead = pstTask_->pstEvents->ucHead + 1;
		if (ucNextHead >= pstTask_->pstEvents->ucNumEvents)
		{
			ucNextHead = 0;
		}
		
		// If the next head index is the tail index, we're full, so bail
		if (ucNextHead == pstTask_->pstEvents->ucTail)
		{
			bReturn = FALSE;
		}
		else
		{
			// We have a free spot - return TRUE
			pstTask_->pstEvents->pusEventID[pstTask_->pstEvents->ucHead] = usEventID_;
			pstTask_->pstEvents->pusEventData[pstTask_->pstEvents->ucHead] = usEventData_;
			pstTask_->pstEvents->ucHead = ucNextHead;
			bReturn = TRUE;
		}
	}

	//Protected block
	CS_EXIT();

	return bReturn;
}

//---------------------------------------------------------------------------
/*!
	Checks to see if there are events in a given task's event queue.
	\fn BOOL PTask_AreEventsPending(PTASK_STRUCT *pstTask_)
	\param pstTask_ - pointer to the task
	\return TRUE on events pending, FALSE on event queue empty
*/
//---------------------------------------------------------------------------
BOOL PTask_AreEventsPending(PTASK_STRUCT *pstTask_)
{
    BOOL bFound = FALSE;

	if (pstTask_->pstEvents->ucNumEvents == 1)
	{
		// And the tail is non-zero, that means we have something...
		if (pstTask_->pstEvents->ucTail == 1)
		{
			bFound = TRUE;
		}
	}
	else
	{
		// If the head and tail are not equal, there is pending data
		if (pstTask_->pstEvents->ucHead != pstTask_->pstEvents->ucTail)
		{
			bFound = TRUE;
		}
    }

    return bFound;
}

//---------------------------------------------------------------------------
/*!
	Checks to see if there are events in a given task's event queue.
	\fn BOOL PTask_ReadEvent(PTASK_STRUCT *pstTask_, USHORT *pusEventID_, USHORT *pusEventData_)
	\param pstTask_ - pointer to the task
	\param pusEventID_ - array of event IDs
	\param pusEventData_ - array of event Data
	\return TRUE if an event was read, FALSE on queue empty
*/
//---------------------------------------------------------------------------
BOOL PTask_ReadEvent(PTASK_STRUCT *pstTask_, USHORT *pusEventID_, USHORT *pusEventData_)
{
	UCHAR ucNextTail;
	BOOL bFound = FALSE;
	
	// Protection block (events can be written from interrupt context)
	CS_ENTER();

	// If there's only one event
	if (pstTask_->pstEvents->ucNumEvents == 1)
	{
		// And the tail is non-zero, that means we have something...
		if (pstTask_->pstEvents->ucTail == 1)
		{
			// Copy out the event ID and Data
			*pusEventID_ = pstTask_->pstEvents->pusEventID[pstTask_->pstEvents->ucTail];
			*pusEventData_ = pstTask_->pstEvents->pusEventData[pstTask_->pstEvents->ucTail];
			bFound = TRUE;
			pstTask_->pstEvents->ucTail = 0;
		}
	}
	else
	{
		// If the head and tail are not equal, there is pending data
		if (pstTask_->pstEvents->ucHead != pstTask_->pstEvents->ucTail)
		{
			// Copy out the event ID and Data
			*pusEventID_ = pstTask_->pstEvents->pusEventID[pstTask_->pstEvents->ucTail];
			*pusEventData_ = pstTask_->pstEvents->pusEventData[pstTask_->pstEvents->ucTail];
			bFound = TRUE;
		}

		// We found an event, update the tail
		if (bFound == TRUE)
		{
			// Set the new tail of the linked list.
			ucNextTail = pstTask_->pstEvents->ucTail ;
			ucNextTail = ucNextTail + 1;
			if (ucNextTail >= pstTask_->pstEvents->ucNumEvents)
			{
				ucNextTail = 0;
			}
			pstTask_->pstEvents->ucTail = ucNextTail;
		}
	}

	// Protection block
	CS_EXIT();

	return bFound;
}

