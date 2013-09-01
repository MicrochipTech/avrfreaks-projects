#include "kerneltypes.h"
#include "mark3cfg.h"
#include "kernel.h"
#include "thread.h"
#include "driver.h"
#include "drvUART.h"
#include "profile.h"
#include "kprofile.h"
#include "ksemaphore.h"
#include "mutex.h"
#include "message.h"
#include "timerlist.h"
#include "unit_test.h"

extern "C" void __cxa_pure_virtual() { }
//---------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//---------------------------------------------------------------------------
static volatile K_UCHAR ucTestVal;
#define TX_BUF_SIZE             (4)
//---------------------------------------------------------------------------
static ATMegaUART clUART;
static K_UCHAR aucTxBuf[TX_BUF_SIZE];

#define PROFILE_TEST 0
#if PROFILE_TEST

//---------------------------------------------------------------------------
#define TEST_STACK1_SIZE            (256)
#define TEST_STACK2_SIZE            (32)
#define TEST_STACK3_SIZE            (32)
#define MAIN_STACK_SIZE            (256)
#define IDLE_STACK_SIZE            (256)

//---------------------------------------------------------------------------
static ProfileTimer clProfileOverhead;

static ProfileTimer clSemInitTimer;
static ProfileTimer clSemPostTimer;
static ProfileTimer clSemPendTimer;

static ProfileTimer clMutexInitTimer;
static ProfileTimer clMutexClaimTimer;
static ProfileTimer clMutexReleaseTimer;

static ProfileTimer clThreadInitTimer;
static ProfileTimer clThreadStartTimer;
static ProfileTimer clThreadExitTimer;
static ProfileTimer clContextSwitchTimer;

static ProfileTimer clSemaphoreFlyback;
static ProfileTimer clSchedulerTimer;
#endif

//---------------------------------------------------------------------------
static Semaphore clSemaphore;
static Mutex clMutex;

#define UNIT_TEST 1
#if UNIT_TEST

//---------------------------------------------------------------------------
#define TEST_STACK1_SIZE            (224)
#define TEST_STACK2_SIZE            (224)
#define TEST_STACK3_SIZE            (224)
#define MAIN_STACK_SIZE            (224)
#define IDLE_STACK_SIZE            (224)

static UnitTest clSemTest;
static UnitTest clSleepTest;
static UnitTest clMutexTest;

static MessageQueue clMsgQ1;
static MessageQueue clMsgQ2;
static UnitTest    clMsgQTest;

static UnitTest clRoundRobinTest;
static UnitTest clQuantumTest;
static UnitTest clTimerTest;

static UnitTest clTimedSemaphoreTest;

static Thread clTestThread2;
static Thread clTestThread3;

static K_UCHAR aucTestStack2[TEST_STACK2_SIZE];
static K_UCHAR aucTestStack3[TEST_STACK3_SIZE];

#endif

//---------------------------------------------------------------------------
static Thread clMainThread;
static Thread clIdleThread;

static Thread clTestThread1;

//---------------------------------------------------------------------------
static K_UCHAR aucMainStack[MAIN_STACK_SIZE];
static K_UCHAR aucIdleStack[IDLE_STACK_SIZE];
static K_UCHAR aucTestStack1[TEST_STACK1_SIZE];

//---------------------------------------------------------------------------
static void AppMain( void *unused );
static void IdleMain( void *unused );
static void PrintWait( Driver *pclDriver_, K_USHORT usSize_, const K_CHAR *data );
//---------------------------------------------------------------------------
int main(void)
{
    Kernel::Init();

    clMainThread.Init(  aucMainStack,
                        MAIN_STACK_SIZE,
                        1,                       
                        (ThreadEntry_t)AppMain,
                        NULL );
                        
    clIdleThread.Init(  aucIdleStack,
                        MAIN_STACK_SIZE,
                        0,
                        (ThreadEntry_t)IdleMain,
                        NULL );

    clMainThread.Start();
    clIdleThread.Start();

    clUART.SetName("/dev/tty");
    clUART.Init();
    
    DriverList::Add( &clUART );
    
    Kernel::Start();
}

//---------------------------------------------------------------------------
static void IdleMain( void *unused )
{
    while(1)
    {
#if 1
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
// Basic string routines
K_USHORT KUtil_Strlen( const K_CHAR *szStr_ )
{
    K_CHAR *pcData = (K_CHAR*)szStr_;
    K_USHORT usLen = 0;

    while (*pcData++)
    {
        usLen++;
    }
    return usLen;
}

//---------------------------------------------------------------------------
void KUtil_Ultoa( K_ULONG ucData_, K_CHAR *szText_ )
{
    K_ULONG ucMul;
    K_ULONG ucMax;

    // Find max index to print...
    ucMul = 10;
    ucMax = 1;
    while ( ucMul <= ucData_ )
    {
        ucMax++;
        ucMul *= 10; 
    }
    
    szText_[ucMax] = 0;
    while (ucMax--)
    {
        szText_[ucMax] = '0' + (ucData_ % 10);
        ucData_/=10;
    }
}

#if PROFILE_TEST
//---------------------------------------------------------------------------
static void Profile_Init()
{
    
    clProfileOverhead.Init();
    
    clSemInitTimer.Init();
    clSemPendTimer.Init();    
    clSemPostTimer.Init();
    clSemaphoreFlyback.Init();
    
    clMutexInitTimer.Init();
    clMutexClaimTimer.Init();
    clMutexReleaseTimer.Init();
    
    clThreadExitTimer.Init();
    clThreadInitTimer.Init();
    clThreadStartTimer.Init();
    clContextSwitchTimer.Init();
    
    clSchedulerTimer.Init();
}

//---------------------------------------------------------------------------
static void Profile_Overhead()
{
    K_USHORT i;
    for (i = 0; i < 100; i++)
    {
        clProfileOverhead.Start();
        clProfileOverhead.Stop();
    }
}

//---------------------------------------------------------------------------
static void Semaphore_Flyback( Semaphore *pclSem_ )
{    

    clSemaphoreFlyback.Start();
    pclSem_->Pend();
    clSemaphoreFlyback.Stop();
    
    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
static void Semaphore_Profiling()
{
    Semaphore clSem;

    K_USHORT i;
    
    for (i = 0; i < 100; i++)
    {
        clSemInitTimer.Start();        
        clSem.Init(0, 1000);        
        clSemInitTimer.Stop();
    }

    for (i = 0; i < 100; i++)
    {        
        clSemPostTimer.Start();
        clSem.Post();        
        clSemPostTimer.Stop();    
    }
    
    for (i = 0; i < 100; i++)
    {    
        clSemPendTimer.Start();    
        clSem.Pend();        
        clSemPendTimer.Stop();    
    }
    
    clSem.Init(0, 1);
    for (i = 0; i < 100; i++)
    {
        clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)Semaphore_Flyback, (void*)&clSem);
        clTestThread1.Start();
        
        clSem.Post();
    }
    
    return;
}

//---------------------------------------------------------------------------
static void Mutex_Profiling()
{
    K_USHORT i;
    Mutex clMutex;
    
    for (i = 0; i < 100; i++)
    {
        clMutexInitTimer.Start();
        clMutex.Init();
        clMutexInitTimer.Stop();
    }
    
    for (i = 0; i < 100; i++)
    {
        clMutexClaimTimer.Start();
        clMutex.Claim();
        clMutexClaimTimer.Stop();
        
        clMutexReleaseTimer.Start();
        clMutex.Release();
        clMutexReleaseTimer.Stop();
    }
}

//---------------------------------------------------------------------------
static void Thread_ProfilingThread()
{
    // Stop the "thread start" profiling timer, which was started from the 
    // main app thread
    clThreadStartTimer.Stop();
    
    // Start the "thread exit" profiling timer, which will be stopped after
    // returning back to the main app thread
    clThreadExitTimer.Start();
    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
static void Thread_Profiling()
{
    K_USHORT i;
    
    for (i = 0; i < 100; i++)
    {
        // Profile the amount of time it takes to initialize a representative
        // test thread, simulating an "average" system thread.  Create the 
        // thread at a higher priority than the current thread.
        clThreadInitTimer.Start();
        clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)Thread_ProfilingThread, NULL);
        clThreadInitTimer.Stop();
        
        // Profile the time it takes from calling "start" to the time when the
        // thread becomes active
        clThreadStartTimer.Start();
        
        clTestThread1.Start(); //-- Switch to the test thread --
        
        // Stop the thread-exit profiling timer, which was started from the
        // test thread
         clThreadExitTimer.Stop();         
    }
    
    Scheduler::SetScheduler(0);
    for (i = 0; i < 100; i++)
    {
        // Context switch profiling - this is equivalent to what's actually
        // done within the AVR-implementation.
        clContextSwitchTimer.Start();
        {
            Thread_SaveContext();
            g_pstNext = g_pstCurrent;
            Thread_RestoreContext();
        }
        clContextSwitchTimer.Stop();
    }
    Scheduler::SetScheduler(1);
}

//---------------------------------------------------------------------------
void Scheduler_Profiling()
{
    K_USHORT i;
    
    for (i = 0; i < 100; i++)
    {
        // Profile the scheduler.  Running at priority 1, we'll get
        // the worst-case scheduling time (not necessarily true of all 
        // schedulers, but true of ours).
        clSchedulerTimer.Start();
        Scheduler::Schedule();    
        clSchedulerTimer.Stop();
    }    
}

//---------------------------------------------------------------------------
void Profile_Print( ProfileTimer *pclProfile_, const K_CHAR *szName_ )
{
    Driver *pclUART = DriverList::FindByPath("/dev/tty");
    K_CHAR szBuf[16];
    K_ULONG ulVal = pclProfile_->GetAverage() - clProfileOverhead.GetAverage();
    ulVal *= 8;
    
    pclUART->Write( KUtil_Strlen(szName_), (K_UCHAR*)szName_ );
    KUtil_Ultoa(ulVal, szBuf);
    
    pclUART->Write( 2, (K_UCHAR*)": " );
    pclUART->Write( KUtil_Strlen(szBuf), (K_UCHAR*)szBuf );
    pclUART->Write( 1, (K_UCHAR*)"\n" );
    Thread::Sleep(100);
}

//---------------------------------------------------------------------------
void Profile_PrintResults()
{
    Profile_Print( &clMutexInitTimer, "MI");
    Profile_Print( &clMutexClaimTimer, "MC");
    Profile_Print( &clMutexReleaseTimer, "MC");
    Profile_Print( &clSemInitTimer, "SI");
    Profile_Print( &clSemPendTimer, "SPo");
    Profile_Print( &clSemPostTimer, "SPe");
    Profile_Print( &clSemaphoreFlyback, "SF");
    Profile_Print( &clThreadExitTimer, "TE");
    Profile_Print( &clThreadInitTimer, "TI");
    Profile_Print( &clThreadStartTimer, "TS");
    Profile_Print( &clContextSwitchTimer, "CS");
    Profile_Print( &clSchedulerTimer, "SC");
    Profile_Print( &clTimedSemaphoreTest, "ST" );
}

#endif

#if UNIT_TEST

#if 1
//---------------------------------------------------------------------------
void TestSemThread(Semaphore *pstSem_)
{
    pstSem_->Pend();    
    if (ucTestVal != 0x12)
    {
        clSemTest.Fail();
    }
    ucTestVal = 0x21;

    pstSem_->Pend();    
    if (ucTestVal != 0x32)
    {
        clSemTest.Fail();
    }
    ucTestVal = 0x23;
    
    pstSem_->Pend();

    if (ucTestVal != 0x45)
    {
        clSemTest.Fail();
    }
    ucTestVal = 0x54;

    Scheduler::GetCurrentThread()->Exit();
}
//---------------------------------------------------------------------------
// Binary semaphore test:
//  Create a worker thread at a higher priority, which pends on a semaphore
//  that we hold.  The main thread and the new thread alternate pending/posting
//  the semaphore, while modifying/verifying a global variable.
//---------------------------------------------------------------------------
void UT_SemaphoreTest(void)
{
    clSemaphore.Init(0, 1);
    clSemTest.Start();
    
    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 3, (ThreadEntry_t)TestSemThread, (void*)&clSemaphore);
    clTestThread1.Start();

    
    ucTestVal = 0x12;
    
    clSemaphore.Post();    

    if (ucTestVal != 0x21)
    {
        clSemTest.Fail();
    }
    
    ucTestVal = 0x32;
    clSemaphore.Post();

    if (ucTestVal != 0x23)
    {
        clSemTest.Fail();
    }
    
    ucTestVal = 0x45;
    clSemaphore.Post();
    if (ucTestVal != 0x54)
    {
        clSemTest.Fail();
    } else {
        clSemTest.Pass();
    }
}

//---------------------------------------------------------------------------
void TimedSemaphoreThread_Short( Semaphore *pstSem_ )
{
    Scheduler::GetCurrentThread()->Sleep(10);
    pstSem_->Post();
    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
void TimedSemaphoreThread_Long( Semaphore *pstSem_ )
{
    Scheduler::GetCurrentThread()->Sleep(20);
    pstSem_->Post();
    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
void UT_TimedSemaphoreTest(void)
{
    Semaphore clSem;

    Scheduler::GetCurrentThread()->SetPriority(3);
    
    clSem.Init(0,1);
    clTimedSemaphoreTest.Start();
    
    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TimedSemaphoreThread_Short, (void*)&clSem );
    clTestThread1.Start();
    
// Test 1 - block on a semaphore, wait on thread that will post before expiry
    
    if (!clSem.Pend(15))
    {
        // FAIL if we expired
        clTimedSemaphoreTest.Fail();
    }
    else
    {
        // Pass if we did not expire
        clTimedSemaphoreTest.Pass();
    }
    
#if 1
// Test 2 - block on a semaphore, wait on thread that will post after expiry
    clSem.Init(0,1);
    clTimedSemaphoreTest.Start();
    
    //clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TimedSemaphoreThread_Long, (void*)&clSem );
    //clTestThread1.Start();    
    
    
    if (!clSem.Pend(15))
    {
        // PASS if we expired
        clTimedSemaphoreTest.Pass();
    }
    else
    {
        // Fail if we did not expire
        clTimedSemaphoreTest.Fail();
    }
#endif    
    Scheduler::GetCurrentThread()->SetPriority(1);
}

//---------------------------------------------------------------------------

void TestSleepThread(void *pvArg_)
{
    while(1)
    {
        ucTestVal = 0xAA;
    }
}

//---------------------------------------------------------------------------
// Sleep Test
//  Verify that thread sleeping works as expected.  Check that the lower
//  priority thread is able to execute, setting the global variable to a
//  target value.
//---------------------------------------------------------------------------
void UT_SleepTest(void)
{
    clSleepTest.Start();
    
    Scheduler::GetCurrentThread()->SetPriority(3);
    
    ucTestVal = 0x00;
    
    // Create a lower-priority thread that sets the test value to a known
    // cookie.
    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TestSleepThread, NULL);
    clTestThread1.Start();
    
    // Sleep, when we wake up check the test value
    Thread::Sleep(5);
    
    if (ucTestVal != 0xAA)
    {
        clSleepTest.Fail();
    } else {
        clSleepTest.Pass();    
    }
        
    clTestThread1.Exit();
    
    Scheduler::GetCurrentThread()->SetPriority(1);
    
}

//---------------------------------------------------------------------------
void TestMutexThread(Mutex *pclMutex_)
{
    pclMutex_->Claim();
    
    if (ucTestVal != 0xDC)
    {
        clMutexTest.Fail();
        
    }
    ucTestVal = 0xAC;
    pclMutex_->Release();

    Scheduler::GetCurrentThread()->Exit();
}


//---------------------------------------------------------------------------
void TestTimedMutexThreadShort(Mutex *pclMutex_)
{
    pclMutex_->Claim();
    Scheduler::GetCurrentThread()->Sleep(10);
    pclMutex_->Release();

    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
void TestTimedMutexThreadLong(Mutex *pclMutex_)
{
    pclMutex_->Claim();
    Scheduler::GetCurrentThread()->Sleep(20);
    pclMutex_->Release();

    Scheduler::GetCurrentThread()->Exit();
}


//---------------------------------------------------------------------------
// Mutex test
//  Create a mutex and claim it.  While the mutex is owned, create a new
//  thread at a higher priority, which tries to claim the mutex itself.
//  Use a global variable to verify that the threads do not proceed outside
//  of the control.
//---------------------------------------------------------------------------
void UT_MutexTest(void)
{
    clMutex.Init();
    clMutexTest.Start();
    
    ucTestVal = 0x10;
    clMutex.Claim();

    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TestMutexThread, (void*)&clMutex );
    clTestThread1.Start();
    
    ucTestVal = 0xDC;
    
    clMutex.Release();
    
    if (ucTestVal != 0xAC)
    {
        clMutexTest.Fail();
    }
    else
    {
        clMutexTest.Pass();
    }
    
#if KERNEL_USE_TIMERS
    clMutex.Init();
    clMutexTest.Start();
    
    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TestTimedMutexThreadShort, (void*)&clMutex );
    clTestThread1.Start();
    
    if (!clMutex.Claim(15))
    {
        clMutexTest.Fail();
    }
    else
    {
        clMutexTest.Pass();
    }
    
    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TestTimedMutexThreadLong, (void*)&clMutex );
    clTestThread1.Start();
    
    if (clMutex.Claim(15))
    {
        clMutexTest.Fail();
    }
    else
    {
        clMutexTest.Pass();
    }    
#endif    
        
}

//---------------------------------------------------------------------------
void TimedMessageThread(MessageQueue *pclMsgQ_)
{
    Message *pclMsg = GlobalMessagePool::Pop();
    
    pclMsg->SetData(0);
    pclMsg->SetCode(0);
    
    Scheduler::GetCurrentThread()->Sleep(10);
    
    pclMsgQ_->Send(pclMsg);
    
    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
void UT_TimedMessageTest(void)
{
#if KERNEL_USE_TIMERS
    Message *pclMsg;
    
    clMsgQTest.Start();
    
    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TimedMessageThread, (void*)&clMsgQ1);
    clTestThread1.Start();
    
    pclMsg = clMsgQ1.Receive(15);
    
    if (!pclMsg)
    {
        clMsgQTest.Fail();
    }
    else
    {
        clMsgQTest.Pass();
        GlobalMessagePool::Push(pclMsg);
    }
    
    clMsgQTest.Start();
    pclMsg = clMsgQ1.Receive(10);
    
    if (pclMsg)
    {
        clMsgQTest.Fail();
        GlobalMessagePool::Push(pclMsg);
    }
    else
    {
        clMsgQTest.Pass();        
    }
    
#endif    
}

//---------------------------------------------------------------------------
void TestMessageTest(void *pvArg)
{
    Message *pclMesg;
    
    pclMesg = clMsgQ2.Receive();

    if (pclMesg->GetCode() != 0x11)
    {
        clMsgQTest.Fail();
    }
    
    GlobalMessagePool::Push(pclMesg);
    
    pclMesg = GlobalMessagePool::Pop();
    
    pclMesg->SetCode(0x22);
    
    clMsgQ1.Send(pclMesg);
    
    pclMesg = clMsgQ2.Receive();
    
    if (pclMesg->GetCode() != 0xAA)
    {
        clMsgQTest.Fail();
    }

    GlobalMessagePool::Push(pclMesg);
    
    pclMesg = clMsgQ2.Receive();

    if (pclMesg->GetCode() != 0xBB)
    {
        clMsgQTest.Fail();
    }

    GlobalMessagePool::Push(pclMesg);

    pclMesg = clMsgQ2.Receive();

    if (pclMesg->GetCode() != 0xCC)
    {
        clMsgQTest.Fail();
    }

    GlobalMessagePool::Push(pclMesg);

    pclMesg = GlobalMessagePool::Pop();
    pclMesg->SetCode(0xDD);
    clMsgQ1.Send(pclMesg);
    
    pclMesg = GlobalMessagePool::Pop();
    pclMesg->SetCode(0xEE);
    clMsgQ1.Send(pclMesg);
    
    pclMesg = GlobalMessagePool::Pop();
    pclMesg->SetCode(0xFF);
    clMsgQ1.Send(pclMesg);
    
    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
// Message test
//  Create a second thread that we communicate with by passing messages.
//  Ensure that messages being passed between threads are received as expected
//  in the correct FIFO order, and have the correct IDs.
//---------------------------------------------------------------------------
void UT_MessageTest(void)
{
    clMsgQTest.Start();
    clMsgQ1.Init();
    clMsgQ2.Init();    
    
    Message *pclMesg;

    pclMesg = GlobalMessagePool::Pop();

    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TestMessageTest, NULL);
    clTestThread1.Start();
    Thread::Yield();
    
    pclMesg->SetCode(0x11);
    pclMesg->SetData(NULL);
    
    clMsgQ2.Send(pclMesg);
    
    pclMesg = clMsgQ1.Receive();
    
    if (pclMesg->GetCode() != 0x22)
    {
        clMsgQTest.Fail();
    }
    GlobalMessagePool::Push(pclMesg);

    pclMesg = GlobalMessagePool::Pop();
    pclMesg->SetCode(0xAA);
    clMsgQ2.Send(pclMesg);
    
    pclMesg = GlobalMessagePool::Pop();
    pclMesg->SetCode(0xBB);
    clMsgQ2.Send(pclMesg);
    
    pclMesg = GlobalMessagePool::Pop();
    pclMesg->SetCode(0xCC);
    clMsgQ2.Send(pclMesg);
    
    pclMesg = clMsgQ1.Receive();
    if (pclMesg->GetCode() != 0xDD)
    {
        clMsgQTest.Fail();
    }
    GlobalMessagePool::Push(pclMesg);

    pclMesg = clMsgQ1.Receive();
    if (pclMesg->GetCode() != 0xEE)
    {
        clMsgQTest.Fail();
    }
    GlobalMessagePool::Push(pclMesg);
    
    pclMesg = clMsgQ1.Receive();
    if (pclMesg->GetCode() != 0xFF)
    {
        clMsgQTest.Fail();
    }
    GlobalMessagePool::Push(pclMesg);
    clMsgQTest.Pass();    
        
}
#endif

//---------------------------------------------------------------------------
void TestRRThread(volatile K_ULONG *pulCounter_)
{
    while (1)
    {
        (*pulCounter_)++;
    }
}

//---------------------------------------------------------------------------
// Round-Robin Thread
//  Create 3 threads in the same priority group (lower than our thread), and
//  set their quantums to different values.  Verify that the ratios of their
//  "work cycles" are close to equivalent.
//---------------------------------------------------------------------------

void UT_RoundRobinTest(void)
{
    volatile K_ULONG ulCounter1 = 0;
    volatile K_ULONG ulCounter2 = 0;
    volatile K_ULONG ulCounter3 = 0;
    K_ULONG ulDelta;

    clRoundRobinTest.Start();

    Scheduler::GetCurrentThread()->SetPriority(3);
    
    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TestRRThread, (void*)&ulCounter1 );
    clTestThread2.Init(aucTestStack2, TEST_STACK2_SIZE, 2, (ThreadEntry_t)TestRRThread, (void*)&ulCounter2 );
    clTestThread3.Init(aucTestStack3, TEST_STACK3_SIZE, 2, (ThreadEntry_t)TestRRThread, (void*)&ulCounter3 );
    
    clTestThread1.Start();
    clTestThread2.Start();
    clTestThread3.Start();    
    
    // Sleep for a while to let the other threads execute
    Thread::Sleep(120);  // Must be modal to the worker thread quantums

    // Kill the worker threads
    
    if (ulCounter1 > ulCounter2)
    {
        ulDelta = ulCounter1 - ulCounter2;
    }
    else
    {
        ulDelta = ulCounter2 - ulCounter1;
    }

    // Give or take...
    if (ulDelta > ulCounter1/2)
    {
        clRoundRobinTest.Fail();
    }

    if (ulCounter1 > ulCounter3)
    {
        ulDelta = ulCounter1 - ulCounter3;
    }
    else
    {
        ulDelta = ulCounter3 - ulCounter1;
    }

    // Give or take...
    if (ulDelta > ulCounter1/2)
    {
        clRoundRobinTest.Fail();
    }

    clTestThread1.Exit();
    clTestThread2.Exit();
    clTestThread3.Exit();
    
    clRoundRobinTest.Pass();
    Scheduler::GetCurrentThread()->SetPriority(1);
}

//---------------------------------------------------------------------------
void UT_QuantumTest(void)
{
    volatile K_ULONG ulCounter1 = 0;
    volatile K_ULONG ulCounter2 = 0;
    volatile K_ULONG ulCounter3 = 0;
    K_ULONG ulDelta;

    clQuantumTest.Start();

    Scheduler::GetCurrentThread()->SetPriority(3);
    
    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)TestRRThread, (void*)&ulCounter1 );
    clTestThread2.Init(aucTestStack2, TEST_STACK2_SIZE, 2, (ThreadEntry_t)TestRRThread, (void*)&ulCounter2 );
    clTestThread3.Init(aucTestStack3, TEST_STACK3_SIZE, 2, (ThreadEntry_t)TestRRThread, (void*)&ulCounter3 );
    
    clTestThread1.SetQuantum(10);
    clTestThread2.SetQuantum(20);
    clTestThread3.SetQuantum(30);
    
    clTestThread1.Start();
    clTestThread2.Start();
    clTestThread3.Start();    
    
    // Sleep for a while to let the other threads execute
    Thread::Sleep(180);  // Must be modal to the worker thread quantums

    // Kill the worker threads
    ulCounter2 /= 2;
    ulCounter3 /= 3;
    
    if (ulCounter1 > ulCounter2)
    {
        ulDelta = ulCounter1 - ulCounter2;
    }
    else
    {
        ulDelta = ulCounter2 - ulCounter1;
    }

    // Give or take...
    if (ulDelta > ulCounter1/2)
    {
        clQuantumTest.Fail();
    }

    if (ulCounter1 > ulCounter3)
    {
        ulDelta = ulCounter1 - ulCounter3;
    }
    else
    {
        ulDelta = ulCounter3 - ulCounter1;
    }

    // Give or take...
    if (ulDelta > ulCounter1/2)
    {
        clQuantumTest.Fail();
    }

    clTestThread1.Exit();
    clTestThread2.Exit();
    clTestThread3.Exit();
    
    clQuantumTest.Pass();
    Scheduler::GetCurrentThread()->SetPriority(1);
}

void TimerTestCallback(Thread *pclOwner_, void *pvData_)
{
    ucTestVal++;
}

void UT_TimerTest(void)
{
    Timer clTimer;
    
    clTimerTest.Start();
    
    ucTestVal = 0;

    clTimer.Init();
    clTimer.Start(1, 2, TimerTestCallback, NULL);
    
    Thread::Sleep(3);
    if (ucTestVal != 1)
    {
        clTimerTest.Fail();
    }
    
    ucTestVal = 0;
    clTimer.Stop();

    clTimer.Start(1, 1, TimerTestCallback, NULL);
    
    Thread::Sleep(10);
    
    if (ucTestVal < 9)
    {
        clTimerTest.Fail();
    }
    
    clTimer.Stop();
    clTimerTest.Pass();
}
//---------------------------------------------------------------------------
static void UT_Init()
{
    clTimerTest.SetName("Timer");
    clQuantumTest.SetName("Quantum");
    clRoundRobinTest.SetName("RR");
    clSemTest.SetName("Sem");
    clMutexTest.SetName("Mutex");
    clSleepTest.SetName("Sleep");
    clMsgQTest.SetName("Message");
    clTimedSemaphoreTest.SetName("TimedSem");
}

//---------------------------------------------------------------------------
static void PrintWait( Driver *pclDriver_, K_USHORT usSize_, const K_CHAR *data )
{
    K_USHORT usWritten = 0;
    
    while (usWritten < usSize_)
    {
        usWritten += pclDriver_->Write((usSize_ - usWritten), (K_UCHAR*)(&data[usWritten]));
    }
}

//---------------------------------------------------------------------------
static void UT_Print( UnitTest *pclTest_ )
{
    const K_CHAR *pass = "PASS";
    const K_CHAR *fail = "FAIL";
    const K_CHAR *pcName;

    Driver *pclUART = DriverList::FindByPath("/dev/tty");
    
    PrintWait( pclUART, KUtil_Strlen(pclTest_->GetName()), (const K_CHAR*)pclTest_->GetName() );
    if( pclTest_->GetResult())
    {
        pcName = pass;
    }
    else
    {
        pcName = fail;
    }
    PrintWait( pclUART,  2, ": " );
    PrintWait( pclUART, 4, (const K_CHAR*) pcName );
    PrintWait( pclUART, 1, "\n" );
}

//---------------------------------------------------------------------------
static void UT_PrintResults()
{
    UT_Print(&clTimerTest);
    UT_Print(&clQuantumTest);
    UT_Print(&clRoundRobinTest);
    UT_Print(&clSemTest);
    UT_Print(&clTimedSemaphoreTest);
    UT_Print(&clMutexTest);
    UT_Print(&clSleepTest);
    UT_Print(&clMsgQTest);
}
#endif


//---------------------------------------------------------------------------
static void AppMain( void *unused )
{
    Driver *pclUART = DriverList::FindByPath("/dev/tty");
    K_ULONG ulIter = 0;
    
#if UNIT_TEST    
    UT_Init();
#endif
    
#if PROFILE_TEST
    Profile_Init();
#endif    

    pclUART->Control(CMD_SET_BUFFERS, NULL, 0, aucTxBuf, TX_BUF_SIZE);
    {
        K_ULONG ulBaudRate = 57600;
        pclUART->Control(CMD_SET_BAUDRATE, &ulBaudRate, 0, 0, 0 );
        pclUART->Control(CMD_SET_RX_DISABLE, 0, 0, 0, 0);
    }
    
    pclUART->Open();
    pclUART->Write(6,(K_UCHAR*)"START\n");

    while(1)
    {
#if UNIT_TEST
    
        //---[ Behavioral Tests ]--------------------------
        PrintWait( pclUART, KUtil_Strlen("Iteration:"), "Iteration:");
        {
            K_CHAR szBuf[16];
            KUtil_Ultoa(ulIter, szBuf);        
            PrintWait( pclUART, KUtil_Strlen(szBuf), (const K_CHAR*)szBuf);
            PrintWait( pclUART, 1, "\n");
            ulIter++;
        }
        UT_SemaphoreTest();
        UT_TimedSemaphoreTest();
        UT_SleepTest();
        UT_MutexTest();
        UT_MessageTest();
        UT_TimedMessageTest();
        UT_RoundRobinTest();
        UT_QuantumTest();
        UT_TimerTest();        
        UT_PrintResults();
        
#endif

#if PROFILE_TEST
        //---[ API Profiling ]-----------------------------
        Profiler::Start();
        Profile_Overhead();        
        Semaphore_Profiling();
        Mutex_Profiling();
        Thread_Profiling();
        Scheduler_Profiling();
        Profiler::Stop();
                
        Profile_PrintResults();
        Thread::Sleep(500);
#endif        
    }
}
