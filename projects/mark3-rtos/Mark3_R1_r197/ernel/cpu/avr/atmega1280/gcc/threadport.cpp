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

    \file   threadport.cpp   

    \brief  ATMega328p Multithreading

*/

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "thread.h"
#include "threadport.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "timerlist.h"
#include "quantum.h"
#include <avr/io.h>
#include <avr/interrupt.h>

//---------------------------------------------------------------------------
Thread *g_pstCurrentThread;

//---------------------------------------------------------------------------
void ThreadPort::InitStack(Thread *pclThread_)
{
    // Initialize the stack for a Thread
    K_USHORT usAddr;
    K_UCHAR *pucStack;
    K_USHORT i;

    // Get the address of the thread's entry function
    usAddr = (K_USHORT)(pclThread_->m_pfEntryPoint);

    // Start by finding the bottom of the stack
    pucStack = (K_UCHAR*)pclThread_->m_pwStackTop;

    // clear the stack, and initialize it to a known-default value (easier
    // to debug when things go sour with stack corruption or overflow)
    for (i = 0; i < pclThread_->m_usStackSize; i++)
    {
        pclThread_->m_pwStack[i] = 0xFF;
    }

    // Our context starts with the entry function
    PUSH_TO_STACK(pucStack, (K_UCHAR)(usAddr & 0x00FF));
    PUSH_TO_STACK(pucStack, (K_UCHAR)((usAddr >> 8) & 0x00FF));

    // R0
    PUSH_TO_STACK(pucStack, 0x00);   // R0

    // Push status register and R1 (which is used as a constant zero)
    PUSH_TO_STACK(pucStack, 0x80);  // SR
    PUSH_TO_STACK(pucStack, 0x00);  // R1

    // Push other registers
    for (i = 2; i <= 23; i++) //R2-R23
    {
        PUSH_TO_STACK(pucStack, i);
    }

    // Assume that the argument is the only stack variable
    PUSH_TO_STACK(pucStack, (K_UCHAR)(((K_USHORT)(pclThread_->m_pvArg)) & 0x00FF));    //R24
    PUSH_TO_STACK(pucStack, (K_UCHAR)((((K_USHORT)(pclThread_->m_pvArg))>>8) & 0x00FF)); //R25

    // Push the rest of the registers in the context
    for (i = 26; i <=31; i++)
    {
        PUSH_TO_STACK(pucStack, i);
    }
    
    // Push the RAMPZ and EIND registers
    PUSH_TO_STACK(pucStack, 'z');
    PUSH_TO_STACK(pucStack, 'd');

    // Set the top o' the stack.
    pclThread_->m_pwStackTop = (K_UCHAR*)pucStack;

    // That's it!  the thread is ready to run now.
}

//---------------------------------------------------------------------------
static void Thread_Switch(void)
{
    g_pstCurrent = g_pstNext;
}


//---------------------------------------------------------------------------
void ThreadPort::StartThreads()
{
    KernelSWI::Config();                 // configure the task switch SWI
    KernelTimer::Config();                 // configure the kernel timer
    
    Scheduler::SetScheduler(1);               // enable the scheduler
    Scheduler::Schedule();                 // run the scheduler - determine the first thread to run

    Thread_Switch();                     // Set the next scheduled thread to the current thread

    KernelTimer::Start();                 // enable the kernel timer
    KernelSWI::Start();                     // enable the task switch SWI

    // Restore the context...
    Thread_RestoreContext();        // restore the context of the first running thread
    ASM("reti");                    // return from interrupt - will return to the first scheduled thread
}

//---------------------------------------------------------------------------
/*!
    SWI using INT0 - used to trigger a context switch
    \fn ISR(INT0_vect) __attribute__ ( ( signal, naked ) );
*/
//---------------------------------------------------------------------------
ISR(INT0_vect) __attribute__ ( ( signal, naked ) );
ISR(INT0_vect)
{
    Thread_SaveContext();        // Push the context (registers) of the current task
    Thread_Switch();            // Switch to the next task
    Thread_RestoreContext();    // Pop the context (registers) of the next task
    ASM("reti");                // Return to the next task
}

//---------------------------------------------------------------------------
/*!
    Timer interrupt ISR - causes a tick, which may cause a context switch
    \fn ISR(TIMER1_COMPA_vect) ;
*/
//---------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect)
{
#if KERNEL_USE_TIMERS    
    TimerScheduler::Process();
#endif    
#if KERNEL_USE_QUANTUM    
    Quantum::UpdateTimer();
#endif    
}
