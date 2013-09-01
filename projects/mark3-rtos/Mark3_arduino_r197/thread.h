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

    \file   thread.h    

    \brief  Platform independent thread class declarations
    
    Threads are an atomic unit of execution, and each instance of the thread
    class represents an instance of a program running of the processor.
    The Thread is the fundmanetal user-facing object in the kernel - it
    is what makes multiprocessing possible from application code.
    
    In Mark3, threads each have their own context - consisting of a stack, and
    all of the registers required to multiplex a processor between multiple 
    threads.
    
    The Thread class inherits directly from the LinkListNode class to 
    facilitate efficient thread management using Double, or Double-Circular
    linked lists.
    
*/

#ifndef __THREAD_H__
#define __THREAD_H__

#include "kerneltypes.h"
#include "mark3cfg.h"

#include "ll.h"
#include "threadlist.h"
#include "scheduler.h"
#include "threadport.h"
#include "quantum.h"

//---------------------------------------------------------------------------
/*!
    Function pointer type used for thread entrypoint functions
*/
typedef void (*ThreadEntry_t)(void *pvArg_);

//---------------------------------------------------------------------------
/*!
    Object providing fundamental multitasking support in the kernel.
*/
class Thread : public LinkListNode
{
public:    
    /*!
        \fn void Init(K_UCHAR *paucStack_, 
              K_USHORT usStackSize_,
              K_UCHAR ucPriority_,              
              ThreadEntry_t pfEntryPoint_,
              void *pvArg_ );
              
        Initialize a thread prior to its use.  Initialized threads are
        placed in the stopped state, and are not scheduled until the 
        thread's start method has been invoked first.
        
        \param paucStack_    Pointer to the stack to use for the thread
        \param usStackSize_  Size of the stack (in bytes)
        \param ucPriority_   Priority of the thread (0 = idle, 7 = max)
        \param pfEntryPoint_ This is the function that gets called when the
                             thread is started
        \param pvArg_        Pointer to the argument passed into the thread's
                             entrypoint function.
    */
    void Init(K_WORD *paucStack_, 
              K_USHORT usStackSize_,
              K_UCHAR ucPriority_,
              ThreadEntry_t pfEntryPoint_,
              void *pvArg_ );

    /*!
        \fn void Start()
       
        Start the thread - remove it from the stopped list, add it to the
        scheduler's list of threads (at the thread's set priority), and
        continue along.       
    */              
    void Start();    
    

    /*!
        \fn void Stop()

        Stop a thread that's actively scheduled without destroying its
        stacks.  Stopped threads can be restarted using the Start() API.
    */
    void Stop();

#if KERNEL_USE_THREADNAME    
    /*!
        \fn void SetName(const K_CHAR *szName_)
        
        Set the name of the thread - this is purely optional, but can be 
        useful when identifying issues that come along when multiple threads
        are at play in a system.
        
        \param szName_ Char string containing the thread name
    */
    void SetName(const K_CHAR *szName_) { m_szName = szName_; }

    /*!
        \fn const K_CHAR* GetName()

        \return Pointer to the name of the thread.  If this is not set, 
                will be NULL.
    */
    const K_CHAR* GetName() { return m_szName; }
#endif
    
    /*!
        \fn ThreadList *GetOwner(void)
        
        Return the ThreadList where the thread belongs when it's in the 
        active/ready state in the scheduler.
        
        \return Pointer to the Thread's owner list
    */
    ThreadList *GetOwner(void) { return m_pclOwner; }
    
    /*!        
        \fn ThreadList *GetCurrent(void)
        
        Return the ThreadList where the thread is currently located
        
        \return Pointer to the thread's current list        
    */
    ThreadList *GetCurrent(void) { return m_pclCurrent; }
    
    /*!
        \fn K_UCHAR GetPriority(void)
        
        Return the priority of the current thread
        
        \return Priority of the current thread
    */

    K_UCHAR GetPriority(void) { return m_ucPriority; }

    /*!
        \fn K_UCHAR GetCurPriority(void)
        
        Return the priority of the current thread
        
        \return Priority of the current thread
    */
    K_UCHAR GetCurPriority(void) { return m_ucCurPriority; }
    
#if KERNEL_USE_QUANTUM    
    /*!
        \fn void SetQuantum( K_USHORT usQuantum_ )
        
        Set the thread's round-robin execution quantum.
        
        \param usQuantum_ Thread's execution quantum (in milliseconds)
    */
    void SetQuantum( K_USHORT usQuantum_ ) { m_usQuantum = usQuantum_; }

    /*!
        \fn K_USHORT GetQuantum(void)
        
        Get the thread's round-robin execution quantum.
        
        \return The thread's quantum
    */
    K_USHORT GetQuantum(void) { return m_usQuantum; }
#endif

    /*!
        \fn void SetCurrent( ThreadList *pclNewList_ )
        
        Set the thread's current to the specified thread list
        
        \param pclNewList_ Pointer to the threadlist to apply thread ownership       
    */
    void SetCurrent( ThreadList *pclNewList_ ) { m_pclCurrent = pclNewList_; }
    
    /*!
        \fn void SetOwner( ThreadList *pclNewList_ )
        
        Set the thread's owner to the specified thread list
        
        \param pclNewList_ Pointer to the threadlist to apply thread ownership       
    */
    void SetOwner( ThreadList *pclNewList_ ) { m_pclOwner = pclNewList_; }
    
    
    /*!
        \fn void SetPriority( K_UCHAR ucPriority_ )
        
        Set the priority of the Thread (running or otherwise) to a different
        level.  This activity involves re-scheduling, and must be done so 
        with due caution, as it may effect the determinism of the system.
        
        This should *always* be called from within a critical section to
        prevent system issues.
        
        \param ucPriority_ New priority of the thread
    */
    void SetPriority(K_UCHAR ucPriority_);
    
    /*!
        \fn void InheritPriority(K_UCHAR ucPriority_)
        
        Allow the thread to run at a different priority level (temporarily)
        for the purpose of avoiding priority inversions.  This should
        only be called from within the implementation of blocking-objects.
        
        \param ucPriority_  New Priority to boost to.        
    */
    void InheritPriority(K_UCHAR ucPriority_); 
    
#if KERNEL_USE_DYNAMIC_THREADS    
    /*!
        \fn void Exit()
        
        Remove the thread from being scheduled again.  The thread is 
        effectively destroyed when this occurs.  This is extremely 
        useful for cases where a thread encounters an unrecoverable
        error and needs to be restarted, or in the context of systems
        where threads need to be created and destroyed dynamically.
        
        This must not be called on the idle thread.
    */    
    void Exit();
#endif    

#if KERNEL_USE_SLEEP    
    /*!
        \fn void Sleep(K_ULONG ulTimeMs_);
        
        Put the thread to sleep for the specified time (in milliseconds).
        Actual time slept may be longer (but not less than) the interval specified.
        
        \param ulTimeMs_ Time to sleep (in ms)
    */
    static void Sleep(K_ULONG ulTimeMs_);

    /*!
        \fn void USleep(K_ULONG ulTimeUs_);

        Put the thread to sleep for the specified time (in microseconds).
        Actual time slept may be longer (but not less than) the interval specified.

        \param ulTimeUs_ Time to sleep (in microseconds)
    */
    static void USleep(K_ULONG ulTimeUs_);
#endif
    
    /*!
        \fn void Yield(void)
        
        Yield the thread - this forces the system to call the scheduler and 
        determine what thread should run next.  This is typically used when
        threads are moved in and out of the scheduler.
    */
    static void Yield(void);
    
    /*!
        \fn void SetID( K_UCHAR ucID_ )
        
        Set an 8-bit ID to uniquely identify this thread.
        
        \param ucID_ 8-bit Thread ID, set by the user
    */
    void SetID( K_UCHAR ucID_ ) { m_ucThreadID = ucID_; }
    
    /*!
        \fn K_UCHAR GetID()
        
        Return the 8-bit ID corresponding to this thread.
        
        \return Thread's 8-bit ID, set by the user
    */
    K_UCHAR GetID() { return m_ucThreadID; }
    
    
    /*!
        \fn K_USHORT GetStackSlack()
        
        Performs a (somewhat lengthy) check on the thread stack to check the
        amount of stack margin (or "slack") remaining on the stack. If you're
        having problems with blowing your stack, you can run this function 
        at points in your code during development to see what operations 
        cause problems.  Also useful during development as a tool to optimally 
        size thread stacks.
        
        \return The amount of slack (unused bytes) on the stack
    */
    K_USHORT GetStackSlack();
    
#if KERNEL_USE_EVENTFLAG
    /*!
        \brief GetEventFlagMask returns the thread's current event-flag mask,
              which is used in conjunction with the EventFlag blocking object
              type.

        \return A copy of the thread's event flag mask
     */
    K_USHORT GetEventFlagMask() { return m_usFlagMask; }

    /*!
        \brief SetEventFlagMask Sets the active event flag bitfield mask
        \param usMask_
     */
    void SetEventFlagMask(K_USHORT usMask_) { m_usFlagMask = usMask_; }

    /*!
     * \brief SetEventFlagMode Sets the active event flag operation mode
     * \param eMode_ Event flag operation mode, defines the logical operator
     *               to apply to the event flag.
     */
    void SetEventFlagMode(EventFlagOperation_t eMode_ ) { m_eFlagMode = eMode_; }

    /*!
     * \brief GetEventFlagMode Returns the thread's event flag's operating mode
     * \return The thread's event flag mode.
     */
    EventFlagOperation_t GetEventFlagMode() { return m_eFlagMode; }
#endif

#if KERNEL_USE_TIMERS
	/*!
		Return a pointer to the thread's timer object
	*/
	Timer *GetTimer();
	void SetExpired( K_BOOL bExpired_ );
	K_BOOL GetExpired();
#endif

    friend class ThreadPort;
    
private:
    /*!
        \fn void ContextSwitchSWI()
       
        This code is used to trigger the context switch interrupt.  Called 
        whenever the kernel decides that it is necessary to swap out the 
        current thread for the "next" thread.       
    */
    static void ContextSwitchSWI(void);

    /*!
        \fn void SetPriorityBase(K_UCHAR ucPriority_)
        \param ucPriority_
     */
    void SetPriorityBase(K_UCHAR ucPriority_);
    
    //! Pointer to the top of the thread's stack
    K_WORD *m_pwStackTop;
    
    //! Pointer to the thread's stack
    K_WORD *m_pwStack;

    //! Size of the stack (in bytes)
    K_USHORT m_usStackSize;   

#if KERNEL_USE_QUANTUM    
    //! Thread quantum (in milliseconds)
    K_USHORT m_usQuantum;     
#endif
    
    //! Thread ID
    K_UCHAR m_ucThreadID;
    
    //! Default priority of the thread
    K_UCHAR m_ucPriority;     
    
    //! Current priority of the thread (priority inheritence)
    K_UCHAR m_ucCurPriority;  
    
    //! The entry-point function called when the thread starts
    ThreadEntry_t m_pfEntryPoint;
    
    //! Pointer to the argument passed into the thread's entrypoint
    void *m_pvArg;

#if KERNEL_USE_THREADNAME
    //! Thread name
    const K_CHAR *m_szName;
#endif    

#if KERNEL_USE_EVENTFLAG
    //! Event-flag mask
    K_USHORT m_usFlagMask;

    //! Event-flag mode
    EventFlagOperation_t m_eFlagMode;
#endif

#if KERNEL_USE_TIMERS
	//! Timer used for blocking-object timeouts
	Timer	m_clTimer;
	K_BOOL	m_bExpired;
#endif
    
    //! Pointer to the thread-list where the thread currently resides
    ThreadList *m_pclCurrent;
    
    //! Pointer to the thread-list where the thread resides when active
    ThreadList *m_pclOwner;
};

#endif
