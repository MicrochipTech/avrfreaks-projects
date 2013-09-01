/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
	\file writebuf16.h

	\brief Thread-safe circular buffer implementation with 16-bit elements
*/

#ifndef __WRITEBUF16_H__
#define __WRITEBUF16_H__

#include "kerneltypes.h"
#include "mark3cfg.h"

#if KERNEL_USE_DEBUG && !KERNEL_AWARE_SIMULATION

/*!
	Function pointer type used to define a callback handler for when
	the circular buffer reaches 50% capacity, 
*/
typedef void (*WriteBufferCallback)( K_USHORT *pusData_, K_USHORT usSize_ );

/*!
	This class is used to provide a general-purpose, fully thread-safe circular
	buffer implementation which can be used for creating tracebuffers, data 
	logging queues, transaction queues, etc.  We use it for implementing
	a debug print journal.
*/
class WriteBuffer16
{
public:
	/*!
		\fn void SetBuffers( K_USHORT *pusData_, K_USHORT usSize_ )
		
		Assign the data to be used as storage for this circular buffer
		
		\param pusData_ Pointer to the array of data to be managed 
				as a circular buffer by this object.

		\param usSize_ Size of the buffer in 16-bit elements
	*/
	void SetBuffers( K_USHORT *pusData_, K_USHORT usSize_ )
	{ 
		m_pusData = pusData_; 
		m_usSize = usSize_; 
		m_usHead = 0;
		m_usTail = 0;
	}
	
	/*!
		\fn void SetCallback( WriteBufferCallback pfCallback_ ) 
		
		Set the callback function to be called when the buffer hits 
		50% of its capacity, and again when the buffer rolls over 
		completely.
		
		\param pfCallback_ Function pointer to call whenever the 
				buffer has reached 50% capacity, or has rolled over
				completely.
	*/
	void SetCallback( WriteBufferCallback pfCallback_ ) 
		{ m_pfCallback = pfCallback_; }
	
	/*!
		\fn void WriteData( K_USHORT *pusBuf_, K_USHORT usLen_ )
		
		Write an array of values to the circular buffer
		
		\param pusBuf_ Source data array to write to the circular buffer		
		\param usLen_ Length of the source data array in 16-bit elements
	*/
	void WriteData( K_USHORT *pusBuf_, K_USHORT usLen_ );
	
	/*!
		\fn void WriteVector( K_USHORT **ppusBuf_, K_USHORT *pusLen_, K_UCHAR ucCount_)
		
		Write a multi-part vector to the circular buffer
		
		\param ppusBuf_ Pointer to the array of source data pointers
		\param pusLen_ Array of buffer lengths
		\param ucCount_ Number of source-data arrays to write to the buffer
	*/
	void WriteVector( K_USHORT **ppusBuf_, K_USHORT *pusLen_, K_UCHAR ucCount_);

private:
	K_USHORT *m_pusData;				//!< Pointer to the circular buffer data
	
	volatile K_USHORT m_usSize;			//!< Size of the buffer
	volatile K_USHORT m_usHead;			//!< Current head element (where data is written)
	volatile K_USHORT m_usTail;			//!< Current tail element (where data is read)
	
	WriteBufferCallback m_pfCallback;	//!< Buffer callback function
};
#endif

#endif
