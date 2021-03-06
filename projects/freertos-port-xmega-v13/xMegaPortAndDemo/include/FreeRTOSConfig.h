/*
	FreeRTOS.org V5.3.1 - Copyright (C) 2003-2009 Richard Barry.


	Settings specific to xmega and this project, VG 25.08.2010


	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License (version 2) as published
	by the Free Software Foundation and modified by the FreeRTOS exception.
	**NOTE** The exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS.org without being obliged to provide
	the source code for any proprietary components.  Alternative commercial
	license and support terms are also available upon request.  See the 
	licensing section of http://www.FreeRTOS.org for full details.

	FreeRTOS.org is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.


	***************************************************************************
	*                                                                         *
	* Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
	*                                                                         *
	***************************************************************************

	1 tab == 4 spaces!

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

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


#define configCPU_CLOCK_HZ			( ( unsigned portLONG ) 14745600 )
#ifndef F_CPU
# warning ("F_CPU undefined !")
#else
#if (F_CPU != 14745600)
#  warning ("F_CPU and configCPU_CLOCK_HZ are not consistent !")
#endif
#endif

#define configTICK_RATE_HZ			( ( portTickType ) 1000 )

#define	configPROGMEM_SIZE			0x8000  // 32K program flash for xMega 32A4 (=2 byte pc)

#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned portSHORT ) 140 )
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

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				0
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1 // needed to block on queues
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

#define INCLUDE_uxTaskGetStackHighWaterMark	1
#define	INCLUDE_xTaskGetCurrentTaskHandle	1

#endif /* FREERTOS_CONFIG_H */
