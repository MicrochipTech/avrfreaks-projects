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
#include "ksemaphore.h"
#include "thread.h"
#include "memutil.h"
#include "driver.h"
#include "kernel_aware.h"
//===========================================================================
// Local Defines
//===========================================================================

//===========================================================================
// Define Test Cases Here
//===========================================================================
TEST(ut_semaphore_count)
{
    // Test - verify that we can only increment a counting semaphore to the
    // maximum count.

    // Verify that a counting semaphore with an initial count of zero and a
    // maximum count of 10 can only be posted 10 times before saturaiton and
    // failure.

    Semaphore clTestSem;
    clTestSem.Init( 0, 10 );

    for (int i = 0; i < 10; i++)
    {
        EXPECT_TRUE(clTestSem.Post());
    }
    EXPECT_FALSE(clTestSem.Post());
}
TEST_END

//===========================================================================
static Thread clThread;
static K_UCHAR aucStack[300];
static Semaphore clSem1;
static Semaphore clSem2;
static volatile K_UCHAR ucCounter = 0;

//===========================================================================
void PostPendFunction(void *param_)
{
    Semaphore *pclSem = (Semaphore*)param_;
    while(1)
    {
        pclSem->Pend();
        ucCounter++;
    }
}

//===========================================================================
TEST(ut_semaphore_post_pend)
{
    // Test - Make sure that pending on a semaphore causes a higher-priority
    // waiting thread to block, and that posting that semaphore from a running
    // lower-priority thread awakens the higher-priority thread

    clSem1.Init(0, 1);

    clThread.Init(aucStack, 300, 7, PostPendFunction, (void*)&clSem1);
    clThread.Start();
    Kernel_Aware::Profile_Init("seminit");
    for (int i = 0; i < 10; i++)
    {
        Kernel_Aware::Profile_Start();
        clSem1.Post();
        Kernel_Aware::Profile_Stop();
    }
    Kernel_Aware::Profile_Report();

    // Verify all 10 posts have been acknowledged by the high-priority thread
    EXPECT_EQUALS(ucCounter, 10);

    // After the test is over, kill the test thread.
    clThread.Exit();

    // Test - same as above, but with a counting semaphore instead of a
    // binary semaphore.  Also using a default value.
    clSem2.Init(10, 10);

    // Restart the test thread.
    ucCounter = 0;
    clThread.Init(aucStack, 300, 7, PostPendFunction, (void*)&clSem2);
    clThread.Start();

    // We'll kill the thread as soon as it blocks.
    clThread.Exit();

    // semaphore should have pended 10 times before returning.
    EXPECT_EQUALS(ucCounter, 10);
}
TEST_END

//===========================================================================
void TimeSemFunction(void *param_)
{
    Semaphore *pclSem = (Semaphore*)param_;

    Thread::Sleep(20);
    pclSem->Post();
    Scheduler::GetCurrentThread()->Exit();
}

//===========================================================================
TEST(ut_semaphore_timed)
{
    Semaphore clTestSem;
    Semaphore clTestSem2;

    clTestSem.Init(0,1);

    clThread.Init(aucStack, 300, 7, TimeSemFunction, (void*)&clTestSem);
    clThread.Start();

    EXPECT_FALSE( clTestSem.Pend(10) );
    Thread::Sleep(20);

    // Pretty nuanced - we can only re-init the semaphore under the knowledge
    // that there's nothing blocking on it already...  don't do this in
    // production
    clTestSem2.Init(0,1);

    clThread.Init(aucStack, 300, 7, TimeSemFunction, (void*)&clTestSem2);
    clThread.Start();

    EXPECT_TRUE( clTestSem2.Pend(30) );
}
TEST_END

//===========================================================================
// Test Whitelist Goes Here
//===========================================================================
TEST_CASE_START
  TEST_CASE(ut_semaphore_count),
  TEST_CASE(ut_semaphore_post_pend),
  TEST_CASE(ut_semaphore_timed),
TEST_CASE_END
