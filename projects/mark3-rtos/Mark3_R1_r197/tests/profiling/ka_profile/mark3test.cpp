
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
#include "kernel_aware.h"

extern "C" void __cxa_pure_virtual() { }
//---------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//---------------------------------------------------------------------------
static volatile K_UCHAR ucTestVal;

//---------------------------------------------------------------------------
#define TEST_STACK1_SIZE            (384)
#define TEST_STACK2_SIZE            (32)
#define TEST_STACK3_SIZE            (32)
#define MAIN_STACK_SIZE            (384)
#define IDLE_STACK_SIZE            (384)

//---------------------------------------------------------------------------
static Semaphore clSemaphore;
static Mutex clMutex;

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
static void Profile_Init()
{
    
}

//---------------------------------------------------------------------------
static void Profile_Overhead()
{

    CS_ENTER();
    Kernel_Aware::Profile_Init("Overhead");
    Kernel_Aware::Profile_Start();
    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();
    CS_EXIT();

}

//---------------------------------------------------------------------------
static void Semaphore_Flyback( Semaphore *pclSem_ )
{        
    Kernel_Aware::Profile_Init("SemFlyback");
    Kernel_Aware::Profile_Start();

    pclSem_->Pend();

    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();

    Scheduler::GetCurrentThread()->Exit();
}

//---------------------------------------------------------------------------
static void Semaphore_Profiling()
{
    Semaphore clSem;

    CS_ENTER();
    Kernel_Aware::Profile_Init("SemInit");
    Kernel_Aware::Profile_Start();

    clSem.Init(0,1000);

    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();
    CS_EXIT();

    CS_ENTER();
    Kernel_Aware::Profile_Init("SemPost");
    Kernel_Aware::Profile_Start();

    clSem.Post();

    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();
    CS_EXIT();

    CS_ENTER();
    Kernel_Aware::Profile_Init("SemPend");
    Kernel_Aware::Profile_Start();

    clSem.Pend();

    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();
    CS_EXIT();

    clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)Semaphore_Flyback, (void*)&clSem);
    clTestThread1.Start();
    clSem.Post();

    return;
}

//---------------------------------------------------------------------------
static void Mutex_Profiling()
{
    K_USHORT i;
    Mutex clMutex;
    
    CS_ENTER();
    Kernel_Aware::Profile_Init("MutexInit");
    Kernel_Aware::Profile_Start();

    clMutex.Init();

    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();
    CS_EXIT();

    CS_ENTER();
    Kernel_Aware::Profile_Init("MutexClaim");
    Kernel_Aware::Profile_Start();

    clMutex.Claim();

    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();
    CS_EXIT();

    CS_ENTER();
    Kernel_Aware::Profile_Init("MutexRelease");
    Kernel_Aware::Profile_Start();

    clMutex.Release();

    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();
    CS_EXIT();

}

//---------------------------------------------------------------------------
static void Thread_ProfilingThread()
{
    // Stop the "thread start" profiling timer, which was started from the 
    // main app thread
    Kernel_Aware::Profile_Stop();
    Kernel_Aware::Profile_Report();
    
    // Start the "thread exit" profiling timer, which will be stopped after
    // returning back to the main app thread

    Kernel_Aware::Profile_Init("ThreadExit");
    Kernel_Aware::Profile_Start();
    Scheduler::GetCurrentThread()->Exit();

}

//---------------------------------------------------------------------------
static void Thread_Profiling()
{

        // Profile the amount of time it takes to initialize a representative
        // test thread, simulating an "average" system thread.  Create the 
        // thread at a higher priority than the current thread.
        Kernel_Aware::Profile_Init("ThreadInit");
        CS_ENTER();
        Kernel_Aware::Profile_Start();
        clTestThread1.Init(aucTestStack1, TEST_STACK1_SIZE, 2, (ThreadEntry_t)Thread_ProfilingThread, NULL);
        Kernel_Aware::Profile_Stop();
        CS_EXIT();
        Kernel_Aware::Profile_Report();

        // Profile the time it takes from calling "start" to the time when the
        // thread becomes active
        Kernel_Aware::Profile_Init("ThreadStart");

        CS_ENTER();
        Kernel_Aware::Profile_Start();
        clTestThread1.Start(); //-- Switch to the test thread --
        CS_EXIT();

        CS_ENTER();
        // Stop the thread-exit profiling timer, which was started from the
        // test thread
        Kernel_Aware::Profile_Stop();
        Kernel_Aware::Profile_Report();
        CS_EXIT();

        Scheduler::SetScheduler(0);
        // Context switch profiling - this is equivalent to what's actually
        // done within the AVR-implementation.
        CS_ENTER();
        Kernel_Aware::Profile_Init("ContextSwitch");
        Kernel_Aware::Profile_Start();
        {
            Thread_SaveContext();
            g_pstNext = g_pstCurrent;
            Thread_RestoreContext();
        }
        Kernel_Aware::Profile_Stop();
        Kernel_Aware::Profile_Report();
        CS_EXIT();

        Scheduler::SetScheduler(1);
}

//---------------------------------------------------------------------------
void Scheduler_Profiling()
{

        // Profile the scheduler.  Running at priority 1, we'll get
        // the worst-case scheduling time (not necessarily true of all 
        // schedulers, but true of ours).
    CS_ENTER();
    Kernel_Aware::Profile_Init("Scheduler");
    Kernel_Aware::Profile_Start();
    Scheduler::Schedule();
    Kernel_Aware::Profile_Stop();
    CS_EXIT();
    Kernel_Aware::Profile_Report();
}

//---------------------------------------------------------------------------
static void AppMain( void *unused )
{

    Profile_Init();

    while(1)
    {
        //---[ API Profiling ]-----------------------------
        Profile_Overhead();
        Semaphore_Profiling();
        Mutex_Profiling();
        Thread_Profiling();
        Scheduler_Profiling();
        Kernel_Aware::Exit_Simulator();
    }
}
