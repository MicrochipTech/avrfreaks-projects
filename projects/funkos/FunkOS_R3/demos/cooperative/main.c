//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  main.c

	Description:
		Test harness for cooperative mode scheduler.
*/

//---------------------------------------------------------------------------

#include <stdio.h>

#include "ptask.h"
#include "types.h"

//---------------------------------------------------------------------------
// These structures are used to define the three PTASK event handlers
// In this demo app, we have 3 tasks, each with their own, different-sized
// event queues.
//
// This module can be used on any architecture, and does not require a functional
// FunkOS port.  However, the PTask module can be used with FunkOS quite easily,
// even allowing for multiple threads to each have their own independentg PTask lists.
// Keep in mind that no critical section protection is included in the PTask
// library, and so care should be taken to ensure that where events are set from
// different contexts, that the calls to "QueueEvent" should be handled in a
// critical section or other protected block.
//
// Many FunkOS features can be used with cooperative threading mode (including
// device drivers, timers, heap) without requiring the full kernel to be included.
// In this case, the source modules associated with those functions can be
// compiled into the proejct without requiring port code (For example, you
// can copy the "heap" and "driver" modules without needing any of the "taskXXX"
// modules in your projct).
//
// Please see the doxygen documentation for these modules for more information
// on how to use the ptask module
//---------------------------------------------------------------------------


static PTASK_EVENT_STRUCT   stTask1Events;
static PTASK_EVENT_STRUCT   stTask2Events;
static PTASK_EVENT_STRUCT   stTask3Events;

static USHORT usEvent1ID;
static USHORT usEvent1Data;

static USHORT ausEvent2ID[10];
static USHORT ausEvent2Data[10];

static USHORT ausEvent3ID[25];
static USHORT ausEvent3Data[25];

static PTASK_STRUCT stTask1;
static PTASK_STRUCT stTask2;
static PTASK_STRUCT stTask3;

static PTASK_LIST_STRUCT stTaskList;
static PTASK_STRUCT *apstTasks[3];

//---------------------------------------------------------------------------
USHORT Task1_Run(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_);
USHORT Task2_Run(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_);
USHORT Task3_Run(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_);

//---------------------------------------------------------------------------

#pragma argsused
int main(int argc, char* argv[])
{
    USHORT i, j;

    // initialize our 3 ptasks
    PTask_InitTask(&stTask1, &stTask1Events, (PTASK_FUNCTION)Task1_Run, 1);
    PTask_InitTask(&stTask2, &stTask2Events, (PTASK_FUNCTION)Task2_Run, 2);
    PTask_InitTask(&stTask3, &stTask3Events, (PTASK_FUNCTION)Task3_Run, 3);

    // Initialize each PTask's event queue
    PTask_InitEventQueue(&stTask1Events, &usEvent1ID, &usEvent1Data, 1);
    PTask_InitEventQueue(&stTask2Events, ausEvent2ID, ausEvent2Data, 10);
    PTask_InitEventQueue(&stTask3Events, ausEvent3ID, ausEvent3Data, 25);

    // Initialize the global task list
    PTask_InitTaskList(&stTaskList, apstTasks, 3);

    // Add each task to the list
    PTask_AddTaskToList(&stTaskList, &stTask1);
    PTask_AddTaskToList(&stTaskList, &stTask2);
    PTask_AddTaskToList(&stTaskList, &stTask3);

	i = 0;
	while (1)
	{
        for (j = 0; j < i; j++)
        {
            PTask_QueueEvent(&stTask1, i, j);
            PTask_QueueEvent(&stTask2, 2*i, j);
            PTask_QueueEvent(&stTask3, 3*i, j);
        }
		i++;
		if (i > 100)
		{
			i = 0;
		}

       PTask_Loop(&stTaskList);
    }

    return 0;
}
//---------------------------------------------------------------------------

USHORT Task1_Run(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_)
{
    printf("Task1: %d %d\n", usEventID_, usEventData_);
    return 0;
}

USHORT Task2_Run(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_)
{
    printf("Task2: %d %d\n", usEventID_, usEventData_);
    return 0;
}

USHORT Task3_Run(PTASK_STRUCT *pstTask_, USHORT usEventID_, USHORT usEventData_)
{
    printf("Task3: %d %d\n", usEventID_, usEventData_);
    return 0;
}
