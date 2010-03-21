//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  ptask.h
	
	Description:
		Cooperative mode header files.
*/
//--------------------------------------------------------------------------- 

#ifndef __PTASK_H__
#define __PTASK_H__

#include "types.h"

// Pseudotask module for cooperative tasks
typedef void (*PTASK_FUNCTION)(void *this_, USHORT usEventID_, USHORT usEventData_);	//!< Function pointer used...

//---------------------------------------------------------------------------
typedef struct
{
	USHORT *pusEventID;					//!< Array of event IDs
	USHORT *pusEventData;				//!< Array of event data
	
	UCHAR 	ucNumEvents;				//!< Size of the event buffer
	
	UCHAR	ucHead;						//!< Head index in the event buffer (circular)
	UCHAR	ucTail;						//!< Tail index of the event buffer (circular)
} PTASK_EVENT_STRUCT;

//---------------------------------------------------------------------------
typedef struct 
{
	UCHAR 				ucPriority;		//!< Priority of the ptask

	PTASK_EVENT_STRUCT 	*pstEvents;		//!< Pointer to the task's event queue
	PTASK_FUNCTION 		pfHandler;		//!< Pointer to the task function
} PTASK_STRUCT;

//---------------------------------------------------------------------------
typedef struct
{
	UCHAR ucSize;						//!< Number of tasks	
	UCHAR ucCurrentTask;				//!< Task index currently being executing
	PTASK_STRUCT **pstTasks;			//!< Pointer to array of tasks
} PTASK_LIST_STRUCT;
//---------------------------------------------------------------------------

// Initialize an individual ptask
void PTask_InitTask(PTASK_STRUCT *pstTask_, PTASK_EVENT_STRUCT *pstEvent_, PTASK_FUNCTION pfTask_, UCHAR ucPriority_);

// Initialize a ptask's event queue
void PTask_InitEventQueue(PTASK_EVENT_STRUCT *pstEvent_, USHORT *pusEventID_, USHORT *pusEventData_, UCHAR ucSize_);

// Initialize a task list with an array of tasks
void PTask_InitTaskList(PTASK_LIST_STRUCT *pstList_, PTASK_STRUCT **pstTask_, UCHAR ucSize_);

// Add a task to the specified task list 
BOOL PTask_AddTaskToList(PTASK_LIST_STRUCT *pstList_, PTASK_STRUCT *pstTask_);

// Function that executes the tasks based on the presence of events in the loop
void PTask_Loop(PTASK_LIST_STRUCT *pstList_);

//---------------------------------------------------------------------------
// Queue event data
BOOL PTask_QueueEvent(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_);

// Get event data from the queue
BOOL PTask_ReadEvent(PTASK_STRUCT *pstTask_, USHORT *pusEventID_, USHORT *pusEventData_);

#endif
