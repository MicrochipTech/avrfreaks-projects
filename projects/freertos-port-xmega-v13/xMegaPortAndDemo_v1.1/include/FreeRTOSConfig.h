/*
    FreeRTOS V6.1.0 - Copyright (C) 2010 Real Time Engineers Ltd.

	xmega 32A4,128A1 port, VG Jan 2011

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS books - available as PDF or paperback  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/



#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <avr/io.h>

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0


#define configCPU_CLOCK_HZ			( ( unsigned portLONG ) 29491200 )
#ifndef F_CPU
# warning ("F_CPU undefined !")
#else
#if (F_CPU != 29491200)
#  warning ("F_CPU and configCPU_CLOCK_HZ are not consistent !")
#endif
#endif

#define configTICK_RATE_HZ			( ( portTickType ) 1000 )

// In most cases r2 - r17 do not need to be preserved in the context switch
// since the compiler takes care to restore them if necessary.
//
#define configPRESERVE_R2R17			0

// atxmega  32A4 has a 2 byte program counter and 2 byte stack pointer, push/pop increments/decrements by 2
// atxmega 128A1 has a 3 byte program counter and 2 byte stack pointer, push/pop increments/decrements by 3
// atxmega 256a3 has a 3 byte program counter and 2 byte stack pointer, push/pop increments/decrements by 3
//
#define config24BITADDRESSING		0 // 0 for 32A4 ; 1 for 128A1,256A3

// controls whether the registers RAMPD,RAMPX,RAMPZ are saved in the context of a task.
// The gcc compiler does it, but I think that most applications don't need it. So leaving
// this to 0 makes the context switch faster and uses less stack space. If you suspect you need this,
// or just want to be on the safe side, set this value to 1. Rethink your decision if you are using 
// external RAM or a CPU with large program flash (>128k).
//
#define configEXTENDED_ADRESSING	0

// This enables round robin scheduling of low-level interrupts.
// When this option is 0, low-level interrupts are priority controlled.
// This option is not available for med- and high-level interrupts (the xmega hardware does not support this).
//
#define configENABLE_ROUND_ROBIN	1

// This defines the interrupt level on which the kernel is running (tick-interrupt).
// There are very few reasons not to use 0.
//
#define configKERNEL_INTERRUPT_PRIORITY			0	// 0=low-level ,1=medium-level, 2=high-level

// This value defines which ISRs are allowed to call the light-weight
// api functions (the fromISR... ones).
// ISRs running in a level < configKERNEL_INTERRUPT_PRIORITY are never allowed to call the 
// light-weight api functions.  
#define configMAX_SYSCALL_INTERRUPT_PRIORITY	2	// 0=low-level ,1=medium-level, 2=high-level

#if (configMAX_SYSCALL_INTERRUPT_PRIORITY < configKERNEL_INTERRUPT_PRIORITY)
	#error configMAX_SYSCALL_INTERRUPT_PRIORITY must be >= configKERNEL_INTERRUPT_PRIORITY !
#endif

#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned portSHORT ) 200 )
#define configTOTAL_HEAP_SIZE		( (size_t ) ( 3000 ) )
#define configMAX_TASK_NAME_LEN		( 8 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configQUEUE_REGISTRY_SIZE	0
#define	configCHECK_FOR_STACK_OVERFLOW 2 // use both method 1 and method 2

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 4 )
#define	configNUMBER_OF_COROUTINES	6

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			0
#define INCLUDE_vTaskDelete					0
#define INCLUDE_vTaskCleanUpResources		0
#define INCLUDE_vTaskSuspend				1 // needed to block on queues
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1

#define INCLUDE_uxTaskGetStackHighWaterMark	1
#define	INCLUDE_xTaskGetCurrentTaskHandle	1

#endif /* FREERTOS_CONFIG_H */
