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

    \file   kernel.h    

    \brief  Kernel initialization and startup class
    
    The Kernel namespace provides functions related to initializing and 
    starting up the kernel.
    
    The Kernel::Init() function must be called before any of the other
    functions in the kernel can be used.
    
    Once the initial kernel configuration has been completed (i.e. first 
    threads have been added to the scheduler), the Kernel::Start() 
    function can then be called, which will transition code execution from
    the "main()" context to the threads in the scheduler.    
*/

#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "kerneltypes.h"
#include "panic_codes.h"

//---------------------------------------------------------------------------
/*!
	Class that encapsulates all of the kernel startup functions.
*/
class Kernel
{
public:    
    /*!
	    Kernel Initialization Function, call before any other OS function
        
		\fn Init()
        
        Initializes all global resources used by the operating system.  This 
        must be called before any other kernel function is invoked.
    */
	static void Init(void);
        
    /*!
		Start the kernel; function never returns
	
        \fn Start()
        
        Start the operating system kernel - the current execution context is
        cancelled, all kernel services are started, and the processor resumes
        execution at the entrypoint for the highest-priority thread.

        You must have at least one thread added to the kernel before calling
        this function, otherwise the behavior is undefined.
    */
	static void Start(void);	

    /*!
      \brief IsStarted
      \return Whether or not the kernel has started - true = running, false =
              not started
     */
    static bool IsStarted()    {   return m_bIsStarted;    }

    /*!
     * \brief SetPanic Set a function to be called when a kernel panic occurs,
     *        giving the user to determine the behavior when a catastrophic
     *        failure is observed.
     *
     * \param pfPanic_ Panic function pointer
     */
    static void SetPanic( panic_func_t pfPanic_ ) { m_pfPanic = pfPanic_; }

    /*!
     * \brief IsPanic Returns whether or not the kernel is in a panic state
     * \return Whether or not the kernel is in a panic state
     */
    static bool IsPanic()      {   return m_bIsPanic;   }

    /*!
     * \brief Panic Cause the kernel to enter its panic state
     * \param usCause_ Reason for the kernel panic
     */
    static void Panic(K_USHORT usCause_);

private:
    static bool m_bIsStarted;       //!< true if kernel is running, false otherwise
    static bool m_bIsPanic;         //!< true if kernel is in panic state, false otherwise
    static panic_func_t m_pfPanic;  //!< user-set panic function
};

#endif

