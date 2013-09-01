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

    \file   threadport.h    

    \brief  Linux Multithreading support.
*/

#ifndef __THREADPORT_H_
#define __THREADPORT_H_

#include "kerneltypes.h"
#include "thread.h"

#include <semaphore.h>
#include <pthread.h>

extern volatile K_USHORT usIntFlags;

#define FLAG_SWI        (0x0001)
#define FLAG_TIMER      (0x0002)
#define FLAG_BRIDGE     (0x0004)
#define TOP_OF_STACK(x, y)            (x)

extern volatile K_BOOL bIntEnabled;
extern volatile K_BOOL bInterrupt;
extern volatile K_BOOL bSwitchOnCS;
extern volatile K_BOOL bTriggerInt;
extern sem_t stKSem;

//------------------------------------------------------------------------
//! These macros *must* be used in pairs !
//------------------------------------------------------------------------
//! Enter critical section (copy status register, disable interrupts)

#define CS_ENTER() \
{ \
    struct sched_param stParam___; \
    int iPolicy___; \
    pthread_getschedparam(0, &iPolicy___, &stParam___ ); \
    stParam___.sched_priority++; \
    pthread_setschedparam(0, iPolicy___, &stParam___);

#define CS_EXIT() \
    stParam___.sched_priority--; \
    pthread_setschedparam(0, iPolicy___, &stParam___); \
}

//------------------------------------------------------------------------
//! Initiate a contex switch without using the SWI
#define ENABLE_INTS()
#define DISABLE_INTS()

//------------------------------------------------------------------------
class Thread;
/*!
    Class defining the architecture specific functions required by the 
    kernel.  
    
    This is limited (at this point) to a function to start the scheduler,
    and a function to initialize the default stack-frame for a thread.
*/
class ThreadPort
{
public:
    /*!        
        \fn void StartThreads()
       
        Function to start the scheduler, initial threads, etc.
    */
    static void StartThreads();
    friend class Thread;

    static void SaveContext();

    static void RestoreContext();

    static void *Interrupts(void *unused_);

    static void *InternalEntry(void *pstThread_);
private:

    /*!
        \fn void InitStack(Thread *pstThread_)
        
        Initialize the thread's stack.
        
        \param pstThread_ Pointer to the thread to initialize
    */
    static void InitStack(Thread *pstThread_);


};

#endif //__ThreadPORT_H_
