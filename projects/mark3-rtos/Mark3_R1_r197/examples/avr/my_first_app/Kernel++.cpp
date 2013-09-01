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

    \file   kernel++.cpp 

    \brief  Test harness

*/

//---------------------------------------------------------------------------

#include "kerneltypes.h"
#include "kernel.h"
#include "thread.h"
#include "drvUART.h"
#include "slip.h"
#include "slip_mux.h"
#include "fixed_heap.h"

#include <avr/io.h>
#include <avr/sleep.h>

//---------------------------------------------------------------------------
// Need to have this to handle pure-virtual functions
extern "C" void __cxa_pure_virtual() {}

//---------------------------------------------------------------------------
// Global objects
static Thread AppThread;			//!< Main "application" thread
static Thread IdleThread;			//!< Idle thread - runs when app can't

static ATMegaUART clUART;			//!< UART device driver object

//---------------------------------------------------------------------------
#define STACK_SIZE_APP		(256)	//!< Size of the main app's stack
#define STACK_SIZE_IDLE		(256)	//!< Size of the idle thread stack

//---------------------------------------------------------------------------
#define UART_SIZE_RX		(32)	//!< UART RX Buffer size
#define UART_SIZE_TX		(64)	//!< UART TX Buffer size

//---------------------------------------------------------------------------
static K_UCHAR aucAppStack[STACK_SIZE_APP];
static K_UCHAR aucIdleStack[STACK_SIZE_IDLE];

//---------------------------------------------------------------------------
static K_UCHAR aucTxBuffer[UART_SIZE_TX];
static K_UCHAR aucRxBuffer[UART_SIZE_RX];

//---------------------------------------------------------------------------
static void AppEntry(void);
static void IdleEntry(void);

static K_UCHAR aucHeap[400];
static HeapConfig aclHeaps[4];
static FixedHeap clHeap;

void MemSet( void *pvData_, K_UCHAR ucValue_, K_USHORT usCount_ )
{
	K_UCHAR *pucData = (K_UCHAR*)pvData_;
	K_USHORT i;
	for (i = 0; i < usCount_; i++)
	{
		pucData[i]= ucValue_;
	}
	
}

//---------------------------------------------------------------------------
int main(void)
{		
	Kernel::Init();						//!< MUST be before other kernel ops
	
	AppThread.Init(	aucAppStack,		//!< Pointer to the stack
					STACK_SIZE_APP,		//!< Size of the stack
					1,					//!< Thread priority
					(ThreadEntry_t)AppEntry,	//!< Entry function
					(void*)&AppThread );//!< Entry function argument

	IdleThread.Init( aucIdleStack,		//!< Pointer to the stack
					 STACK_SIZE_IDLE,	//!< Size of the stack
					 0,					//!< Thread priority		
					 (ThreadEntry_t)IdleEntry,	//!< Entry function
					 NULL );			//!< Entry function argument
	
	AppThread.Start();					//!< Schedule the threads
	IdleThread.Start();
	
	clUART.SetName("/dev/tty");			//!< Add the serial driver
	clUART.Init();	
	
	DriverList::Add( &clUART );
	
	Kernel::Start();					//!< Start the kernel!
}

//---------------------------------------------------------------------------
void AppEntry(void)
{
	Slip *pclSlip;
	// Initialize the serial mux
	SlipMux::Init("/dev/tty", UART_SIZE_RX, aucRxBuffer, UART_SIZE_TX, aucTxBuffer);
	clUART.Open();

	aclHeaps[0].m_usBlockCount = 8;
	aclHeaps[0].m_usBlockSize = 8; //14 * 8 = 112
	
	aclHeaps[1].m_usBlockCount = 4;
	aclHeaps[1].m_usBlockSize = 16; //22 * 4 = 88
	
	aclHeaps[2].m_usBlockCount = 2;
	aclHeaps[2].m_usBlockSize = 32; //38 * 2 = 76

	aclHeaps[3].m_usBlockCount = 0;
	aclHeaps[3].m_usBlockSize = 0;
	
	clHeap.Create( (void*)aucHeap, aclHeaps );
	
	// Dummy code - just write hello world forever...
	while(1)
	{
		void *pvData1 = clHeap.Alloc(4);
		MemSet(pvData1, 0xAA, 4);
		clHeap.Free(pvData1);
	
		pvData1 = clHeap.Alloc(8);
		MemSet(pvData1, 0xBB, 8);
		clHeap.Free(pvData1);

		pvData1 = clHeap.Alloc(12);
		MemSet(pvData1, 0xCC, 12);
		clHeap.Free(pvData1);

		pvData1 = clHeap.Alloc(24);
		MemSet(pvData1, 0xDD, 24);
		FixedHeap::Free(pvData1);	// Can free an object without knowing its allocator.

		pvData1 = clHeap.Alloc(32);
		MemSet(pvData1, 0xEE, 32);
		FixedHeap::Free(pvData1);

		pclSlip = SlipMux::GetSlip();
		pclSlip->WriteData( SLIP_CHANNEL_TERMINAL, (K_CHAR*)"Hello World!", 12 );
		//Thread::Sleep(5);
		//Scheduler::GetCurrentThread()->Exit();
	}
}

//---------------------------------------------------------------------------
void IdleEntry(void)
{
	while(1)
	{
		// LPM code;
		set_sleep_mode(SLEEP_MODE_IDLE);
		cli();
		sleep_enable();
		sei();
		sleep_cpu();
		sleep_disable();
		sei();
	}
}

