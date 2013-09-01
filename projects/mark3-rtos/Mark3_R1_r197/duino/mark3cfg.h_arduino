/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file mark3cfg.h
    
    \brief Mark3 Kernel Configuration
    
	This file is used to configure the kernel for your specific application
	in order to provide the optimal set of features for a given use case.

	Since you only pay the price (code space/RAM) for the features you use,
	you can usually find a sweet spot between features and resource usage
	by picking and choosing features a-la-carte.  This config file is
	written in an "interactive" way, in order to minimize confusion about
	what each option provides, and to make dependencies obvious.
    
    As of 7.6.2012 on AVR, these are the costs associated with the various
    features:

    Base Kernel:        2888 bytes
    Tickless Timers:    1194 bytes
    Semaphores:         224 bytes
    Message Queues:     332 bytes (+ Semaphores)
    Mutexes:            290 bytes
    Thread Sleep:       162 bytes (+ Semaphores/Timers)
    Round-Robin:        304 bytes (+ Timers)
    Drivers:            144 bytes
	Dynamic Threads:	68 bytes
    Thread Names:       8 bytes
	Profiling Timers:	624 bytes
*/

#ifndef __MARK3CFG_H__
#define __MARK3CFG_H__

/*!
	The following options is related to all kernel time-tracking.

	-timers provide a way for events to be periodically triggered in a 
	 lightweight manner.  These can be periodic, or one-shot.

    -Thread Quantum (used for round-robin scheduling) is dependent on this
     module, as is Thread Sleep functionality.
*/
#define KERNEL_USE_TIMERS               (1)

/*!
	Do you want to enable time quanta?  This is useful when you want to have
	tasks in the same priority group share time in a controlled way.  This
	allows equal tasks to use unequal amounts of the CPU, which is a great 
	way to set up CPU budgets per thread in a round-robin scheduling system.
	If enabled, you can specify a number of ticks that serves as the default 
	time period (quantum).  Unless otherwise specified, every thread in a 
	priority will get the default quantum.
*/
#if KERNEL_USE_TIMERS    
    #define KERNEL_USE_QUANTUM          (1)
#else
    #define KERNEL_USE_QUANTUM          (0)
#endif

/*!
	Do you want the ability to use counting/binary semaphores for thread 
	synchronization?  Enabling this features provides fully-blocking semaphores
	and enables all API functions declared in semaphore.h.  If you have to 
	pick one blocking mechanism, this is the one to choose.  By also enabling 
	per-thread semaphores, each thread will receive it's own built-in semaphore. 
*/
#define KERNEL_USE_SEMAPHORE			(1)

/*!
	Enable inter-thread messaging using named mailboxes.  
	If per-thread mailboxes are defined, each thread is allocated a default
	mailbox of a depth specified by THREAD_MAILBOX_SIZE.  
*/
#if KERNEL_USE_SEMAPHORE				
    #define KERNEL_USE_MESSAGE          (1)
#else
    #define KERNEL_USE_MESSAGE          (0)
#endif

/*!
    If Messages are enabled, define the size of the default kernel message
    pool.  Messages can be manually added to the message pool, but this
    mechansims is more convenient and automatic.
*/
#if KERNEL_USE_MESSAGE
    #define GLOBAL_MESSAGE_POOL_SIZE    (8)
#endif

/*!
	Do you want the ability to use mutual exclusion semaphores (mutex) for 
	resource/block protection?  Enabling this feature provides mutexes, with
	priority inheritence, as declared in mutex.h.  Enabling per-thread mutex
	automatically allocates a mutex for each thread.
 */
#define KERNEL_USE_MUTEX                (1)

/*!
	Do you want to be able to set threads to sleep for a specified time?
	This enables the Thread::Sleep() API.
*/
#if KERNEL_USE_TIMERS && KERNEL_USE_SEMAPHORE
    #define KERNEL_USE_SLEEP            (1)	
#else
    #define KERNEL_USE_SLEEP            (0)
#endif


/*!
	Enabling device drivers provides a posix-like filesystem interface for 
	peripheral device drivers.  When enabled, the size of the filesystem 
	table is specified in DRIVER_TABLE_SIZE.  Permissions are enforced for 
	driver access by thread ID and group when DRIVER_USE_PERMS are enabled.
*/
#define KERNEL_USE_DRIVER               (0)

/*!
	Provide Thread method to allow the user to set a name for each
	thread in the system.  Adds to the size of the thread member 
	data.
*/
#define KERNEL_USE_THREADNAME			(1)

/*!
	Provide extra Thread methods to allow the application to create
	(and more importantly destroy) threads at runtime.  Useful for
	designs implementing worker threads, or threads that can be restarted
	after encountering error conditions.
*/
#define KERNEL_USE_DYNAMIC_THREADS		(1)

/*!
	Provides extra classes for profiling the performance of code.  Useful
	for debugging and development, but uses an additional timer.
*/
#define KERNEL_USE_PROFILER				(0)

/*!
	Provides extra logic for kernel debugging, and instruments the kernel 
	with extra asserts, and kernel trace functionality.
*/
#define KERNEL_USE_DEBUG				(0)

#define KERNEL_USE_EVENTFLAG            (1)

#endif
