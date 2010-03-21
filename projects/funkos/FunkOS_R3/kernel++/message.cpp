//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//--------------------------------------------------------------------------- 
/*!
	\file:  message.cpp
	
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
	\fn void FunkOS_Message::Init(EVENT_STRUCT *pstEvent_, USHORT usSize_)
	\param pstEvent_ - the event queue array specified by the owner task
	\param usSize_ - the size of the event queue array
*/
void FunkOS_Message::Init(EVENT_STRUCT *pstEvent_, USHORT usSize_)
{
	m_pstEvents = pstEvent_;
	m_usSize = 0;
	m_usHead = 0;
	m_usTail = 0;
	m_bEmpty = TRUE;
	m_bFull = FALSE;
}

//---------------------------------------------------------------------------
/*!
	Writes an event to a specified message queue
	\fn BOOL FunkOS_Message::Send(USHORT usEventID_, USHORT usEventData_)
	\param usEventID_ - the pointer that the event ID is written out to
	\param usEventData_ - the pointer that the event data is written out to
	\return TRUE if a message was successfully queued, FALSE if the buffer is full.
*/
BOOL FunkOS_Message::Send(USHORT usEventID_, USHORT usEventData_)
{
	USHORT usIndex;
	EVENT_STRUCT *pstEvent;
	
	// If the event queue is full, bail
	if (m_bFull)
	{
		return FALSE;
	}
	
	//--[Critical Section!!]-------------------------------------------------
	CS_ENTER();
	
	// Get the pointer of the head index
	usIndex = m_usHead;
	pstEvent = &(m_pstEvents[usIndex]);
	
	// Copy the data in
	pstEvent->usEventID = usEventID_;
	pstEvent->usEventData = usEventData_;
	
	// update the circular buffer
	m_usHead = m_usSize + 1;
	if (m_usHead >= m_usSize)
	{
		m_usHead = 0;
	}
	
	// Check for buffer-full conditions
	if (m_usHead == m_usTail)
	{
		m_bFull = TRUE;
	}
	else
	{
		m_bFull = FALSE;
	}
	m_bEmpty = FALSE;
	
	//--[Critical Section!!]-------------------------------------------------	
	CS_EXIT();
	
	m_clSem.Post();
	return TRUE;
}
//---------------------------------------------------------------------------
/*!
	Read an event from the front of the given event queue 
	\fn BOOL FunkOS_Message::Read(USHORT *pusEventID_, USHORT *pusEventData_)
	\param pusEventID_ - the pointer that the event ID is written out to
	\param pusEventData_ - the pointer that the event data is written out to
	\return BOOL - TRUE if an event was read, otherwise FALSE if the buffer is full
*/
BOOL FunkOS_Message::Read(USHORT *pusEventID_, USHORT *pusEventData_)
{
	USHORT usIndex;
	EVENT_STRUCT *pstEvent;
	
	if (m_bEmpty)
	{
		*pusEventID_ = 0;
		*pusEventData_ = 0;
		return FALSE;
	}
	
	CS_ENTER();
	
	// Read the data out
	usIndex = m_usTail;
	pstEvent = &(m_pstEvents[usIndex]);
	
	*pusEventData_ = pstEvent->usEventData;
	*pusEventID_ = pstEvent->usEventID;
	
	// update the circular buffer
	m_usTail = m_usSize + 1;
	if (m_usTail >= m_usSize)
	{
		m_usTail = 0;
	}

	// Check for buffer-empty conditions
	if (m_usHead == m_usTail)
	{
		m_bEmpty = TRUE;
	}
	else
	{
		m_bEmpty = FALSE;
	}
	
	// Clear the buffer-full flag
	m_bFull = FALSE;
	
	CS_EXIT();
	return TRUE;
}
//---------------------------------------------------------------------------
#endif

