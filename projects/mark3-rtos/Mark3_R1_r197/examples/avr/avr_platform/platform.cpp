#include "kerneltypes.h"
#include "kernel.h"
#include "thread.h"
#include "fixed_heap.h"
#include "slip.h"
#include "slip_mux.h"
#include "drvUART.h"
#include "message.h"
#include "ksemaphore.h"
#include "driver.h"

//---------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

//---------------------------------------------------------------------------
// Need to have this to handle pure-virtual functions
extern "C" void __cxa_pure_virtual() {}

//---------------------------------------------------------------------------
#define IDLE_STACK_SIZE				(192)	
#define MAIN_STACK_SIZE				(256)

//---------------------------------------------------------------------------
#define UART_TX_SIZE				(64)
#define UART_RX_SIZE				(24)

//---------------------------------------------------------------------------
static Thread clIdleThread;
static Thread clMainThread;
static K_UCHAR aucIdleStack[IDLE_STACK_SIZE];
static K_UCHAR aucMainStack[MAIN_STACK_SIZE];

//---------------------------------------------------------------------------
static ATMegaUART clUART;
static K_UCHAR aucRxData[UART_RX_SIZE];
static K_UCHAR aucTxData[UART_TX_SIZE];

//---------------------------------------------------------------------------
MessageQueue g_clMainThreadQueue;

//---------------------------------------------------------------------------
static void MainThread( void* unused );
static void IdleThread( void* unused );

//---------------------------------------------------------------------------
int main(void)
{
	Kernel::Init();
	
	clIdleThread.Init(aucIdleStack, IDLE_STACK_SIZE, 0, (ThreadEntry_t)IdleThread, NULL);
	clMainThread.Init(aucMainStack, MAIN_STACK_SIZE, 1, (ThreadEntry_t)MainThread, NULL);
	
	clIdleThread.Start();
	clMainThread.Start();
	
	clUART.SetName("/dev/tty");
	clUART.Init();
	DriverList::Add(&clUART);
	
	Kernel::Start();
}

#if 0
//!! These are example slip callback handlers, which must be setup explicitly
//---------------------------------------------------------------------------
static void Slip_TerminalCallback( Driver *pclDriver_, K_UCHAR ucChannel_, K_UCHAR *pucData_, K_USHORT usLen_ )
{
	// Receive terminal/shell command
}

//---------------------------------------------------------------------------
static void Slip_NVMCallback( Driver *pclDriver_, K_UCHAR ucChannel_, K_UCHAR *pucData_, K_USHORT usLen_ )
{
	// Command to write to a location in NVM - essentially a "device program" command.
}

//---------------------------------------------------------------------------
static void Slip_ResetCallback( Driver *pclDriver_, K_UCHAR ucChannel_, K_UCHAR *pucData_, K_USHORT usLen_ )
{
	// Reset callback - enable watchdog, and disable interrupts
	cli();
	wdt_enable(WDTO_15MS);
	while(1) { /* Spin until reset */ };
}
#endif

//---------------------------------------------------------------------------
static void MainThread( void* unused )
{
	// Initialize the message queue attached to the main thread...
	g_clMainThreadQueue.Init();

	// Initialize the slip multiplexer on the UART, and let it initialize all of 
	// buffers according to what we've allocated.
	SlipMux::Init("/dev/tty", UART_RX_SIZE, aucRxData, UART_TX_SIZE, aucTxData);
	
	// Set the slip multiplexer to send notifications to our message queue
	// whenever a new message comes in.
	SlipMux::SetQueue(&g_clMainThreadQueue);

	while(1)
	{		
		// Block here until our thread gets a message.
		Message *pclMsg = g_clMainThreadQueue.Receive();

		// Read the message, and figure out what to do based on the 
		// message code.
		switch (pclMsg->GetCode())
		{
			case SLIP_RX_MESSAGE_ID:
				// Acknowledge the buffered slip message, and run the
				// appropriate handler.
				SlipMux::MessageReceive();
				break;
			default:
				break;
		}
		
		// Push the message back to the global pool once finished processing
		GlobalMessagePool::Push(pclMsg);
	}
}

//---------------------------------------------------------------------------
static void IdleThread( void* unused )
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
