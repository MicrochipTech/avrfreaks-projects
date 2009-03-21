/*
	FreeRTOS.org V5.2.0 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify it 
	under the terms of the GNU General Public License (version 2) as published
	by the Free Software Foundation and modified by the FreeRTOS exception.

	FreeRTOS.org is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
	more details.

	You should have received a copy of the GNU General Public License along 
	with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59 
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.

	A special exception to the GPL is included to allow you to distribute a 
	combined work that includes FreeRTOS.org without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details.


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

/*******************************************************************************
     FreeRTOS port to the CodeVisionAVR C compiler V2.04.0 or later
     by Pavel Haiduc, HP InfoTech s.r.l. www.hpinfotech.com
     V1.0, 16 March 2009
********************************************************************************/

#ifdef __CODEVISIONAVR__
#if __CODEVISIONAVR__ < 2040
#error Please compile with CodeVisionAVR V2.04.0 or later
#endif
#endif

/* allocation of global variables to registers R2..R14 must be disabled */
#pragma regalloc- 

#include <stdlib.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/*------------------------------------------------------------------------------
 * Implementation of functions defined in portable.h for the CodeVisionAVR port.
 *-----------------------------------------------------------------------------*/

/* Start tasks with interrupts enables. */
#define portFLAGS_INT_ENABLED ( ( portSTACK_TYPE ) 0x80 )

/* Hardware constants for Timer1. */
#if (defined _CHIP_ATMEGA128_) | (defined _CHIP_ATMEGA64_)

/* ATmega64, ATmega128 chips, Timer 1 runs in Mode 4 */
#define portCLEAR_COUNTER_ON_MATCH ( ( unsigned portCHAR ) 0x08 ) // WGM13=0 WGM12=1
#define portPRESCALE_64	( ( unsigned portCHAR ) 0x03 )
#define portCLOCK_PRESCALER ( ( unsigned portLONG ) 64 )
#define portCOMPARE_MATCH_A_INTERRUPT_ENABLE ( ( unsigned portCHAR ) 0x10 )

#else
#error please modify Timer1 hardware constants  to suit your chip
#endif

/* The number of bytes used on the hardware stack by the task start address. */
#define portBYTES_USED_BY_RETURN_ADDRESS ( 2 )
/*-----------------------------------------------------------*/

/* Stores the critical section nesting.  This must not be initialised to 0.
It will be initialised when a task starts. */
#define portNO_CRITICAL_NESTING	( ( unsigned portBASE_TYPE ) 0 )
unsigned portBASE_TYPE uxCriticalNesting = 0x50;

#asm
    .MACRO portSAVE_CONTEXT
	st	-y, r0			; First save the r0 register - we need to use this.
	in	r0, SREG		; Obtain the SREG value so we can disable interrupts
	cli					; as soon as possible.
	st	-y, r0			; Store the SREG as it was before we disabled interrupts.

	in	r0, SPL			; Next store the hardware stack pointer.  The CodeVisionAVR
	st	-y, r0			; compiler uses the hardware stack as a call stack
	in	r0, SPH			; only.
	st	-y, r0

	st	-y, r1			; Now store the rest of the registers.  Don't store the
	st	-y, r2			; Y register here as it is used as the software
	st	-y, r3			; stack pointer and will get saved into the TCB.
	st	-y, r4
	st	-y, r5
	st	-y, r6
	st	-y, r7
	st	-y, r8
	st	-y, r9
	st	-y, r10
	st	-y, r11
	st	-y, r12
	st	-y, r13
	st	-y, r14
	st	-y, r15
	st	-y, r16
	st	-y, r17
	st	-y, r18
	st	-y, r19
	st	-y, r20
	st	-y, r21
	st	-y, r22
	st	-y, r23
	st	-y, r24
	st	-y, r25
	st	-y, r26
	st	-y, r27
	st	-y, r30
	st	-y, r31
    
	lds r0, _uxCriticalNesting
	st	-y, r0			; Store the critical nesting counter.
    
	lds	r26, _pxCurrentTCB	; Finally save the software stack pointer (Y ...
	lds	r27, _pxCurrentTCB + 1	; ... register) into the TCB.
	st	x+, r28
	st	x+, r29
	.ENDM

    .MACRO portRESTORE_CONTEXT
	lds	r26, _pxCurrentTCB
	lds	r27, _pxCurrentTCB + 1	; Restore the software stack pointer from ...
	ld	r28, x+			; the TCB into the software stack pointer (...
	ld	r29, x+			; ... the Y register).
    
	ld	r0,  y+
	sts	_uxCriticalNesting, r0
    
	ld	r31, y+			; Restore the registers down to R0.  The Y
	ld	r30, y+			; register is missing from this list as it
	ld	r27, y+			; has already been restored.
	ld	r26, y+
	ld	r25, y+
	ld	r24, y+
	ld	r23, y+
	ld	r22, y+
	ld	r21, y+
	ld	r20, y+
	ld	r19, y+
	ld	r18, y+
	ld	r17, y+
	ld	r16, y+
	ld	r15, y+
	ld	r14, y+
	ld	r13, y+
	ld	r12, y+
	ld	r11, y+
	ld	r10, y+
	ld	r9, y+
	ld	r8, y+
	ld	r7, y+
	ld	r6, y+
	ld	r5, y+
	ld	r4, y+
	ld	r3, y+
	ld	r2, y+
	ld	r1, y+

	ld	r0, y+			; The next thing on the stack is the ...
	out	SPH, r0			; ... hardware stack pointer.
	ld	r0, y+
	out	SPL, r0

	ld	r0, y+			; Next there is the SREG register.
	out SREG, r0

	ld	r0, y+			; Finally we have finished with r0, so restore r0.
	.ENDM
#endasm


/*
 * Perform hardware setup to enable ticks from timer 1, compare match A.
 */
static void prvSetupTimerInterrupt( void );

/*
 *   Manual and preemptive context switch functions respectively.
 */
void vPortYield( void )
{
    #asm("portSAVE_CONTEXT") /* Save the context of the current task. */
    vTaskSwitchContext();   /* Call the scheduler. */
    #asm("portRESTORE_CONTEXT")	/* Restore the context of whichever task the scheduler decided should run. */
}

void vPortYieldFromTick( void )
{
    #asm("portSAVE_CONTEXT") /* Save the context of the current task. */
    vTaskIncrementTick();   /* Call the timer tick function. */
    vTaskSwitchContext();   /* Call the scheduler. */
    #asm("portRESTORE_CONTEXT")	/* Restore the context of whichever task the scheduler decided should run. */
}

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
    /* use registers for optimal code */
    portSTACK_TYPE * register pxTopOfHardwareStack,
    * register px = pxTopOfStack;

	/* Place a few bytes of known values on the bottom of the stack.
	This is just useful for debugging. */
    
    /*
	*px-- = 0x11;
	*px-- = 0x22;
	*px-- = 0x33;
    */
    
	/* Remember where the top of the hardware stack is - this is required below. */
	pxTopOfHardwareStack = px;

	/* Simulate how the stack would look after a call to vPortYield(). */

	/*lint -e950 -e611 -e923 Lint doesn't like this much - but nothing I can do about it. */

	/* The CodeVisionAVR compiler requires two stacks per task.  First there is the
	hardware call stack which uses the AVR stack pointer.  Second there is the
	software stack (local variables, parameter passing, etc.) which uses the
	AVR Y register as a pointer.
	
	This function places both stacks within the memory block passed in as the
	first parameter.  The hardware stack is placed at the bottom of the memory
	block.  A gap is then left for the hardware stack to grow.  Next the software
	stack is placed.  The amount of space between the software and hardware
	stacks is defined by configCALL_STACK_SIZE.

	The first part of the stack is the hardware stack.  Place the start
	address of the task on the hardware stack. */
	*px-- = ( portSTACK_TYPE ) *( ( unsigned portCHAR *) &pxCode );
	*px-- = ( portSTACK_TYPE ) *( ( unsigned portCHAR *) &pxCode + 1 );

	/* Leave enough space for the hardware stack before starting the software
	stack.  The '- 2' is because we have already used two spaces for the
	address of the start of the task. */
	px -= ( configCALL_STACK_SIZE - 2 );

	/* Place the parameter on the stack in the expected location. */
    /* the MSB of the parameter is at higher software stack address, it will be addressed by Y+1 */
	*px-- = ( portSTACK_TYPE ) *( ( unsigned portCHAR *) &pvParameters + 1 );
	/* the LSB of the parameter is at lower software stack address, it will be addressed by Y+0 */
	*px-- = ( portSTACK_TYPE ) *( ( unsigned portCHAR *) &pvParameters );

	/* Next simulate the stack as if after a call to portSAVE_CONTEXT().
	portSAVE_CONTEXT places the flags on the stack immediately after R0
	to ensure the interrupts get disabled as soon as possible, and so ensuring
	the stack use is minimal should a context switch interrupt occur. */
	*px-- = ( portSTACK_TYPE ) 0x00;	/* R0 */
	
    *px-- = portFLAGS_INT_ENABLED;

	/* Next place the address of the hardware stack.  This is required so
	the AVR stack pointer can be restored to point to the hardware stack. */
	pxTopOfHardwareStack -= portBYTES_USED_BY_RETURN_ADDRESS;

	/* SPL */
	*px-- = ( portSTACK_TYPE ) *( ( unsigned portCHAR *) &pxTopOfHardwareStack );
	/* SPH */
	*px-- = ( portSTACK_TYPE ) *( ( unsigned portCHAR *) &pxTopOfHardwareStack + 1 );

	/* Now the remaining registers. */
	*px-- = ( portSTACK_TYPE ) 0x01;	/* R1 */
	*px-- = ( portSTACK_TYPE ) 0x02;	/* R2 */
	*px-- = ( portSTACK_TYPE ) 0x03;	/* R3 */
	*px-- = ( portSTACK_TYPE ) 0x04;	/* R4 */
	*px-- = ( portSTACK_TYPE ) 0x05;	/* R5 */
	*px-- = ( portSTACK_TYPE ) 0x06;	/* R6 */
	*px-- = ( portSTACK_TYPE ) 0x07;	/* R7 */
	*px-- = ( portSTACK_TYPE ) 0x08;	/* R8 */
	*px-- = ( portSTACK_TYPE ) 0x09;	/* R9 */
	*px-- = ( portSTACK_TYPE ) 0x10;	/* R10 */
	*px-- = ( portSTACK_TYPE ) 0x11;	/* R11 */
	*px-- = ( portSTACK_TYPE ) 0x12;	/* R12 */
	*px-- = ( portSTACK_TYPE ) 0x13;	/* R13 */
	*px-- = ( portSTACK_TYPE ) 0x14;	/* R14 */
	*px-- = ( portSTACK_TYPE ) 0x15;	/* R15 */
	*px-- = ( portSTACK_TYPE ) 0x16;	/* R16 */
	*px-- = ( portSTACK_TYPE ) 0x17;	/* R17 */
	*px-- = ( portSTACK_TYPE ) 0x18;	/* R18 */
	*px-- = ( portSTACK_TYPE ) 0x19;	/* R19 */
	*px-- = ( portSTACK_TYPE ) 0x20;	/* R20 */
	*px-- = ( portSTACK_TYPE ) 0x21;	/* R21 */
	*px-- = ( portSTACK_TYPE ) 0x22;	/* R22 */
	*px-- = ( portSTACK_TYPE ) 0x23;	/* R23 */
	*px-- = ( portSTACK_TYPE ) 0x24;	/* R24 */
	*px-- = ( portSTACK_TYPE ) 0x25;	/* R25 */
	*px-- = ( portSTACK_TYPE ) 0x26;	/* R26 X */
	*px-- = ( portSTACK_TYPE ) 0x27;	/* R27 */

	/* The Y register is not stored as it is used as the software stack and
	gets saved into the task control block. */

	*px-- = ( portSTACK_TYPE ) 0x30;	/* R30 Z */
	*px-- = ( portSTACK_TYPE ) 0x31;	/* R31 */

	*px = portNO_CRITICAL_NESTING;	/* Critical nesting is zero when the task starts. */

	/*lint +e950 +e611 +e923 */

	return px;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
	/* Setup the hardware to generate the tick. */
	prvSetupTimerInterrupt();

	/* Restore the context of the first task that is going to run. */
    #asm
    portRESTORE_CONTEXT
    ret
    #endasm

	/* Should not get here! */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the AVR port will get stopped.  If required simply
	disable the tick interrupt here. */
}
/*-----------------------------------------------------------*/

/*
 * Setup timer 1 compare match A to generate a tick interrupt.
 */
static void prvSetupTimerInterrupt( void )
{
    unsigned portLONG ulCompareMatch;
    unsigned portCHAR ucHighByte, ucLowByte;

	/* Using 16bit timer 1 to generate the tick.  Correct fuses must be
	selected for the configCPU_CLOCK_HZ clock. */

	ulCompareMatch = configCPU_CLOCK_HZ / configTICK_RATE_HZ;

	/* We only have 16 bits so have to scale to get our required tick rate. */
	ulCompareMatch /= portCLOCK_PRESCALER;

	/* Adjust for correct value. */
	ulCompareMatch -= ( unsigned portLONG ) 1;

	/* Setup compare match value for compare match A.  Interrupts are disabled
	before this is called so we need not worry here. */
	ucLowByte = ( unsigned portCHAR ) ( ulCompareMatch & ( unsigned portLONG ) 0xff );
	ulCompareMatch >>= 8;
	ucHighByte = ( unsigned portCHAR ) ( ulCompareMatch & ( unsigned portLONG ) 0xff );

    #if (defined _CHIP_ATMEGA128_) | (defined _CHIP_ATMEGA64_)
    /* ATmega128, ATmega64 chips */
	OCR1AH = ucHighByte;
	OCR1AL = ucLowByte;
	/* Setup clock source and compare match behaviour. */
    /* Timer 1 runs in mode 4 */
    /* WGM10=0, WGM11=0, OC1A, OC1B, OC1C are disconnected */
    TCCR1A = 0; 
    /* WGM12=1, WGM13=0, set clock prescaler */
	TCCR1B = portCLEAR_COUNTER_ON_MATCH | portPRESCALE_64; 
	/* Enable the interrupt - this is okay as interrupt are currently globally
	disabled. */
	TIMSK |= portCOMPARE_MATCH_A_INTERRUPT_ENABLE;
    #else
    #error please modify Timer1 initialization to suit your chip
    #endif

}
/*-----------------------------------------------------------*/

#if configUSE_PREEMPTION == 1
	/*
	 * Tick ISR for preemptive scheduler.  We can use a #pragma savereg as
	 * the context is saved at the start of vPortYieldFromTick().  The tick
	 * count is incremented after the context is saved.
	 */
#pragma savereg-
	interrupt[TIM1_COMPA] void SIG_OUTPUT_COMPARE1A( void )
	{
		vPortYieldFromTick();
	}
#pragma savereg-
#else
	/*
	 * Tick ISR for the cooperative scheduler.  All this does is increment the
	 * tick count.  We don't need to switch context, this can only be done by
	 * manual calls to taskYIELD();
	 */		
	interrupt[TIM1_COMPA] void SIG_OUTPUT_COMPARE1A( void )
	{
		vTaskIncrementTick();
	}
#endif
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	uxCriticalNesting--;
	if( uxCriticalNesting == portNO_CRITICAL_NESTING )
	{
		portENABLE_INTERRUPTS();
	}
}

	
