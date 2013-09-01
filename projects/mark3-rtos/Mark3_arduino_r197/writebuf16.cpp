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
	\file writebuf16.cpp
	
	\brief 16 bit circular buffer implementation with callbacks.
*/

#include "kerneltypes.h"
#include "writebuf16.h"
#include "kernel_debug.h"
#include "threadport.h"

#if KERNEL_USE_DEBUG && !KERNEL_AWARE_SIMULATION

//---------------------------------------------------------------------------
void WriteBuffer16::WriteData( K_USHORT *pusBuf_, K_USHORT usLen_ )
{
	K_USHORT *apusBuf[1];
	K_USHORT ausLen[1];
	
	apusBuf[0] = pusBuf_;
	ausLen[0] = usLen_;
	
	WriteVector( apusBuf, ausLen, 1 );
}

//---------------------------------------------------------------------------
void WriteBuffer16::WriteVector( K_USHORT **ppusBuf_, K_USHORT *pusLen_, K_UCHAR ucCount_ )
{
	K_USHORT usTempHead;	
	K_UCHAR i;
	K_UCHAR j;
	K_USHORT usTotalLen = 0;
	bool bCallback = false;
	bool bRollover = false;
	// Update the head pointer synchronously, using a small 
	// critical section in order to provide thread safety without
	// compromising on responsiveness by adding lots of extra
	// interrupt latency.
	
	CS_ENTER();
	
	usTempHead = m_usHead;
	{		
		for (i = 0; i < ucCount_; i++)
		{
			usTotalLen += pusLen_[i];
		}
		m_usHead = (usTempHead + usTotalLen) % m_usSize;
	}	
	CS_EXIT();
	
	// Call the callback if we cross the 50% mark or rollover 
	if (m_usHead < usTempHead)
	{
		if (m_pfCallback)
		{
			bCallback = true;
			bRollover = true;
		}
	}
	else if ((usTempHead < (m_usSize >> 1)) && (m_usHead >= (m_usSize >> 1)))
	{
		// Only trigger the callback if it's non-null
		if (m_pfCallback)
		{
			bCallback = true;
		}
	}	
	
	// Are we going to roll-over?
	for (j = 0; j < ucCount_; j++)
	{
		K_USHORT usSegmentLength = pusLen_[j];
		if (usSegmentLength + usTempHead >= m_usSize)
		{	
			// We need to two-part this... First part: before the rollover
			K_USHORT usTempLen;
			K_USHORT *pusTmp = &m_pusData[ usTempHead ];
			K_USHORT *pusSrc = ppusBuf_[j];
			usTempLen = m_usSize - usTempHead;
			for (i = 0; i < usTempLen; i++)
			{
				*pusTmp++ = *pusSrc++;
			}

			// Second part: after the rollover
			usTempLen = usSegmentLength - usTempLen;
			pusTmp = m_pusData;
			for (i = 0; i < usTempLen; i++)
			{		
				*pusTmp++ = *pusSrc++;
			}
		}
		else
		{	
			// No rollover - do the copy all at once.
			K_USHORT *pusSrc = ppusBuf_[j];
			K_USHORT *pusTmp = &m_pusData[ usTempHead ];
			for (K_USHORT i = 0; i < usSegmentLength; i++)
			{		
				*pusTmp++ = *pusSrc++;
			}
		}
	}


	// Call the callback if necessary
	if (bCallback)
	{
		if (bRollover)
		{
			// Rollover - process the back-half of the buffer
			m_pfCallback( &m_pusData[ m_usSize >> 1], m_usSize >> 1 );
		} 
		else 
		{
			// 50% point - process the front-half of the buffer
			m_pfCallback( m_pusData, m_usSize >> 1);
		}
	}
}

#endif
