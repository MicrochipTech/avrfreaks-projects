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
#include "kernel.h"
#include "thread.h"
#include "drvUART.h"
#include "slip.h"
#include "slip_mux.h"

#include "profile.h"
#include "ksemaphore.h"
#include "timerlist.h"
#include "kprofile.h"
#include "slipterm.h"
#include "message.h"


#include <avr/io.h>
#include <avr/sleep.h>

//---------------------------------------------------------------------------
// Need to have this to handle pure-virtual functions
extern "C" void __cxa_pure_virtual() {}

//---------------------------------------------------------------------------
// Global objects
static ProfileTimer clProfiler100m;		//!< Profiling timer
static ProfileTimer clProfiler10m;		//!< Profiling timer
static ProfileTimer clProfiler1m;		//!< Profiling timer
static ProfileTimer clProfiler1;
static ProfileTimer clProfiler2;
static ProfileTimer clProfiler3;

static Thread AppThread;			//!< Main "application" thread
static Thread IdleThread;			//!< Idle thread - runs when app can't

static ATMegaUART clUART;			//!< UART device driver object
static MessageQueue clMsgQ;			//!< Message Queue for timers

//---------------------------------------------------------------------------
#define STACK_SIZE_APP		(256)	//!< Size of the main app's stack
#define STACK_SIZE_IDLE		(256)	//!< Size of the idle thread stack

//---------------------------------------------------------------------------
#define UART_SIZE_RX		(32)	//!< UART RX Buffer size
#define UART_SIZE_TX		(64)	//!< UART TX Buffer size

//---------------------------------------------------------------------------
static unsigned char aucAppStack[STACK_SIZE_APP];
static unsigned char aucIdleStack[STACK_SIZE_IDLE];

//---------------------------------------------------------------------------
static unsigned char aucTxBuffer[UART_SIZE_TX];
static unsigned char aucRxBuffer[UART_SIZE_RX];

//---------------------------------------------------------------------------
static void AppEntry(void);
static void IdleEntry(void);

void MemSet( void *pvData_, unsigned char ucValue_, unsigned short usCount_ )
{
	unsigned char *pucData = (unsigned char*)pvData_;
	unsigned short i;
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

	DriverList::Add( &clUART );
	
	Kernel::Start();					//!< Start the kernel!
}

static Timer clTimer;
static Timer clTimer1;
static Timer clTimer2;
static Timer clTimer3;
static Semaphore clTimerSem;
//---------------------------------------------------------------------------
void TCallback( Thread *pclOwner_, void *data_ )
{
	clTimerSem.Post();
}

//---------------------------------------------------------------------------
static void TCallbackMulti1( Thread *pclOwner_, void *data_ )
{
	// Send a message to a queue
	Message *pclMsg = GlobalMessagePool::Pop();
	pclMsg->SetCode(0);
	clMsgQ.Send(pclMsg);
}
//---------------------------------------------------------------------------
static void TCallbackMulti2( Thread *pclOwner_, void *data_ )
{
	// Send a message to a queue
	Message *pclMsg = GlobalMessagePool::Pop();
	pclMsg->SetCode(1);
	clMsgQ.Send(pclMsg);
}
//---------------------------------------------------------------------------
static void TCallbackMulti3( Thread *pclOwner_, void *data_ )
{
	// Send a message to a queue
	Message *pclMsg = GlobalMessagePool::Pop();
	pclMsg->SetCode(2);
	clMsgQ.Send(pclMsg);
}

volatile K_ULONG aulDelta[3];
//---------------------------------------------------------------------------
static void MultiTimer(void)
{	
	K_ULONG i;
	
	// Use prime numbers for extra random interaction.
	clTimer1.Start(true, 7, TCallbackMulti1, NULL);
	clProfiler1.Start();
	clTimer2.Start(true, 13, TCallbackMulti2, NULL);
	clProfiler2.Start();
	clTimer3.Start(true, 19, TCallbackMulti3, NULL);
	clProfiler3.Start();
	
	for (i = 0; i < 100000; i++)
	{
		Message *pclMsg;
		pclMsg = clMsgQ.Receive();	
		switch  (pclMsg->GetCode()) 
		{
			case 0:
				clUART.Write(1, (K_UCHAR*)"1");
				clProfiler1.Stop();
				aulDelta[0] = clProfiler1.GetCurrent() * 8;
				clProfiler1.Start();
				if ((aulDelta[0] < (6 * 16000L)) || (aulDelta[0] > (8*16000L)))
				{
					clUART.Write(5, (K_UCHAR*)"FAIL\n");
				}
				break;
			case 1:
				clUART.Write(1, (K_UCHAR*)"2");
				clProfiler2.Stop();
				aulDelta[1] = clProfiler2.GetCurrent() * 8;
				clProfiler2.Start();
				if ((aulDelta[1] < (12 * 16000L)) || (aulDelta[1] > (14*16000L)))
				{
					clUART.Write(5, (K_UCHAR*)"FAIL\n");
				}
				break;
			case 2:
				clUART.Write(1, (K_UCHAR*)"3");
				clProfiler3.Stop();
				aulDelta[2] = clProfiler3.GetCurrent() * 8;
				clProfiler3.Start();
				if ((aulDelta[2] < (18 * 16000L)) || (aulDelta[2] > (20*16000L)))
				{
					clUART.Write(5, (K_UCHAR*)"FAIL\n");
				}
				break;
			default:
				break;	
		}
		GlobalMessagePool::Push(pclMsg);
	}
	
	clTimer1.Stop();
	clTimer2.Stop();
	clTimer3.Stop();
	clProfiler1.Stop();
	clProfiler2.Stop();
	clProfiler3.Stop();
}
//---------------------------------------------------------------------------

volatile K_ULONG ulDelta = 0;
void Sanity(void)
{
	K_ULONG i;
	
	// 1ms repeated counter
	clUART.Write(10, (K_UCHAR*)"Start 1ms\n");
	clTimer.Start(true, 1, TCallback, NULL);
	for (i = 0; i < 100000; i++)
	{
		clProfiler1m.Start();
		if ((i % 128) == 0)
		{
			clUART.Write(1, (K_UCHAR*)".");
		}
		clTimerSem.Pend();
		clProfiler1m.Stop();
		ulDelta = clProfiler1m.GetCurrent() * 8;
		if ((ulDelta < 12000) || (ulDelta > 18000))
		{
			// Write error...
			clUART.Write(5, (K_UCHAR*)"FAIL\n");
			break;
		}
	}
	clTimer.Stop();
	
	
	// 10ms repeated counter
	clUART.Write(11, (K_UCHAR*)"Start 10ms\n");
	clTimer.Start(true, 10, TCallback, NULL);
	for (i = 0; i < 10000; i++)
	{
		clProfiler10m.Start();
		clUART.Write(1, (K_UCHAR*)".");
		clTimerSem.Pend();
		clProfiler10m.Stop();
		ulDelta = clProfiler10m.GetCurrent() * 8;
		if ((ulDelta < 155000) || (ulDelta > 165000))
		{
			// Write error...
			
			break;
		}
	}
	clTimer.Stop();
	
	// 100ms repeated counter
	clUART.Write(12, (K_UCHAR*)"Start 100ms\n");
	clTimer.Start(true, 100, TCallback, NULL);
	for (i = 0; i < 1000; i++)
	{
		clProfiler100m.Start();
		clUART.Write(1, (K_UCHAR*)".");
		clTimerSem.Pend();
		clProfiler100m.Stop();
		ulDelta = clProfiler100m.GetCurrent() * 8;
		if ((ulDelta < 1595000) || (ulDelta > 1605000))
		{
			break;
			// Write error...
			clUART.Write(5, (K_UCHAR*)"FAIL\n");
			break;
		}
	}
	clTimer.Stop();
}
//---------------------------------------------------------------------------
void AppEntry(void)
{

	// Initialize the serial mux
	clUART.Init();

	SlipMux::Init("/dev/tty", UART_SIZE_RX, aucRxBuffer, UART_SIZE_TX, aucTxBuffer);
	
	clUART.Open();
	
	clProfiler1m.Init();
	clProfiler10m.Init();
	clProfiler100m.Init();
	clProfiler1.Init();
	clProfiler2.Init();
	clProfiler3.Init();
	
	clTimerSem.Init(0, 1);
		
	clMsgQ.Init();
		
	Profiler::Init();
	Profiler::Start();
	clUART.Write(6, (K_UCHAR*)"BEGIN\n");
	while(1)
	{
		Sanity();
		
		MultiTimer();
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

