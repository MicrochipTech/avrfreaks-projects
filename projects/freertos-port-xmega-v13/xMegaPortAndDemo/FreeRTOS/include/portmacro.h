/*
	FreeRTOS.org V5.3.1 - Copyright (C) 2003-2009 Richard Barry.

	xmega 32A4 port, VG Oct 2010

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


#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "pmic_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.  
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		int
#define portSTACK_TYPE	unsigned portCHAR
#define portBASE_TYPE	char

#if( configUSE_16_BIT_TICKS == 1 )
	typedef unsigned portSHORT portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffff
#else
	typedef unsigned portLONG portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif

//-----------------------------------------------------------	

// Critical section management.

// Interrupt on the xmega cpu never interrupt each other within one level,
// so that real nesting does not occur.
//
// Level-1 und -2 interrupts kann be always enabled and never disturb level-0 ints as
// long as no api functions of FreeRTOS are called.
//
// There should be no need to globally disable int via cli.
//

// disable the low-level interrupts only in the pmic
// medium-level and high-level interrupts are still allowed
//
#define portDISABLE_INTERRUPTS() \
{ \
	PMIC_DisableLowLevel();      \
}

// round robin may be enabled or disabled, depending on the application
#define portENABLE_INTERRUPTS() \
{ \
	PMIC_EnableRoundRobin(); 	\
	PMIC_EnableLowLevel();      \
	sei(); \
}


#define portENTER_CRITICAL()                            \
	asm volatile( 	"ldi	r30, 0xA2			\n\t"	\
	 				"ldi	r31, 0x00			\n\t"	\
	 				"ld		r24, Z				\n\t"	\
					"mov 	r25, r24			\n\t"	\
					"andi 	r24, 0x01			\n\t"	\
					"push 	r24					\n\t"	\
					"andi 	r25, 0xFE			\n\t"	\
					"st	Z, 	r25					\n\t"	\
				); 

#define portEXIT_CRITICAL() \
	asm volatile( 	"pop r24					\n\t"	\
					"ldi	r30, 0xA2			\n\t"	\
					"ldi	r31, 0x00			\n\t"	\
					"ld r25, Z					\n\t"	\
					"or r25, r24				\n\t"	\
					"st	Z, r25					\n\t"	\
				); 


/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_RATE_MS			( ( portTickType ) 1000 / configTICK_RATE_HZ )		
#define portBYTE_ALIGNMENT			1
#define portNOP()					asm volatile ( "nop" );
/*-----------------------------------------------------------*/

/* Kernel utilities. */
extern void vPortYield( void ) __attribute__ ( ( naked ) );
#define portYIELD()					vPortYield()

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/*-----------------------------------------------------------*/

/* 
 * Macro to save all the general purpose registers, the save the stack pointer
 * into the TCB.  
 * 
 * The first thing we do is save the flags then disable interrupts.  This is to 
 * guard our stack against having a context switch interrupt after we have already 
 * pushed the registers onto the stack - causing the 32 registers to be on the 
 * stack twice. 
 * 
 * r1 is set to zero as the compiler expects it to be thus, however some
 * of the math routines make use of R1. 
 * 
 * The low-interrupts will have been disabled during the call to portSAVE_CONTEXT()
 * so we need not worry about reading/writing to the stack pointer. 
 */


// Save pmic status as well.
// Interupts do not need to be disabled, because another low-level
// cannot occur. Ints within one level do nest. This is ensured by
// the xmega hardware.
//
#define portSAVE_CONTEXT()									\
	asm volatile (	"push	r0						\n\t"	\
					"in		r0, __SREG__			\n\t"	\
					"push	r0						\n\t"	\
					"push	r28						\n\t"	\
					"push	r29						\n\t"	\
					"push	r30						\n\t"	\
					"push	r31						\n\t"	\
					"ldi	r30, 0xA2 ;pmic address \n\t"	\
					"ldi	r31, 0x00				\n\t"	\
					"ld		r29, Z					\n\t"	\
					"mov    r28, r29				\n\t"	\
					"andi	r29, 0x01 ;extract low-level interrupt bit	\n\t"	\
					"push	r29	      ;remember low-level status		\n\t"	\
					"andi	r28, 0xFE				\n\t"	\
					"st		Z,   r28  ;disable low-level				\n\t"	\
					"push	r1						\n\t"	\
					"clr	r1						\n\t"	\
					"push	r2						\n\t"	\
					"push	r3						\n\t"	\
					"push	r4						\n\t"	\
					"push	r5						\n\t"	\
					"push	r6						\n\t"	\
					"push	r7						\n\t"	\
					"push	r8						\n\t"	\
					"push	r9						\n\t"	\
					"push	r10						\n\t"	\
					"push	r11						\n\t"	\
					"push	r12						\n\t"	\
					"push	r13						\n\t"	\
					"push	r14						\n\t"	\
					"push	r15						\n\t"	\
					"push	r16						\n\t"	\
					"push	r17						\n\t"	\
					"push	r18						\n\t"	\
					"push	r19						\n\t"	\
					"push	r20						\n\t"	\
					"push	r21						\n\t"	\
					"push	r22						\n\t"	\
					"push	r23						\n\t"	\
					"push	r24						\n\t"	\
					"push	r25						\n\t"	\
					"push	r26						\n\t"	\
					"push	r27						\n\t"	\
					"lds	r26, pxCurrentTCB		\n\t"	\
					"lds	r27, pxCurrentTCB + 1	\n\t"	\
					"in		r0, 0x3d				\n\t"	\
					"st		x+, r0					\n\t"	\
					"in		r0, 0x3e				\n\t"	\
					"st		x+, r0					\n\t"	\
				);

/* 
 * Opposite to portSAVE_CONTEXT().  Interrupts will have been disabled during
 * the context save so we can write to the stack pointer. 
 */

#define portRESTORE_CONTEXT()								\
	asm volatile (	"lds	r26, pxCurrentTCB		\n\t"	\
					"lds	r27, pxCurrentTCB + 1	\n\t"	\
					"ld		r28, x+					\n\t"	\
					"out	__SP_L__, r28			\n\t"	\
					"ld		r29, x+					\n\t"	\
					"out	__SP_H__, r29			\n\t"	\
					"pop	r27						\n\t"	\
					"pop	r26						\n\t"	\
					"pop	r25						\n\t"	\
					"pop	r24						\n\t"	\
					"pop	r23						\n\t"	\
					"pop	r22						\n\t"	\
					"pop	r21						\n\t"	\
					"pop	r20						\n\t"	\
					"pop	r19						\n\t"	\
					"pop	r18						\n\t"	\
					"pop	r17						\n\t"	\
					"pop	r16						\n\t"	\
					"pop	r15						\n\t"	\
					"pop	r14						\n\t"	\
					"pop	r13						\n\t"	\
					"pop	r12						\n\t"	\
					"pop	r11						\n\t"	\
					"pop	r10						\n\t"	\
					"pop	r9						\n\t"	\
					"pop	r8						\n\t"	\
					"pop	r7						\n\t"	\
					"pop	r6						\n\t"	\
					"pop	r5						\n\t"	\
					"pop	r4						\n\t"	\
					"pop	r3						\n\t"	\
					"pop	r2						\n\t"	\
					"pop	r1						\n\t"	\
					"pop	r29	      ;pmic status	\n\t"	\
					"ldi	r30, 0xA2				\n\t"	\
					"ldi	r31, 0x00				\n\t"	\
					"ld		r28, Z					\n\t"	\
					"or		r29, r28  ;combine with current pmic status \n\t"	\
					"st		Z,   r29  ;enable low-level interupts if they were enabled on save context \n\t"	\
					"pop	r31						\n\t"	\
					"pop	r30						\n\t"	\
					"pop	r29						\n\t"	\
					"pop	r28						\n\t"	\
					"pop	r0						\n\t"	\
					"out	__SREG__, r0			\n\t"	\
					"pop	r0						\n\t"	\
				);

//------------------------------------------------------------
//
// ISR helper macros.
//
#define portSTART_ISR() portSAVE_CONTEXT()

#define portEND_ISR() portRESTORE_CONTEXT(); asm volatile ( "reti" );


// This makro must be used in ISRs that trigger a task switch
#define portEND_SWITCHING_ISR( xHigherPriorityTaskWoken ) \
	{                                                     \
		if( xHigherPriorityTaskWoken )                    \
		{                                                 \
			extern void vTaskSwitchContext(void);         \
			vTaskSwitchContext();                         \
		}                                                 \
		portRESTORE_CONTEXT();                            \
	}                                                     \
	asm volatile ( "reti" );
 

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

