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
//
// With e.g. configMAX_SYSCALL_INTERRUPT_PRIORITY=0 , medium- und high-level interrupts can 
// be always enabled and never disturb level-0 ints as
// long as no api functions of FreeRTOS are called.
//
// There is no need to globally disable ints via cli.
//

// Disable the interrupts only in the pmic.
// Interrupts are only disabled up to a certain level, higher level interrupts are still allowed.
// We only need to disable interrupts that might call a RTOS API function.
//
#if configMAX_SYSCALL_INTERRUPT_PRIORITY == 0
	#define PMIC_BITS (PMIC_LOLVLEN_bm)
#elif configMAX_SYSCALL_INTERRUPT_PRIORITY == 1
	#define PMIC_BITS (PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm)
#elif configMAX_SYSCALL_INTERRUPT_PRIORITY == 2
	#define PMIC_BITS (PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm)
#endif

#define ALL_PMIC_BITS	(PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm)  

// Enable all interrupt levels up to configMAX_SYSCALL_INTERRUPT_PRIORITY.
// Round robin may be enabled or disabled, depending on the application.
//
#if configENABLE_ROUND_ROBIN == 1
	#define portENABLE_INTERRUPTS()		(PMIC.CTRL |= ALL_PMIC_BITS | PMIC_RREN_bm);sei()
#else
	#define portENABLE_INTERRUPTS()		(PMIC.CTRL |= ALL_PMIC_BITS);sei()
#endif

// Disable interrupts at and below kernel interrupt level
//
#define portDISABLE_INTERRUPTS()	(PMIC.CTRL &= ~PMIC_BITS)

#define portGLOBAL_INTERRUPT_ENABLE()	cli()
#define portGLOBAL_INTERRUPT_DISABLE()	sei()

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

//-----------------------------------------------------------*/

// 
// Macro to save all the general purpose registers, then save the stack pointer
// into the TCB.  
//
// The first thing we do is save the flags.
// then save all registers
// and then disable the current low-level interrupts.
//
// Another interrupt of this level will never happen, this is ensured by hardware.
// Higher level interrupts may occur at any time. There must be enough stack space!
// Note that writing to the SP is atomic (see corresponding chapter in the xMega documentation) 
// as long as we write low and high byte one after another.
// 
// r1 is set to zero as the compiler expects it to be thus, however some
// of the math routines make use of R1. 
//
#define portSAVE_SREG()												\
	asm volatile (	"push	r31				;Z_H			\n\t"	\
					"in		r31, __SREG__					\n\t"	\
					"push	r31								\n\t"	\
				 );

// Macro to mark the start of a critical code region. 
// Preemptive context switches cannot occur when in a critical region.
// NOTE: This alters the stack, so must be used with care!
//
// Interrupts up to the configMAX_SYSCALL_INTERRUPT_PRIORITY-level are disabled as early as
// possible, so there is no nesting within a level happening.
//
#define portENTER_CRITICAL()                       									\
	asm volatile (	"lds	r31, 0x00A2	 ;pmic ctrl							\n\t"	\
					"push	r31	      	 ;remember pmic-ctrl				\n\t"	\
					"cbr	r31, %[mask] ;disable low- and med-level ints	\n\t"	\
					"sts	0x00A2,r31   ;disable low-level					\n\t"	\
					:: [mask] "M" (PMIC_BITS) :"r31");

#define portSAVE_REGS1()											\
	asm volatile (	"push	r0								\n\t"	\
					"push	r1								\n\t"	\
					"clr	r1	;gcc expects r1 to be zero	\n\t"	\
				 );

// gcc does not save r2 - r17 !
// This is the group of registers, that an ISR (like a subroutine) must preserve.
// The context may be kept small if you follow this convention.
//
#define portSAVE_REGSR2R17()										\
	asm volatile (	"push	r2								\n\t"	\
					"push	r3								\n\t"	\
					"push	r4								\n\t"	\
					"push	r5								\n\t"	\
					"push	r6								\n\t"	\
					"push	r7								\n\t"	\
					"push	r8								\n\t"	\
					"push	r9								\n\t"	\
					"push	r10								\n\t"	\
					"push	r11								\n\t"	\
					"push	r12								\n\t"	\
					"push	r13								\n\t"	\
					"push	r14								\n\t"	\
					"push	r15								\n\t"	\
					"push	r16								\n\t"	\
					"push	r17								\n\t"	\
				 );

#if configPRESERVE_R2R17 == 1
	#define portSAVE_REGS2()	portSAVE_REGSR2R17()
#else
	#define portSAVE_REGS2()
#endif

#define portSAVE_REGS3()											\
	asm volatile ( 	"push	r18								\n\t"	\
					"push	r19								\n\t"	\
					"push	r20								\n\t"	\
					"push	r21								\n\t"	\
					"push	r22								\n\t"	\
					"push	r23								\n\t"	\
					"push	r24								\n\t"	\
					"push	r25								\n\t"	\
					"push	r26		;X_L					\n\t"	\
					"push	r27		;X_H					\n\t"	\
					"push	r28		;Y_L					\n\t"	\
					"push	r29		;Y_H					\n\t"	\
					"push	r30		;Z_L					\n\t"	\
				 );

// RAMPX,RAMPY,RAMPZ are concatenated with X-,Y-, Z-regs to enable indirect
//                   addressing of the whole data memory space above 64K.
// 
// RAMPD is concatenated with the operand to enable direct addressing of 
//       the whole data memory space above 64K.
//
// EIND  is concatenated with the Z-register to enable indirect jump and call
//       to locations above the first 128K bytes (64K words) of the program memory.
//
// (see. AVR XMEGA Manual, section 3.10.1,2,3 )
//
// gcc saves RAMPD, RAMPX, RAMPZ.
// gcc does not save  RAMPY, EIND.
// we do the same as gcc does!
//
// I guess most applications do not need to save these regs. This is true at
// least for the atxmega128A1 and 32A4 since both ram and program memory are
// too small. 
// Rethink this if you eather use external ram oder have more than 128k program 
// flash (e.g. atxmega256A3)!
//
#define portSAVE_MORE_REGS()										\
	asm volatile (	"in		r31,0x003B 	;RAMPZ				\n\t"	\
					"push	r31								\n\t"	\
					"in		r31,0x0039 	;RAMPX				\n\t"	\
					"push	r31								\n\t"	\
					"in		r31,0x0038 	;RAMPD				\n\t"	\
					"push	r31								\n\t"	\
					"sts	0x003B, r1	;RAMPZ=0			\n\t"	\
					"sts	0x0039, r1	;RAMPX=0			\n\t"	\
					"sts	0x0038, r1	;RAMPD=0			\n\t"	\
				 );

#if configEXTENDED_ADRESSING == 1
	#define portSAVE_REGS4() portSAVE_MORE_REGS()
#else
	#define portSAVE_REGS4()
#endif

#define portSAVE_SP()																	\
	asm volatile (	"lds	r26, pxCurrentTCB		;load X with address of SP 	\n\t"	\
					"lds	r27, pxCurrentTCB + 1	; location in the TCB	  	\n\t"	\
					"in		__tmp_reg__, __SP_L__								\n\t"	\
					"st		x+, __tmp_reg__										\n\t"	\
					"in		__tmp_reg__, __SP_H__								\n\t"	\
					"st		x+, __tmp_reg__										\n\t"	\
				 );

#define portSAVE_CONTEXT()	\
	portSAVE_SREG()			\
	portENTER_CRITICAL()	\
	portSAVE_REGS1()		\
	portSAVE_REGS2()		\
	portSAVE_REGS3()		\
	portSAVE_REGS4()		\
	portSAVE_SP()
 
// Opposite to portSAVE_CONTEXT().  
// We can write to the stack pointer, since this is atomic by h/w as long as we do it within 4 instructions. 
//
#define portRESTORE_SP()																	\
	asm volatile (	"lds	r26, pxCurrentTCB										\n\t"	\
					"lds	r27, pxCurrentTCB + 1									\n\t"	\
					"ld		__tmp_reg__, x+											\n\t"	\
					"out	__SP_L__, __tmp_reg__	;this writing to SP is atomic!	\n\t"	\
					"ld		__tmp_reg__, x+											\n\t"	\
					"out	__SP_H__, __tmp_reg__									\n\t"	\
				 );

#define portRESTORE_MORE_REGS()										\
	asm volatile (	"pop	r31								\n\t"	\
					"out	0x0038,r31 ;RAMPD				\n\t"	\
					"pop	r31								\n\t"	\
					"out	0x0039,r31 ;RAMPX				\n\t"	\
					"pop	r31								\n\t"	\
					"out	0x003B,r31 ;RAMPZ				\n\t"	\
				 );

#define portRESTORE_REGS3()											\
	asm volatile (	"pop	r30		;Z_L					\n\t"	\
					"pop	r29		;Y_H					\n\t"	\
					"pop	r28		;Y_L					\n\t"	\
					"pop	r27		;X_H					\n\t"	\
					"pop	r26		;X_L					\n\t"	\
					"pop	r25								\n\t"	\
					"pop	r24								\n\t"	\
					"pop	r23								\n\t"	\
					"pop	r22								\n\t"	\
					"pop	r21								\n\t"	\
					"pop	r20								\n\t"	\
					"pop	r19								\n\t"	\
					"pop	r18								\n\t"	\
				);

#define portRESTORE_REGSR2R17()										\
	asm volatile(	"pop	r17								\n\t"	\
					"pop	r16								\n\t"	\
					"pop	r15								\n\t"	\
					"pop	r14								\n\t"	\
					"pop	r13								\n\t"	\
					"pop	r12								\n\t"	\
					"pop	r11								\n\t"	\
					"pop	r10								\n\t"	\
					"pop	r9								\n\t"	\
					"pop	r8								\n\t"	\
					"pop	r7								\n\t"	\
					"pop	r6								\n\t"	\
					"pop	r5								\n\t"	\
					"pop	r4								\n\t"	\
					"pop	r3								\n\t"	\
					"pop	r2								\n\t"	\
				);

#if configPRESERVE_R2R17 == 1
	#define portRESTORE_REGS2()	portRESTORE_REGSR2R17()
#else
	#define portRESTORE_REGS2()
#endif

#define portRESTORE_REGS1()											\
	asm volatile(	"pop	r1								\n\t"	\
					"pop	r0								\n\t"	\
				);

// Macro to mark the end of a critical code region.
//
#define portEXIT_CRITICAL() 															\
	asm volatile(	"pop	r31	      	;the saved pmic-ctrl register value 	\n\t"	\
					"sts	0x00A2,r31  ;restore pmic-ctrl						\n\t"	\
				 	:::"r31"); 															

#define portRESTORE_SREG()											\
	asm volatile(	"pop	r31								\n\t"	\
					"out	__SREG__, r31					\n\t"	\
					"pop	r31								\n\t"	\
				);

#if configEXTENDED_ADRESSING == 1
	#define portRESTORE_REGS4() portRESTORE_MORE_REGS()
#else
	#define portRESTORE_REGS4()
#endif

#define portRESTORE_CONTEXT()	\
	portRESTORE_SP()			\
	portRESTORE_REGS4()			\
	portRESTORE_REGS3()			\
	portRESTORE_REGS2()			\
	portRESTORE_REGS1()			\
	portEXIT_CRITICAL()			\
	portRESTORE_SREG()


//------------------------------------------------------------
//
// ISR helper macros.
//
#define portSTART_ISR() 	\
	portSAVE_CONTEXT()

#define portEND_ISR()		\
	portRESTORE_CONTEXT();	\
	asm volatile ( "reti" );


// With nesting ISRs that can call the api, the decision
// whether a task-switch can be triggered is a lot more complicated
// and expensive (in terms of time).
//
#if configMAX_SYSCALL_INTERRUPT_PRIORITY > 0

extern unsigned portBASE_TYPE intTaskSwitchPending;

// Takes a note of the current interrupt priority level, then sets
// the priority mask up to that specified by configMAX_SYSCALL_INTERRUPT_PRIORITY
//
#define portSET_INTERRUPT_MASK_FROM_ISR() \
	(PMIC.CTRL &= ~PMIC_BITS)	

// Sets the priority mask down to that noted by portSET_INTERRUPT_MASK_FROM_ISR().
//
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus ) \
	(PMIC.CTRL = uxSavedInterruptStatus)	

// There is no nesting if only a single bit is set.
// The NMI bit is ignored here.
//
#if   configKERNEL_INTERRUPT_PRIORITY == 0
	#define NOT_NESTING() ((PMIC.STATUS == PMIC_LOLVLEX_bm) || (PMIC.STATUS == PMIC_MEDLVLEX_bm) || (PMIC.STATUS == PMIC_HILVLEX_bm))
#elif configKERNEL_INTERRUPT_PRIORITY == 1
	#define NOT_NESTING() (PMIC.STATUS == 					   (PMIC.STATUS == PMIC_MEDLVLEX_bm) || (PMIC.STATUS == PMIC_HILVLEX_bm))
#elif configKERNEL_INTERRUPT_PRIORITY == 2
	#define NOT_NESTING() (pdTRUE)
#endif

/*
// inline is much more convenient than a macro 
//TEMP: inlining führt sofort zum Fehler, warum ?
//static inline void portTaskSwitchContext(unsigned portBASE_TYPE xSwitchRequired) __attribute__((always_inline));
static inline void portTaskSwitchContext(unsigned portBASE_TYPE xSwitchRequired)
{
	intTaskSwitchPending |= xSwitchRequired;					
																
	if(NOT_NESTING() && intTaskSwitchPending)					
	{															
		// Using a register because there is no framepointer.
		// The compiler uses r17 (looking at the assembler code) 
		register unsigned portBASE_TYPE uxSavedPmicCtrlReg; 				
																
		uxSavedPmicCtrlReg = portSET_INTERRUPT_MASK_FROM_ISR(); 
		intTaskSwitchPending = 0;								
		extern void vTaskSwitchContext(void);   				
		vTaskSwitchContext();                   				
		portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedPmicCtrlReg);
	}                                          					
}
*/

// This makro must be used in ISRs that trigger a task switch.
// The actual task switch is deferred until the last ISR nesting level is unrolled.
//
// Task switching must be triggered only in the interrupt-level of the kernel (configKERNEL_INTERRUPT_PRIORITY).
// When the int-level bit of the kernel level is the only one that is set in pmic status, then a switch may happen.
//
// This works without a nesting counter (as could be seen in other ports) because
// all necessay information can be read from the pmic status register.
//
void portTaskSwitchContext(int xSwitchRequired);

#define portEND_SWITCHING_ISR( xSwitchRequired ) 	\
	portTaskSwitchContext(xSwitchRequired); 		\
	portRESTORE_CONTEXT();                      	\
	asm volatile ( "reti" );
 


#else
// Simple version of portEND_SWITCHING_ISR().
// If nesting of ISRs that can call the api does not occur at all,
// then avoid all that complicated overhead.

// This makro must be used in ISRs that trigger a task switch.
// The actual task switch is deferred until the last ISR nesting is unrolled.
#define portEND_SWITCHING_ISR( xHigherPriorityTaskWoken ) \
	if( xHigherPriorityTaskWoken )                    \
	{                                                 \
		extern void vTaskSwitchContext(void);         \
		vTaskSwitchContext();                         \
	}                                                 \
	portRESTORE_CONTEXT();                            \
	asm volatile ( "reti" );

#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

