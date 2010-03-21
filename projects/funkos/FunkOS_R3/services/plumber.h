//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file plumber.h

	Description:
		The plumber is implemented as the highest-priority task in the system.  Whenever
		inter-process communication is required, the current task will yield, giving 
		control to the plumber, who will send the message to the tasks listening on 
		that port.

		If the task hasn't processed its last packet by the time the new packet arrives,
		then the packet buffer is overwritten at this point.  Further packet buffering
		could be added, but keeping the buffering to a minimum is one of the main
		goals of this method of IPC.
*/
//---------------------------------------------------------------------------
#ifndef __PLUMBER_H_
#define __PLUMBER_H_

#include "types.h"
#include "task.h"
#include "semaphore.h"
#include "kernelcfg.h"

//---------------------------------------------------------------------------
#define PIPE_SIZE			((USHORT) 16)		//!< Data size per pipe
#define PLUMBER_STACK_SIZE	((USHORT) 160)		//!< Size of the plumber task stack
#define PIPE_POOL			((USHORT) 4)		//!< Number of pipes in the pool
#define PLUMB_TABLE_SIZE	((USHORT) 4)		//!< # of entries in the plumbing table 

//---------------------------------------------------------------------------
typedef enum
{
	PLUMB_UNINIT = 0,
	PLUMB_CLOSED,
	PLUMB_OPEN
} PLUMB_STATE;

//---------------------------------------------------------------------------
typedef enum
{
	PLUMB_OK = 0,
	PLUMB_FAIL_LIST_FULL,
	PLUMB_FAIL_PIPE_FULL,
	PLUMB_FAIL_PIPE_OPEN,
	PLUMB_FAIL_PIPE_NULL,
	PLUMB_FAIL_PIPE_CLOSED,
	PLUMB_FAIL_UNINIT,
} PLUMB_RETURN;

//---------------------------------------------------------------------------
/*!
	This struct defines a pipe used for holding a packet
	\struct PLUMB_STRUCT
*/
typedef struct _Plumb_Struct
{
	UCHAR 		aucData[PIPE_SIZE];			//!< Size of the pipe (in bytes)
	UCHAR 		ucLen;						//!< Length of the data in the pipe.
	BOOL 		bInUse;						//!< Is the pipe in use
	
	struct 		_Plumb_Struct *pstNext;		//!< The next pipe in the list
} PLUMB_STRUCT;

//---------------------------------------------------------------------------
/*!
	This struct defines an entry in the plumbing table
	\struct PLUMB_TABLE_ENTRY
*/
typedef struct
{	
	TASK_STRUCT 		*pstTask;		//!< Destination task/service
	PLUMB_STRUCT		*pstPlumb;		//!< Pointer to the first output pipe
	SEMAPHORE_STRUCT	*pstSem;		//!< Semaphore for the pipe
	
	PLUMB_STATE			eState;			//!< State variable
	USHORT 				usPortNum;		//!< Port Number
} PLUMB_TABLE_ENTRY;

//---------------------------------------------------------------------------
// Allow dynamic addition of 
PLUMB_RETURN Plumber_InitPipe(PLUMB_STRUCT *pstPlumb_);
void Plumber_CreateTask(void);
//---------------------------------------------------------------------------
// These all takes place in a critical section.
PLUMB_RETURN Plumber_AddToList(TASK_STRUCT *pstTask_, USHORT usPortNum_, SEMAPHORE_STRUCT *psSem_);

//---------------------------------------------------------------------------
PLUMB_RETURN Plumber_WriteToPort(UCHAR *pucData_, UCHAR ucLen_, USHORT usPort_);

//---------------------------------------------------------------------------
PLUMB_RETURN Plumber_ClosePort(USHORT usPort_);
PLUMB_RETURN Plumber_OpenPort(USHORT usPort_);

//---------------------------------------------------------------------------
PLUMB_STRUCT* Plumber_ClaimPipe(TASK_STRUCT *pstTask_, USHORT usPort_);
void Plumber_FreePipe(PLUMB_STRUCT *pstPipe_);

#endif //__PLUMBER_H_
