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
	\file debug_tokens.h
	\brief Hex codes/translation tables used for efficient string tokenization.
		
	We use this for efficiently encoding strings used for kernel traces,
	debug prints, etc.  The upside - this is really fast and efficient for
	encoding strings and data.  Downside?  The tools need to parse this
	header file in order to convert the enumerated data into actual 
	strings, decoding them.
*/

#ifndef __DEBUG_TOKENS_H__
#define __DEBUG_TOKENS_H__
//---------------------------------------------------------------------------
/*! Source file names start at 0x0000 */
#define BLOCKING_CPP		0x0001		/* SUBSTITUTE="blocking.cpp" */
#define DRIVER_CPP			0x0002		/* SUBSTITUTE="driver.cpp" */
#define KERNEL_CPP			0x0003		/* SUBSTITUTE="kernel.cpp" */
#define LL_CPP				0x0004		/* SUBSTITUTE="ll.cpp" */
#define MESSAGE_CPP			0x0005		/* SUBSTITUTE="message.cpp" */
#define MUTEX_CPP			0x0006		/* SUBSTITUTE="mutex.cpp" */
#define PROFILE_CPP			0x0007		/* SUBSTITUTE="profile.cpp" */
#define QUANTUM_CPP			0x0008		/* SUBSTITUTE="quantum.cpp" */
#define SCHEDULER_CPP		0x0009		/* SUBSTITUTE="scheduler.cpp" */
#define SEMAPHORE_CPP		0x000A		/* SUBSTITUTE="semaphore.cpp" */
#define THREAD_CPP			0x000B		/* SUBSTITUTE="thread.cpp" */
#define THREADLIST_CPP		0x000C		/* SUBSTITUTE="threadlist.cpp" */
#define TIMERLIST_CPP		0x000D		/* SUBSTITUTE="timerlist.cpp" */
#define KERNELSWI_CPP		0x000E		/* SUBSTITUTE="kernelswi.cpp" */
#define KERNELTIMER_CPP		0x000F		/* SUBSTITUTE="kerneltimer.cpp" */
#define KPROFILE_CPP		0x0010		/* SUBSTITUTE="kprofile.cpp" */
#define THREADPORT_CPP		0x0011		/* SUBSTITUTE="threadport.cpp" */

//---------------------------------------------------------------------------
/*! Header file names start at 0x1000 */
#define BLOCKING_H			0x1000		/* SUBSTITUTE="blocking.h" */
#define DRIVER_H			0x1001		/* SUBSTITUTE="driver.h" */
#define KERNEL_H			0x1002		/* SUBSTITUTE="kernel.h" */
#define KERNELTYPES_H		0x1003		/* SUBSTITUTE="kerneltypes.h" */
#define LL_H				0x1004		/* SUBSTITUTE="ll.h" */
#define MANUAL_H			0x1005		/* SUBSTITUTE="manual.h" */
#define MARK3CFG_H			0x1006		/* SUBSTITUTE="mark3cfg.h" */
#define MESSAGE_H			0x1007		/* SUBSTITUTE="message.h" */
#define MUTEX_H				0x1008		/* SUBSTITUTE="mutex.h" */
#define PROFILE_H			0x1009		/* SUBSTITUTE="profile.h" */
#define PROFILING_RESULTS_H	0x100A		/* SUBSTITUTE="profiling_results.h" */
#define QUANTUM_H			0x100B		/* SUBSTITUTE="quantum.h" */
#define SCHEDULER_H			0x100C		/* SUBSTITUTE="scheduler.h" */
#define SEMAPHORE_H			0x100D		/* SUBSTITUTE="ksemaphore.h" */
#define THREAD_H			0x100E		/* SUBSTITUTE="thread.h" */
#define THREADLIST_H		0x100F		/* SUBSTITUTE="threadlist.h" */
#define TIMERLIST_H			0x1010		/* SUBSTITUTE="timerlist.h" */
#define KERNELSWI_H			0x1011		/* SUBSTITUTE="kernelswi.h */
#define KERNELTIMER_H		0x1012		/* SUBSTITUTE="kerneltimer.h */
#define KPROFILE_H			0x1013		/* SUBSTITUTE="kprofile.h" */
#define THREADPORT_H		0x1014		/* SUBSTITUTE="threadport.h" */

//---------------------------------------------------------------------------
/*! Indexed strings start at 0x2000 */
#define STR_PANIC				0x2000		/* SUBSTITUTE="!Panic!" */
#define STR_MARK3_INIT			0x2001		/* SUBSTITUTE="Initializing Kernel Objects" */
#define STR_KERNEL_ENTER		0x2002		/* SUBSTITUTE="Starting Kernel" */
#define STR_THREAD_START		0x2003		/* SUBSTITUTE="Switching to First Thread" */
#define STR_START_ERROR			0x2004		/* SUBSTITUTE="Error starting kernel - function should never return" */
#define STR_THREAD_CREATE		0x2005		/* SUBSTITUTE="Creating Thread" */
#define STR_STACK_SIZE_1 		0x2006		/* SUBSTITUTE="  Stack Size: %1" */
#define STR_PRIORITY_1	 		0x2007		/* SUBSTITUTE="  Priority: %1" */
#define STR_THREAD_ID_1	 		0x2008		/* SUBSTITUTE="  Thread ID: %1" */
#define STR_ENTRYPOINT_1	 	0x2009		/* SUBSTITUTE="  EntryPoint: %1" */
#define STR_CONTEXT_SWITCH_1	0x200A		/* SUBSTITUTE="Context Switch To Thread: %1" */
#define STR_IDLING				0x200B		/* SUBSTITUTE="Idling CPU" */
#define STR_WAKEUP				0x200C		/* SUBSTITUTE="Waking up" */
#define STR_SEMAPHORE_PEND_1	0x200D		/* SUBSTITUTE="Semaphore Pend: %1" */
#define STR_SEMAPHORE_POST_1	0x200E		/* SUBSTITUTE="Semaphore Post: %1" */
#define STR_MUTEX_CLAIM_1		0x200F		/* SUBSTITUTE="Mutex Claim: %1" */
#define STR_MUTEX_RELEASE_1		0x2010		/* SUBSTITUTE="Mutex Release: %1" */
#define STR_THREAD_BLOCK_1		0x2011		/* SUBSTITUTE="Thread %1 Blocked" */
#define STR_THREAD_UNBLOCK_1	0x2012-2015		/* SUBSTITUTE="Thread %1 Unblocked" */
#define STR_ASSERT_FAILED		0x2013		/* SUBSTITUTE="Assertion Failed" */
#define STR_SCHEDULE_1			0x2014		/* SUBSTITUTE="Scheduler chose %1" */
#define STR_THREAD_START_1		0x2015		/* SUBSTITUTE="Thread Start: %1" */
#define STR_THREAD_EXIT_1		0x2016		/* SUBSTITUTE="Thread Exit: %1" */

//---------------------------------------------------------------------------
#define STR_UNDEFINED			0xFFFF		/* SUBSTITUTE="UNDEFINED" */
#endif
