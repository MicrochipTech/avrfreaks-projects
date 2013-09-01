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
#include "../ut_platform.h"
#include "eventflag.h"
#include "thread.h"
#include "memutil.h"
#include "driver.h"

//===========================================================================
// Local Defines
//===========================================================================
Thread clThread1;
Thread clThread2;

K_UCHAR aucThreadStack1[256];
K_UCHAR aucThreadStack2[160];

EventFlag clFlagGroup;
volatile K_UCHAR ucFlagCount = 0;
volatile K_UCHAR ucTimeoutCount = 0;

//---------------------------------------------------------------------------
void WaitOnFlag1Any(void *unused_)
{
    clFlagGroup.Wait(0x0001, EVENT_FLAG_ANY);
    ucFlagCount++;

    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
void WaitOnMultiAny(void *unused_)
{
    clFlagGroup.Wait(0x5555, EVENT_FLAG_ANY);
    ucFlagCount++;

    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
void WaitOnMultiAll(void *unused_)
{
    clFlagGroup.Wait(0x5555, EVENT_FLAG_ALL);
    ucFlagCount++;

    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
void WaitOnAny(void *mask_)
{
    K_USHORT usMask = *((K_USHORT*)mask_);
    while(1)
    {
        clFlagGroup.Wait(usMask, EVENT_FLAG_ANY);
        ucFlagCount++;
        clFlagGroup.Clear(usMask);
    }
}

//---------------------------------------------------------------------------
void WaitOnAll(void *mask_)
{
    K_USHORT usMask = *((K_USHORT*)mask_);
    while(1)
    {
        clFlagGroup.Wait(usMask, EVENT_FLAG_ALL);
        ucFlagCount++;
        clFlagGroup.Clear(usMask);
    }
}

//---------------------------------------------------------------------------
void TimedWait(void *time_)
{
    K_USHORT usRet;
    K_USHORT usTime = *((K_USHORT*)time_);
    usRet = clFlagGroup.Wait(0x0001, EVENT_FLAG_ALL, usTime);
    if (usRet == 0x0001)
    {
        ucFlagCount++;
    }
    else if (usRet == 0x0000)
    {
        ucTimeoutCount++;
    }
    clFlagGroup.Clear(0x0001);
    Scheduler::GetCurrentThread()->Exit();

}


//---------------------------------------------------------------------------
void TimedWaitAll(void *time_)
{
    K_USHORT usRet;
    K_USHORT usTime = *((K_USHORT*)time_);
    while(1)
    {
        usRet = clFlagGroup.Wait(0x0001, EVENT_FLAG_ALL, usTime);
        if (usRet == 0x0001)
        {
            ucFlagCount++;
        }
        else if (usRet == 0x0000)
        {
            ucTimeoutCount++;
        }
        clFlagGroup.Clear(0x0001);
    }
    Scheduler::GetCurrentThread()->Exit();
}

//===========================================================================
// Define Test Cases Here
//===========================================================================
TEST(ut_waitany)
{
    // Test - ensure that threads can block using the "waitany" mechanism, and
    // only wake up when bits from its pattern are encountered.
    K_USHORT i;
    K_USHORT usMask = 0x8000;

    clFlagGroup.Init();
    ucFlagCount = 0;

    clThread1.Init(aucThreadStack1, 160, 7, WaitOnAny, (void*)(&usMask));
    clThread1.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    usMask = 0x0001;
    while(usMask)
    {
        clFlagGroup.Set(usMask);
        Thread::Sleep(100);

        if (usMask != 0x8000)
        {
            EXPECT_EQUALS(ucFlagCount, 0);
        }
        else
        {
            EXPECT_EQUALS(ucFlagCount, 1);
        }

        usMask <<= 1;
    }
    clThread1.Exit();

    // Okay, that was a single bit-flag test.  Now let's try using a multi-bit flag
    // and verify that any matching pattern will cause a wakeup

    clFlagGroup.Init();
    ucFlagCount = 0;
    usMask = 0xAAAA;

    clThread1.Init(aucThreadStack1, 160, 7, WaitOnAny, (void*)(&usMask));
    clThread1.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    // Test point - the flag set should kick the test thread on even-indexed
    // counters indexes.
    for (i = 0; i < 16; i++)
    {
        K_UCHAR ucLastFlagCount = ucFlagCount;

        clFlagGroup.Set((K_USHORT)(1 << i));

        Thread::Sleep(100);
        if ((i & 1) == 0)
        {
            EXPECT_EQUALS(ucFlagCount, ucLastFlagCount);
        }
        else
        {
            EXPECT_EQUALS(ucFlagCount, ucLastFlagCount+1);
        }
    }

    clThread1.Exit();
}
TEST_END


//===========================================================================
TEST(ut_waitall)
{
    // Test - ensure that threads can block using the "waitany" mechanism, and
    // only wake up when bits from its pattern are encountered.
    K_USHORT i;
    K_USHORT usMask = 0x8000;

    clFlagGroup.Init();
    ucFlagCount = 0;

    clThread1.Init(aucThreadStack1, 160, 7, WaitOnAll, (void*)(&usMask));
    clThread1.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    usMask = 0x0001;
    while(usMask)
    {
        clFlagGroup.Set(usMask);
        Thread::Sleep(100);

        if (usMask != 0x8000)
        {
            EXPECT_EQUALS(ucFlagCount, 0);
        }
        else
        {
            EXPECT_EQUALS(ucFlagCount, 1);
        }

        usMask <<= 1;
    }
    clThread1.Exit();

    // Okay, that was a single bit-flag test.  Now let's try using a multi-bit flag
    // and verify that any matching pattern will cause a wakeup

    clFlagGroup.Init();
    ucFlagCount = 0;
    usMask = 0xAAAA;

    clThread1.Init(aucThreadStack1, 160, 7, WaitOnAll, (void*)(&usMask));
    clThread1.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    // Test point - the flag set should kick the test thread on even-indexed
    // counters indexes.
    for (i = 0; i < 16; i++)
    {
        K_UCHAR ucLastFlagCount = ucFlagCount;

        clFlagGroup.Set((K_USHORT)(1 << i));

        Thread::Sleep(100);
        if (i != 15)
        {
            EXPECT_EQUALS(ucFlagCount, ucLastFlagCount);
        }
        else
        {
            EXPECT_EQUALS(ucFlagCount, ucLastFlagCount+1);
        }
    }

    clThread1.Exit();
}
TEST_END

//---------------------------------------------------------------------------
TEST(ut_flag_multiwait)
{

    // Test - ensure that all forms of event-flag unblocking work when there
    // are multiple threads blocked on the same flag.

    clFlagGroup.Init();

    // Test point - 2 threads blocking on an event flag, bit 1.  Wait on these
    // threads until this thread sets bit 0x0001.  When that bit is set, the
    // threads should wake up, incrementing the "ucFlagCount" variable.
    ucFlagCount = 0;
    clFlagGroup.Clear(0xFFFF);

    clThread1.Init(aucThreadStack1, 160, 7, WaitOnFlag1Any, 0);
    clThread2.Init(aucThreadStack2, 160, 7, WaitOnFlag1Any, 0);

    clThread1.Start();
    clThread2.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0x0001);

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 2);

    ucFlagCount = 0;
    clFlagGroup.Clear(0xFFFF);

    // Test point - 2 threads blocking on an event flag, bits 0x5555.  Block
    // on these threads, and verify that only bits in the pattern will cause
    // the threads to awaken
    clThread1.Init(aucThreadStack1, 160, 7, WaitOnMultiAny, 0);
    clThread2.Init(aucThreadStack2, 160, 7, WaitOnMultiAny, 0);

    clThread1.Start();
    clThread2.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0xAAAA);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0x5555);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 2);

    ucFlagCount = 0;
    clFlagGroup.Clear(0xFFFF);


    clThread1.Init(aucThreadStack1, 160, 7, WaitOnMultiAny, 0);
    clThread2.Init(aucThreadStack2, 160, 7, WaitOnMultiAny, 0);

    clThread1.Start();
    clThread2.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0xA000);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0x0005);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 2);
    // Test point - same thing as above, but with the "ALL" flags set.

    ucFlagCount = 0;
    clFlagGroup.Clear(0xFFFF);

    clThread1.Init(aucThreadStack1, 160, 7, WaitOnMultiAll, 0);
    clThread2.Init(aucThreadStack2, 160, 7, WaitOnMultiAll, 0);

    clThread1.Start();
    clThread2.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0xAAAA);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0x5555);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 2);


    ucFlagCount = 0;
    clFlagGroup.Clear(0xFFFF);

    // "All" mode - each flag must be set in order to ensure that the threads
    // unblock.
    clThread1.Init(aucThreadStack1, 160, 7, WaitOnMultiAll, 0);
    clThread2.Init(aucThreadStack2, 160, 7, WaitOnMultiAll, 0);

    clThread1.Start();
    clThread2.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0xAAAA);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0x5500);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0x0055);
    Thread::Sleep(100);

    EXPECT_EQUALS(ucFlagCount, 2);
}
TEST_END

//===========================================================================
TEST(ut_timedwait)
{
    K_USHORT usInterval;

    // Test point - verify positive test case (no timeout, no premature
    // unblocking)
    ucTimeoutCount = 0;
    ucFlagCount = 0;
    usInterval = 200;

    clFlagGroup.Init();

    clThread1.Init(aucThreadStack1, 160, 7, TimedWait, (void*)&usInterval);
    clThread1.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucTimeoutCount, 0);
    EXPECT_EQUALS(ucFlagCount, 0);

    clFlagGroup.Set(0x0001);

    EXPECT_EQUALS(ucTimeoutCount, 0);
    EXPECT_EQUALS(ucFlagCount, 1);


    // Test point - verify negative test case (timeouts), followed by a
    // positive test result.
    ucTimeoutCount = 0;
    ucFlagCount = 0;
    usInterval = 200;

    clFlagGroup.Init();
    clFlagGroup.Clear(0xFFFF);

    clThread1.Init(aucThreadStack1, 160, 7, TimedWait, (void*)&usInterval);
    clThread1.Start();

    Thread::Sleep(100);

    EXPECT_EQUALS(ucTimeoutCount, 0);
    EXPECT_EQUALS(ucFlagCount, 0);

    Thread::Sleep(200);

    EXPECT_EQUALS(ucTimeoutCount, 1);
    EXPECT_EQUALS(ucFlagCount, 0);

    // Test point - verify negative test case (timeouts), followed by a
    // positive test result.
    ucTimeoutCount = 0;
    ucFlagCount = 0;
    usInterval = 200;

    clFlagGroup.Init();
    clFlagGroup.Clear(0xFFFF);

    clThread1.Init(aucThreadStack1, 160, 7, TimedWaitAll, (void*)&usInterval);
    clThread1.Start();

    Thread::Sleep(210);

    EXPECT_EQUALS(ucTimeoutCount, 1);
    EXPECT_EQUALS(ucFlagCount, 0);

    Thread::Sleep(210);

    EXPECT_EQUALS(ucTimeoutCount, 2);
    EXPECT_EQUALS(ucFlagCount, 0);

    Thread::Sleep(210);

    EXPECT_EQUALS(ucTimeoutCount, 3);
    EXPECT_EQUALS(ucFlagCount, 0);

    Thread::Sleep(210);

    EXPECT_EQUALS(ucTimeoutCount, 4);
    EXPECT_EQUALS(ucFlagCount, 0);

    Thread::Sleep(210);

    EXPECT_EQUALS(ucTimeoutCount, 5);
    EXPECT_EQUALS(ucFlagCount, 0);

    Thread::Sleep(80);
    clFlagGroup.Set(0x0001);

    EXPECT_EQUALS(ucTimeoutCount, 5);
    EXPECT_EQUALS(ucFlagCount, 1);

    Thread::Sleep(80);
    clFlagGroup.Set(0x0001);

    EXPECT_EQUALS(ucTimeoutCount, 5);
    EXPECT_EQUALS(ucFlagCount, 2);

    Thread::Sleep(80);
    clFlagGroup.Set(0x0001);

    EXPECT_EQUALS(ucTimeoutCount, 5);
    EXPECT_EQUALS(ucFlagCount, 3);

    Thread::Sleep(80);
    clFlagGroup.Set(0x0001);

    EXPECT_EQUALS(ucTimeoutCount, 5);
    EXPECT_EQUALS(ucFlagCount, 4);

    Thread::Sleep(80);
    clFlagGroup.Set(0x0001);

    EXPECT_EQUALS(ucTimeoutCount, 5);
    EXPECT_EQUALS(ucFlagCount, 5);


}
TEST_END

//===========================================================================
// Test Whitelist Goes Here
//===========================================================================
TEST_CASE_START
  //TEST_CASE(ut_waitany),
  //TEST_CASE(ut_waitall),
  //TEST_CASE(ut_flag_multiwait),
  TEST_CASE(ut_timedwait),
TEST_CASE_END
