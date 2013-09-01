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
	\file tracebuffer.cpp
	\brief Kernel trace buffer class definition	
*/

#include "kerneltypes.h"
#include "tracebuffer.h"
#include "mark3cfg.h"
#include "writebuf16.h"
#include "kernel_debug.h"

#if KERNEL_USE_DEBUG && !KERNEL_AWARE_SIMULATION
//---------------------------------------------------------------------------
WriteBuffer16 TraceBuffer::m_clBuffer;
volatile K_USHORT TraceBuffer::m_usIndex;
K_USHORT TraceBuffer::m_ausBuffer[ (TRACE_BUFFER_SIZE/sizeof(K_USHORT)) ];

//---------------------------------------------------------------------------
void TraceBuffer::Init()
{
	m_clBuffer.SetBuffers(m_ausBuffer, TRACE_BUFFER_SIZE/sizeof(K_USHORT));
	m_usIndex = 0;
}

//---------------------------------------------------------------------------
K_USHORT TraceBuffer::Increment()
{
	return m_usIndex++;	
}

//---------------------------------------------------------------------------
void TraceBuffer::Write( K_USHORT *pusData_, K_USHORT usSize_ )
{
	// Pipe the data directly to the circular buffer
	m_clBuffer.WriteData(pusData_, usSize_);
}

#endif

