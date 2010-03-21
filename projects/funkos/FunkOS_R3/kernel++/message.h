//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
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
	\class FunkOS_Message
*/
class FunkOS_Message
{

public:
	void Init(EVENT_STRUCT *pstEvent_, USHORT usSize_);
	BOOL Send(USHORT usEventID_, USHORT usEventData_);
	BOOL Read(USHORT *pusEventID_, USHORT *pusEventData_);
	
	FunkOS_Semaphore m_clSem;			//!< Semaphore associated with the message/event queue
private:
	EVENT_STRUCT *m_pstEvents;			//!< Pointer to the message/event queue
	USHORT m_usSize;					//!< Size of the message/event queue
	USHORT m_usHead;					//!< Head index of the queue
	USHORT m_usTail;					//!< Tail index of the queue
	BOOL m_bEmpty;						//!< Buffer-empty flag
	BOOL m_bFull;						//!< Buffer-full flag	
};

#endif //KERNEL_USE_MESSAGE

#endif

