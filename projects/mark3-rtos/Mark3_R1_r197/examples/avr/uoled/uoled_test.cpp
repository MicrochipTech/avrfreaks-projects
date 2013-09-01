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

    \file   uoled_test.cpp

    \brief  Test harness for 4D systems displays.

*/

//---------------------------------------------------------------------------

#include "kerneltypes.h"
#include "kernel.h"
#include "thread.h"
#include "drvUART.h"
#include "graphics_uoled.h"
#include "draw.h"
#include "colorspace.h"
#include "font.h"
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

static GraphicsUOLED clUOLED;       //!< 4D systems UOLED driver object

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
    clUOLED.SetName("/dev/display");     //!< Add the display driver
    clUOLED.Init();

	DriverList::Add( &clUART );
	
	Kernel::Start();					//!< Start the kernel!
}

//---------------------------------------------------------------------------
void AppEntry(void)
{    	
    Driver *my_uart = DriverList::FindByPath("/dev/tty");

    my_uart->Control( CMD_SET_BUFFERS, aucRxBuffer, UART_SIZE_RX,
                                         aucTxBuffer, UART_SIZE_TX);
    my_uart->Open();

    clUOLED.SetDriver(my_uart);

    // Dummy code - just write hello world forever...
    clUOLED.ClearScreen();
    Thread::Sleep(10);
    clUOLED.ClearScreen();
    Thread::Sleep(10);

    while(1)
	{
        DrawText_t stText;

        stText.usLeft = 8;
        stText.usTop = 8;
        stText.uColor = COLOR_RED;
        //stText.pstFont = &fntTahoma_10_False_False_False_;
        stText.pcString = "Hello World!";
        clUOLED.Text(&stText);

        stText.usLeft = 8;
        stText.usTop = 16;
        stText.uColor = COLOR_RED;
        //stText.pstFont = &fntTahoma_10_False_False_False_;
        stText.pcString = "Goodbye World!";
        clUOLED.Text(&stText);
        Thread::Sleep(10);
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

