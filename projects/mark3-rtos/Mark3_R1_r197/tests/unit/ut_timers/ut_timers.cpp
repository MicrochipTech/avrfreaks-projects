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
#include "timerlist.h"
#include "thread.h"
#include "kprofile.h"
#include "profile.h"
#include "kernel.h"
#include "ksemaphore.h"
#include "kerneltimer.h"
#include "driver.h"
#include "memutil.h"

//===========================================================================
// Local Defines
//===========================================================================
static Timer clTimer1;
static Timer clTimer2;
static Timer clTimer3;
static Semaphore clTimerSem;
static ProfileTimer clProfileTimer;
static ProfileTimer clProfileTimer2;
static ProfileTimer clProfileTimer3;
static K_ULONG ulTimeVal;
static K_ULONG ulTempTime;
static volatile K_ULONG ulCallbackCount = 0;

static void TimerCallback( Thread *pclOwner_, void *pvVal_ )
{
    clTimerSem.Post();
    ulCallbackCount++;
}

//===========================================================================
// Define Test Cases Here
//===========================================================================
TEST(ut_timer_tolerance)
{
    Profiler::Start();
    clTimerSem.Init(0, 1);

    // Test point - 1ms timer should take at least 1ms
    clProfileTimer.Init();
    clProfileTimer.Start();
    clTimer1.Start( false, 1, TimerCallback, 0 );
    clTimerSem.Pend();
    clProfileTimer.Stop();

    ulTimeVal = clProfileTimer.GetCurrent() * CLOCK_DIVIDE;
    ulTempTime = SYSTEM_FREQ / 1000;
    EXPECT_GT(ulTimeVal, ulTempTime);

    // Test point - 1ms timer should be no more than 3ms
    ulTempTime *= 3;
    EXPECT_LT(ulTimeVal, ulTempTime);

    // Test point - 10ms timer should take at least 10ms
    clProfileTimer.Init();
    clProfileTimer.Start();
    clTimer1.Start( false, 10, TimerCallback, 0 );
    clTimerSem.Pend();
    clProfileTimer.Stop();

    ulTimeVal = clProfileTimer.GetCurrent() * CLOCK_DIVIDE;
    ulTempTime = SYSTEM_FREQ / 100;

    EXPECT_GT(ulTimeVal, ulTempTime);

    // Test point - 10ms timer should be no more than 12ms
    ulTempTime += 2* (SYSTEM_FREQ / 1000);
    EXPECT_LT(ulTimeVal, ulTempTime);

    // Test point - 100ms timer should take at least 100ms
    clProfileTimer.Init();
    clProfileTimer.Start();
    clTimer1.Start( false, 100, TimerCallback, 0 );
    clTimerSem.Pend();
    clProfileTimer.Stop();

    ulTimeVal = clProfileTimer.GetCurrent() * CLOCK_DIVIDE;
    ulTempTime = SYSTEM_FREQ / 10;

    EXPECT_GT(ulTimeVal, ulTempTime);

    // Test point - 100ms timer should be no more than 102ms
    ulTempTime += 2 * (SYSTEM_FREQ / 1000);
    EXPECT_LT(ulTimeVal, ulTempTime);

    // Test point - 1000ms timer should take at least 100ms
    clProfileTimer.Init();
    clProfileTimer.Start();
    clTimer1.Start( false, 1000, TimerCallback, 0 );
    clTimerSem.Pend();
    clProfileTimer.Stop();

    ulTimeVal = clProfileTimer.GetCurrent() * CLOCK_DIVIDE;
    ulTempTime = SYSTEM_FREQ;

    EXPECT_GT(ulTimeVal, ulTempTime);

    // Test point - 1000ms timer should be no more than 1002ms
    ulTempTime += 2* (SYSTEM_FREQ / 1000);
    EXPECT_LT(ulTimeVal, ulTempTime);

    Profiler::Stop();
}
TEST_END


TEST(ut_timer_longrun)
{
    // Profiling timer is not really designed for long profiling
    // operations (1.2 seconds is about as high as we get, since it's
    // so high resolution).  So, use sleeps and multiple iterations
    // w/averaging in order to verify.

    K_ULONG ulSleepCount = 0;
    Profiler::Start();
    clTimerSem.Init(0, 1);

    // Test point - long running timer accuracy; 10-second timer
    // expires after 10 seconds.
    clProfileTimer.Init();
    clTimer1.Start( false, 10000, TimerCallback, 0 );
    ulCallbackCount = 0;

    while (!ulCallbackCount)
    {
        clProfileTimer.Start();
        Thread::Sleep(100);
        clProfileTimer.Stop();
        ulSleepCount++;
    }

    clProfileTimer.Stop();

    ulTimeVal = clProfileTimer.GetAverage() * CLOCK_DIVIDE * ulSleepCount;
    ulTempTime = SYSTEM_FREQ * 10;

    EXPECT_GT(ulTimeVal, ulTempTime);

    // Test point - 100ms accuracy over 10 seconds
    ulTempTime += SYSTEM_FREQ / 10;
    EXPECT_LT(ulTimeVal, ulTempTime);

    Profiler::Stop();
}
TEST_END

TEST(ut_timer_repeat)
{
    // Profiling timer is not really designed for long profiling
    // operations (1.2 seconds is about as high as we get, since it's
    // so high resolution).  So, use sleeps and multiple iterations
    // w/averaging in order to verify.
    K_ULONG ulSleepCount = 0;
    Profiler::Start();
    clTimerSem.Init(0, 1);

    // Repeated timer case - run a 10ms timer 100 times and measure
    // accuracy.  Average iteration must be > 10ms
    ulCallbackCount = 0;

    clProfileTimer.Init();
    clProfileTimer.Start();

    clTimer1.Start( true, 10, TimerCallback, 0 );

    while (ulCallbackCount < 100)
    {
        clTimerSem.Pend();
    }

    clProfileTimer.Stop();
    clTimer1.Stop();

    ulTimeVal = clProfileTimer.GetCurrent() * CLOCK_DIVIDE;
    ulTempTime = SYSTEM_FREQ;

    EXPECT_GT(ulTimeVal, ulTempTime);

#if KERNEL_TIMERS_TICKLESS
    // Test point - 50ms (5%) maximum tolerance for callback overhead, etc.
    ulTempTime += SYSTEM_FREQ / 20;
    EXPECT_LT(ulTimeVal, ulTempTime);
#else
    // Test point - 100ms (10%) maximum tolerance for callback overhead, etc.
    ulTempTime += SYSTEM_FREQ / 10;
    EXPECT_LT(ulTimeVal, ulTempTime);
#endif

#if 0
    // Debug code to print out the profiling times
    Driver *pclDriver = DriverList::FindByPath("/dev/tty");
    K_CHAR acData[13];
    MemUtil::DecimalToString(ulTimeVal, acData);
    pclDriver->Write( MemUtil::StringLength(acData), (K_UCHAR*)acData);
    pclDriver->Write(1, (K_UCHAR*)(" "));
    MemUtil::DecimalToString(ulTempTime, acData);
    pclDriver->Write( MemUtil::StringLength(acData), (K_UCHAR*)acData);
#endif

    Profiler::Stop();

}
TEST_END

TEST(ut_timer_multi)
{
    Profiler::Start();
    clTimerSem.Init(0, 3);

    // Test using multiple timers simultaneously, verify that
    // each of them expire at the expected times within a specific
    // tolerance

    clProfileTimer.Init();
    clProfileTimer2.Init();
    clProfileTimer3.Init();

    clProfileTimer.Start();
    clTimer1.Start( false, 100, TimerCallback, 0 );
    clProfileTimer2.Start();
    clTimer2.Start( false, 200, TimerCallback, 0 );
    clProfileTimer3.Start();
    clTimer3.Start( false, 50, TimerCallback, 0 );

    // Each timer expiry will post the semaphore.
    clTimerSem.Pend();
    clProfileTimer3.Stop();

    clTimerSem.Pend();
    clProfileTimer.Stop();

    clTimerSem.Pend();
    clProfileTimer2.Stop();

    // Test Point - Timer 1 expired @ 100ms, with a 1 ms tolerance
    ulTimeVal = clProfileTimer.GetCurrent() * CLOCK_DIVIDE;
    ulTempTime = SYSTEM_FREQ / 10;
    EXPECT_GT(ulTimeVal, ulTempTime);

    ulTempTime += SYSTEM_FREQ / 1000;
    EXPECT_LT(ulTimeVal, ulTempTime);

    // Test Point - Timer 2 expired @ 200ms, with a 1 ms tolerance
    ulTimeVal = clProfileTimer2.GetCurrent() * CLOCK_DIVIDE;
    ulTempTime = SYSTEM_FREQ / 5;
    EXPECT_GT(ulTimeVal, ulTempTime);

    ulTempTime += SYSTEM_FREQ / 1000;
    EXPECT_LT(ulTimeVal, ulTempTime);

    // Test Point - Timer 3 expired @ 50ms, with a 1 ms tolerance
    ulTimeVal = clProfileTimer3.GetCurrent() * CLOCK_DIVIDE;
    ulTempTime = SYSTEM_FREQ / 20;
    EXPECT_GT(ulTimeVal, ulTempTime);

    ulTempTime += SYSTEM_FREQ / 1000;
    EXPECT_LT(ulTimeVal, ulTempTime);

    Profiler::Stop();
}
TEST_END


//===========================================================================
// Test Whitelist Goes Here
//===========================================================================
TEST_CASE_START
  TEST_CASE(ut_timer_tolerance),
  TEST_CASE(ut_timer_longrun),
  TEST_CASE(ut_timer_repeat),
  TEST_CASE(ut_timer_multi),
TEST_CASE_END
