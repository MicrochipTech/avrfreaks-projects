//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file:  kernelcfg.h

	Description:
		Kernel configuration module - used for customizing global kernel options
		at compile-time.  For module-specific functionality, see the header
		file for each individual kernel module.
*/
//---------------------------------------------------------------------------
#ifndef __KERNELCFG_H_
#define __KERNELCFG_H_

//===========================================================================
// Kernel feature configuration
//===========================================================================

#define KERNEL_STACK_DEBUG				0		//!< Set to 1 to include kernel stack measurement
#define KERNEL_INTEGRITY_CHECK			0		//!< Set to 1 to include task integrity checks (TCB corruption)
#define KERNEL_USE_SEMAPHORE			1		//!< Set to 1 to include the use of built-in kernel semaphores
#define KERNEL_USE_MUTEX				0		//!< Set to 1 to include the use of built-in kernel mutex for resource protection
#define KERNEL_USE_QUANTUM				0		//!< Set to 1 to include the use of task quantum functionality
#define KERNEL_USE_WDT					0		//!< Set to 1 to include the built-in watchdog timer modules
#define KERNEL_USE_TIMERS				1		//!< Set to 1 to include the kernel-managed timer functions
#define KERNEL_USE_HEAP					0		//!< Set to 1 to include the FunkOS dynamic memory module
#define KERNEL_USE_MESSAGE				0		//!< Set to 1 to include message queues
#define KERNEL_NESTED_TICK				0		//!< Enable nested interrupts for Lightweight Threads and RTOS Tick

//===========================================================================
// Task configuration:  Use this to setup the parameters of the task handler
//===========================================================================
#define TICK_FREQ						1000		//!< Frequency (in Hz) that the system tick is called
#define SYSTEM_FREQ						8000000		//!< System frequency (in Hz)

#endif //__KERNELCFG_H_
