//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//--------------------------------------------------------------------------- 
/*!
	\file:  message.h
	
	Description:
		Threadsafe message queue
*/

#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include "semaphore.h"
#include "types.h"
#include "kernelcfg.h"

#if KERNEL_USE_MESSAGE

/*!
	Event item - contains an ID and an data field
	\struct EVENT_STRUCT 
*/
typedef struct
{
	USHORT usEventID;		//!< Event ID for the message 
	USHORT usEventData;		//!< Event Data for the message
} EVENT_STRUCT;

//---------------------------------------------------------------------------
/*!
	Message queue - contains a circular buffer of event data specified by an
	owner task.
	\struct MESSAGE_STRUCT
*/
typedef struct
{
	EVENT_STRUCT *pstEvents;		//!< Pointer to the message/event queue
	USHORT usSize;					//!< Size of the message/event queue
	USHORT usHead;					//!< Head index of the queue
	USHORT usTail;					//!< Tail index of the queue
	BOOL bEmpty;					//!< Buffer-empty flag
	BOOL bFull;						//!< Buffer-full flag
	SEMAPHORE_STRUCT stSem;			//!< Semaphore associated with the message/event queue
} MESSAGE_STRUCT;
//---------------------------------------------------------------------------
void Message_Init(MESSAGE_STRUCT *pstMessage_, EVENT_STRUCT *pstEvent_, USHORT usSize_);
BOOL Message_Send(MESSAGE_STRUCT *pstMessage_, USHORT usEventID_, USHORT usEventData_);
BOOL Message_Read(MESSAGE_STRUCT *pstMessage_, USHORT *pusEventID_, USHORT *pusEventData_);

#endif //KERNEL_USE_MESSAGE

#endif

