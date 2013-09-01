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
    \file threadport.h
    \brief POSIX Implementation of Mark3 threads
*/
#include "threadport.h"

#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "kerneltypes.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "ksemaphore.h"
#include "scheduler.h"

//---------------------------------------------------------------------------
static pthread_mutex_t  stMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t   stCond = PTHREAD_COND_INITIALIZER;

static K_USHORT usCSCount = 0;
static K_BOOL bInit = false;

//---------------------------------------------------------------------------
static volatile Thread *pstChosen = NULL;

//---------------------------------------------------------------------------
volatile K_USHORT usIntFlags = 0;
volatile K_BOOL bInterrupt = false;
volatile K_BOOL bIntEnabled = true;
volatile K_BOOL bTriggerInt = false;
volatile K_BOOL bSwitchOnCS = false;
sem_t stKSem;


static Thread clBridgeThread;
static K_UCHAR aucBridgeStack[128];
static Semaphore clBridgeSem;
static sem_t stBridgeSem;

static volatile bool bIsThreadUp = false;
static volatile bool bIsThreadUnlocked = false;

class BridgeLock : private BlockingObject
{
public:
    void LockThread( Thread *pclThread_, sem_t *pstSem_ )
    {
        m_pclBlocked = pclThread_;
        m_pstSem = pstSem_;

        fprintf(stderr, "Blocking\n");

        BlockingObject::Block(pclThread_);
        Scheduler::Schedule();
        fprintf(stderr,"Curr :%X, Next %X\n", g_pstCurrent, g_pstNext);
        g_pstCurrent = g_pstNext;
        usIntFlags |= FLAG_SWI;
        sem_post(&stKSem);
        sem_wait(&stBridgeSem);
    }

    void UnlockThread()
    {
        BlockingObject::UnBlock(m_pclBlocked);
        Scheduler::Schedule();
        g_pstCurrent = g_pstNext;
        sem_post(&stBridgeSem);
    }

private:

    sem_t *m_pstSem;
    Thread *m_pclBlocked;
};

static BridgeLock clBridgeLock;

//---------------------------------------------------------------------------

static void Thread_StartInterrupts( void );
static void Thread_Interrupts( void );
static void Thread_InitInternal( void );

//---------------------------------------------------------------------------
static void BridgeThread(void *unused)
{
    Thread *this_thread = g_pstCurrent;
    //!! ToDo - set this thread at a higher priority than the user threads.
    {
        struct sched_param stParam;
        stParam.sched_priority = sched_get_priority_max( SCHED_RR ) - 1;
        pthread_setschedparam(0, SCHED_RR, &stParam);
    }
    while(1)
    {
        fprintf(stderr, "Bridge thread start\n");

        clBridgeLock.LockThread(this_thread, &stBridgeSem);

        fprintf(stderr, "Processing timer\n");
        TimerScheduler::Process();
#if KERNEL_USE_QUANTUM
        Quantum::UpdateTimer();
#endif
    }
}

//---------------------------------------------------------------------------
static void StartBridge()
{
    sem_init(&stBridgeSem, 0, 0);

    clBridgeThread.Init(aucBridgeStack, 128, 7, BridgeThread, NULL);
    clBridgeThread.Start();
}

//---------------------------------------------------------------------------
void ThreadPort::SaveContext()
{
    // This has to be called from the context of the thread being preempted.
    K_BOOL bChosen = false;
    K_BOOL bFirst = true;
    Thread *pstMyThread = g_pstCurrent;

    // Wait until we're chosen as the next thread to run again.
    while(!bChosen) 
    {
        // Wait until we can get the mutex...
        pthread_mutex_lock( &stMutex );

        // Post interrupt-thread semaphore the first time through only
        if( bFirst ) 
        {
            sem_post(&stKSem);
            bFirst = false;
        }

        // Sleep until we get a condvar broadcast
        pthread_cond_wait( &stCond, &stMutex );

        // If we're the chosen thread, get back to work, otherwise
        if( pstChosen == pstMyThread ) 
        {
            bChosen = true;
        }

        // Always unlock the mutex.
        pthread_mutex_unlock( &stMutex );
    }
}

//---------------------------------------------------------------------------
void ThreadPort::RestoreContext()
{   
    // Change condition variable and pend on interrupt-thread semaphore.
    pthread_mutex_lock( &stMutex );
    pstChosen = g_pstCurrent;
    //fprintf(stderr, "CS: %x\n", pstChosen->GetID());
    pthread_cond_broadcast( &stCond );

    pthread_mutex_unlock( &stMutex );
    fprintf(stderr, "Restored thread: %d\n", syscall(SYS_gettid) );
}

//------------------------------------------------------------------------
void ThreadPort::InitStack(Thread *pstThread_)
{
    // Create the pthread here, set the entry function to ThreadInternalEntry,
    // Passing in the thread as the arg.  Use default thread priority.
    pthread_attr_t stAttr;
    pthread_t dummy;
    struct sched_param stParam;

    if( !bInit ){
        Thread_InitInternal();              // Configure the internal wrapper data
        bInit = true;
        StartBridge();
    }

    pthread_attr_init(&stAttr);

    memset( (void*)&stParam, 0, sizeof(struct sched_param));

    stParam.sched_priority = sched_get_priority_max( SCHED_RR ) - 2;

    pthread_attr_setschedparam( &stAttr, &stParam );

    bIsThreadUp = false;
    pthread_create(&dummy, &stAttr, ThreadPort::InternalEntry, (void*)pstThread_);
    while (!bIsThreadUp) {}
}

//---------------------------------------------------------------------------
void ThreadPort::StartThreads()
{
    if( !bInit ){
        Thread_InitInternal();              // Configure the internal wrapper data
        bInit = true;
    }

    Thread_StartInterrupts();          // Start the internal interrupt functions

    KernelSWI::Config();               // configure the task switch SWI
    KernelTimer::Config();             // configure the kernel timer
    KernelTimer::Start();              // enable the kernel timer
    KernelSWI::Start();                // enable the task switch SWI

    Scheduler::SetScheduler(true);      // enable the scheduler
    Scheduler::Schedule();              // run the scheduler

    g_pstCurrent = g_pstNext;

    // Restore the context...
    ThreadPort::RestoreContext();          // restore the context of the first running thread

    // Sleep forever
    while(1){sleep(3600);}
}

//---------------------------------------------------------------------------
void ThreadriggerSwitch(void)
{
    ThreadPort::SaveContext();        // Push the context (registers) of the current task
}

//---------------------------------------------------------------------------
static void Thread_InitInternal( void )
{
    // Initialize the various objects used internally.
    pthread_cond_init(&stCond, NULL);
    pthread_mutex_init(&stMutex, NULL);
    sem_init(&stKSem, 0, 0);    
}
//---------------------------------------------------------------------------
void *ThreadPort::InternalEntry(void *pstThread_)
{
    // We want to start our threads in a suspended state, to ensure that the
    // threads sleep on creation until the first time scheduled, at which
    // point we call the thread's actual entrypoint function.
    K_BOOL bChosen = false;
    Thread *pclThread = static_cast<Thread*>(pstThread_);
    bIsThreadUp = true;
    while(!bChosen) 
    {
        // Wait until we can get the mutex...
        pthread_mutex_lock( &stMutex );

        // Sleep until we get a condvar broadcast
        pthread_cond_wait( &stCond, &stMutex );

        // If we're the chosen thread, get back to work, otherwise
        if( pstChosen == pstThread_ ) 
        {
            bChosen = true;
        }

        // Always unlock the mutex
        pthread_mutex_unlock( &stMutex );
    }

    // Alright, we can start the thread.

    pclThread->m_pfEntryPoint(pclThread->m_pvArg);
    pthread_exit(0);
    return NULL;
}

//---------------------------------------------------------------------------
static void Thread_StartInterrupts( void )
{
    // create a pthread at the interrupt priority level, pointing to the 
    // thread interrupts function.
    pthread_attr_t stAttr;
    struct sched_param stParam;
    pthread_t dummy;
    pthread_attr_init(&stAttr);

    memset( (void*)&stParam, 0, sizeof(struct sched_param));

    stParam.sched_priority = sched_get_priority_max( SCHED_RR ) - 1;

    pthread_attr_setschedparam( &stAttr, &stParam );

    pthread_create(&dummy, &stAttr, ThreadPort::Interrupts, NULL);

}

//---------------------------------------------------------------------------
void *ThreadPort::Interrupts( void* unused_ )
{
    // create a pthread at the interrupt priority level.
    while(1)
    {
        // wait for the semaphore to be posted
        bInterrupt = false;
        sem_wait( &stKSem );
        bInterrupt = true;
        fprintf(stderr, "In interrupt\n");
        if ( bIntEnabled )
        {
            // process ints.
            if( usIntFlags & FLAG_SWI )
            {
                fprintf(stderr, "SWI Int Ack\n");
                usIntFlags &= ~FLAG_SWI;
                Scheduler::Schedule();
                g_pstCurrent = g_pstNext;
                ThreadPort::RestoreContext();
                fprintf(stderr, "End of Ack\n");
            }
            if( usIntFlags & FLAG_BRIDGE )
            {
                fprintf(stderr, "posting\n");
                sem_post(&stBridgeSem);
                usIntFlags &= ~FLAG_BRIDGE;

            }
            if( usIntFlags & FLAG_TIMER )
            {
                fprintf(stderr, "Timer Int Ack\n");
                usIntFlags &= ~FLAG_TIMER;
                clBridgeLock.UnlockThread();
                fprintf(stderr, "End of ACK\n");
            }

        } else {
            fprintf(stderr, "Trigger int\n");
            bTriggerInt = true;
        }
        fprintf(stderr, "Done INT\n");
    }
}
