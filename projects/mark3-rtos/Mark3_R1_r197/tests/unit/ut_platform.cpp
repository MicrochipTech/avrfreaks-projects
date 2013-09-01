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

//---------------------------------------------------------------------------

#include "kerneltypes.h"
#include "kernel.h"
#include "thread.h"
#include "drvUART.h"
#include "unit_test.h"
#include "ut_platform.h"
#include "memutil.h"

#if defined(AVR)
#include <avr/io.h>
#include <avr/sleep.h>
#endif

extern "C"
{
void __cxa_pure_virtual(void) {}
}

//---------------------------------------------------------------------------
// Global objects
static Thread AppThread;			//!< Main "application" thread
static Thread IdleThread;			//!< Idle thread - runs when app can't

static ATMegaUART clUART;			//!< UART device driver object

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
void MyUnitTest::PrintTestResult()
{
    K_CHAR acTemp[6];
    int iLen;

    PrintString("Test ");
    PrintString(GetName());
    PrintString(": ");
    iLen = MemUtil::StringLength(GetName());
    if (iLen >= 32)
    {
        iLen = 32;
    }
    for (int i = 0; i < 32 - iLen; i++)
    {
        PrintString(".");
    }
    if (GetPassed() == GetTotal())
    {
        PrintString("(PASS)[");
    }
    else
    {
        PrintString("(FAIL)[");
    }
    MemUtil::DecimalToString(GetPassed(), (K_CHAR*)acTemp);
    PrintString((const K_CHAR*)acTemp);
    PrintString("/");
    MemUtil::DecimalToString(GetTotal(), (K_CHAR*)acTemp);
    PrintString((const K_CHAR*)acTemp);
    PrintString("]\n");
}

typedef void (*FuncPtr)(void);
//---------------------------------------------------------------------------
void run_tests()
{
    MyTestCase *pstTestCase;
    pstTestCase = astTestCases;

    while (pstTestCase->pclTestCase)
    {
        pstTestCase->pfTestFunc();
        pstTestCase++;
    }
    PrintString("--DONE--\n");
    Thread::Sleep(100);

    FuncPtr pfReset = 0;
    pfReset();
}

//---------------------------------------------------------------------------
void init_tests()
{
    MyTestCase *pstTestCase;
    pstTestCase = astTestCases;

    while (pstTestCase->pclTestCase)
    {
        pstTestCase->pclTestCase->SetName(pstTestCase->szName);
        pstTestCase++;
    }
}

//---------------------------------------------------------------------------
void PrintString(const K_CHAR *szStr_)
{
    K_CHAR *szTemp = (K_CHAR*)szStr_;
    while (*szTemp)
    {
        while( 1 != clUART.Write( 1, (K_UCHAR*)szTemp ) ) { /* Do nothing */ }
        szTemp++;
    }
}

//---------------------------------------------------------------------------
void AppEntry(void)
{
    {
        Driver *my_uart = DriverList::FindByPath("/dev/tty");

        my_uart->Control( CMD_SET_BUFFERS, aucRxBuffer, UART_SIZE_RX,
                                             aucTxBuffer, UART_SIZE_TX);
        my_uart->Open();

        init_tests();
    }

    while(1)
    {
        run_tests();
    }
}

//---------------------------------------------------------------------------
void IdleEntry(void)
{
    while(1)
    {
#if defined(AVR)
        // LPM code;
        set_sleep_mode(SLEEP_MODE_IDLE);
        cli();
        sleep_enable();
        sei();
        sleep_cpu();
        sleep_disable();
        sei();
#endif
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
