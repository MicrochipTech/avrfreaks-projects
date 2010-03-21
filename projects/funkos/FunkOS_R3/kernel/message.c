//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//--------------------------------------------------------------------------- 
/*!
	\file:  message.c
	
	Description:
		Threadsafe message queue implementation.
*/
//---------------------------------------------------------------------------
#include "taskport.h"
#include "semaphore.h"
#include "types.h"
#include "message.h"
#include "kernelcfg.h"

#if KERNEL_USE_MESSAGE
//---------------------------------------------------------------------------
/*!
	Initialize the message queue structure, resetting all flags and buffer 
	sizes.
	\fn void Message_Init(MESSAGE_STRUCT *pstMessage_, EVENT_STRUCT *pstEvent_, USHORT usSize_)
	\param pstMessage_ - the message queue structure
	\param pstEvent_ - the event queue array specified by the owner task
	\param usSize_ - the size of the event queue array
*/
void Message_Init(MESSAGE_STRUCT *pstMessage_, EVENT_STRUCT *pstEvent_, USHORT usSize_)
{
	pstMessage_->pstEvents = pstEvent_;
	pstMessage_->usSize = 0;
	pstMessage_->usHead = 0;
	pstMessage_->usTail = 0;
	pstMessage_->bEmpty = TRUE;
	pstMessage_->bFull = FALSE;
	Semaphore_Init(&(pstMessage_->stSem));
}
/*!
	Writes an event to a specified message queue
	\fn BOOL Message_Send(MESSAGE_STRUCT *pstMessage_, USHORT usEventID_, USHORT usEventData_)
	\param pstMessage_ - the event queue write to
	\param usEventID_ - the pointer that the event ID is written out to
	\param usEventData_ - the pointer that the event data is written out to
	\return TRUE if a message was successfully queued, FALSE if the buffer is full.
*/
//---------------------------------------------------------------------------
BOOL Message_Send(MESSAGE_STRUCT *pstMessage_, USHORT usEventID_, USHORT usEventData_)
{
	USHORT usIndex;
	EVENT_STRUCT *pstEvent;
	
	// If the event queue is full, bail
	if (pstMessage_->bFull)
	{
		return FALSE;
	}
	
	//--[Critical Section!!]-------------------------------------------------
	CS_ENTER();
	
	// Get the pointer of the head index
	usIndex = pstMessage_->usHead;
	pstEvent = &(pstMessage_->pstEvents[usIndex]);
	
	// Copy the data in
	pstEvent->usEventID = usEventID_;
	pstEvent->usEventData = usEventData_;
	
	// update the circular buffer
	pstMessage_->usHead = pstMessage_->usSize + 1;
	if (pstMessage_->usHead >= pstMessage_->usSize)
	{
		pstMessage_->usHead = 0;
	}
	
	// Check for buffer-full conditions
	if (pstMessage_->usHead == pstMessage_->usTail)
	{
		pstMessage_->bFull = TRUE;
	}
	else
	{
		pstMessage_->bFull = FALSE;
	}
	pstMessage_->bEmpty = FALSE;
	
	//--[Critical Section!!]-------------------------------------------------	
	CS_EXIT();
	
	Semaphore_Post(&(pstMessage_->stSem));
	return TRUE;
}
//---------------------------------------------------------------------------
/*!
	Read an event from the front of the given event queue 
	\fn BOOL Message_Read(MESSAGE_STRUCT *pstMessage_, USHORT *pusEventID_, USHORT *pusEventData_)
	\param pstMessage_ - the event queue to read from
	\param pusEventID_ - the pointer that the event ID is written out to
	\param pusEventData_ - the pointer that the event data is written out to
	\return BOOL - TRUE if an event was read, otherwise FALSE if the buffer is full
*/
BOOL Message_Read(MESSAGE_STRUCT *pstMessage_, USHORT *pusEventID_, USHORT *pusEventData_)
{
	USHORT usIndex;
	EVENT_STRUCT *pstEvent;
	
	if (pstMessage_->bEmpty)
	{
		*pusEventID_ = 0;
		*pusEventData_ = 0;
		return FALSE;
	}
	
	CS_ENTER();
	
	// Read the data out
	usIndex = pstMessage_->usTail;
	pstEvent = &(pstMessage_->pstEvents[usIndex]);
	
	*pusEventData_ = pstEvent->usEventData;
	*pusEventID_ = pstEvent->usEventID;
	
	// update the circular buffer
	pstMessage_->usTail = pstMessage_->usSize + 1;
	if (pstMessage_->usTail >= pstMessage_->usSize)
	{
		pstMessage_->usTail = 0;
	}

	// Check for buffer-empty conditions
	if (pstMessage_->usHead == pstMessage_->usTail)
	{
		pstMessage_->bEmpty = TRUE;
	}
	else
	{
		pstMessage_->bEmpty = FALSE;
	}
	
	// Clear the buffer-full flag
	pstMessage_->bFull = FALSE;
	
	CS_EXIT();
	return TRUE;
}
//---------------------------------------------------------------------------
#endif

