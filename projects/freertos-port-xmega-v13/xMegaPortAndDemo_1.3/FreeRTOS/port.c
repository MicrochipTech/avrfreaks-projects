/*
    FreeRTOS V7.2.0 - Copyright (C) 2012 Real Time Engineers Ltd.

	xmega 32A4,128A1 port, VG Nov 2011

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest information, 
    license and contact details.
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/


#include "all.h"


/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h
 *----------------------------------------------------------*/

/* Start tasks with interrupts enabled. */
#define portFLAGS_INT_ENABLED					( ( portSTACK_TYPE ) 0x80 )


/* Hardware constants for timer 0. */
#define portCLOCK_PRESCALER_TIMER0				( ( unsigned portLONG ) 64 )

// to handle the deferred task swich with nested interrupts
unsigned portBASE_TYPE intTaskSwitchPending;




/*-----------------------------------------------------------*/

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;

//-----------------------------------------------------------

// Perform hardware setup to enable ticks from timer 0.
//
static void prvSetupTimerInterrupt( void );

// Returns the current interrupt priority level bits, then sets
// the priority mask up to that specified by configMAX_SYSCALL_INTERRUPT_PRIORITY.
// Higher interrupts are still active and not locked out.
//
portBASE_TYPE _portSetInterruptMaskFromIsr(void) __attribute__((always_inline));
portBASE_TYPE _portSetInterruptMaskFromIsr(void)
{
	register portBASE_TYPE r = PMIC.CTRL;
	PMIC.CTRL &= ~PMIC_BITS;
	return r;
}


//-----------------------------------------------------------
// 
// See header file for description. 
//

portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{

#if config24BITADDRESSING == 1
 	unsigned portLONG usAddress;	// 32bit, for 3 byte addressing
#else
 	unsigned portSHORT usAddress;	// 16bit, for 2 byte addressing
#endif

	// Place a few bytes of known values on the bottom of the stack. 
	// This is just useful for debugging.

	*pxTopOfStack = 0x11;
	pxTopOfStack--;
	*pxTopOfStack = 0x22;
	pxTopOfStack--;
	*pxTopOfStack = 0x33;
	pxTopOfStack--;

	//Simulate how the stack would look after a call to vPortYield() generated by the compiler.

	// The start of the task code will be popped off the stack last, so place it on first.

	usAddress = ( unsigned portSHORT ) pxCode;
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned portSHORT ) 0x00ff );
	pxTopOfStack--;

	usAddress >>= 8;
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned portSHORT ) 0x00ff );
	pxTopOfStack--;

	// not for xmega 32A4 (has 2-byte pc on stack), but for xmega 128A1 (3 byte pc)
	#ifndef config24BITADDRESSING
		#error Missing definition:  config24BITADDRESSING should be defined in FreeRTOSConfig.h as either 1 or 0.
	#endif
	#if	config24BITADDRESSING == 1  

		usAddress >>= 8;
		*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned portSHORT ) 0x00ff );
		pxTopOfStack--;

	#endif

	// Next simulate the stack as if after a call to portSAVE_CONTEXT().  

	*pxTopOfStack = ( portSTACK_TYPE ) 0x31;	/* R31 */
	pxTopOfStack--;

	// we start with interrupts enabled globally
	*pxTopOfStack = portFLAGS_INT_ENABLED;		/* sreg */
	pxTopOfStack--;

	// and all interrupt levels are enabled in the pmic
	//
	#if configENABLE_ROUND_ROBIN == 1
		*pxTopOfStack = ALL_PMIC_BITS | PMIC_RREN_bm; /* pmic */
	#else
		*pxTopOfStack = ALL_PMIC_BITS;
	#endif
	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	/* R0 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	/* R1 compiler expects R1 to be 0 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x02;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x03;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x04;	/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x05;	/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x06;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x07;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x08;	/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x09;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x10;	/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x11;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x12;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x13;	/* R13 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x14;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x15;	/* R15 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x16;	/* R16 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x17;	/* R17 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x18;	/* R18 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x19;	/* R19 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x20;	/* R20 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x21;	/* R21 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x22;	/* R22 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x23;	/* R23 */
	pxTopOfStack--;

	// This is the argument 'void *pvParameters', that every task has. 
	// Arguments are allocated in registers, left to right, r25 to r8.
	// All arguments are aligned to start in even-numbered registers.
	// (see avr-libv-user-manual.pdf, section "What Registers are used by the C-Compiler?")
	// Place the parameter on the stack in the expected location: r24,r25
	// 
	usAddress = ( unsigned portSHORT ) pvParameters;
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned portSHORT ) 0x00ff ); /* R24 */
	pxTopOfStack--;

	usAddress >>= 8;
	*pxTopOfStack = ( portSTACK_TYPE ) ( usAddress & ( unsigned portSHORT ) 0x00ff ); /* R25 */
	pxTopOfStack--;

	*pxTopOfStack = ( portSTACK_TYPE ) 0x26;	/* R26 X */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x27;	/* R27 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x28;	/* R28 Y */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x29;	/* R29 */
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x30;	/* R30 Z */
	pxTopOfStack--;

#if configEXTENDED_ADRESSING == 1
	// this registers must be zero
	//
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	// RAMPZ
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	// RAMPX
	pxTopOfStack--;
	*pxTopOfStack = ( portSTACK_TYPE ) 0x00;	// RAMPD
	pxTopOfStack--;
#endif

	return pxTopOfStack;
}


/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
	/* Setup the hardware to generate the tick. */
	prvSetupTimerInterrupt();

	/* Restore the context of the first task that is going to run. */
	portRESTORE_CONTEXT();

	/* Simulate a function call end as generated by the compiler.  We will now
	jump to the start of the task the context of which we have just restored. */
	asm volatile ( "ret" );

	/* Should not get here. */
	return pdTRUE;
}

/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the AVR port will get stopped.  If required simply
	disable the tick interrupt here. */
}

//-----------------------------------------------------------
//
// Manual context switch.  The first thing we do is save the registers so we
// can use a naked attribute.
// It is not necessary to disable the nesting interrupts 
// because this code is never executed in an interrupt context.
// (see explanation in task.c about line 1947)
// 
//
void vPortYield( void ) __attribute__ ( ( naked ) );
void vPortYield( void )
{
	portSAVE_CONTEXT();
	vTaskSwitchContext();
	portRESTORE_CONTEXT();
	asm volatile ( "ret" );
}

//-----------------------------------------------------------

#if configUSE_PREEMPTION == 1

#if configMAX_SYSCALL_INTERRUPT_PRIORITY > 0

	//-----------------------------------------------------------
	//
	// The task switch is deferred until there is no more nesting.
	//
	void portTaskSwitchContext(signed portBASE_TYPE xSwitchRequired)
	{
		intTaskSwitchPending |= xSwitchRequired;					
																
		if(NOT_NESTING() && intTaskSwitchPending)					
		{															
			register unsigned portBASE_TYPE uxSavedPmicCtrlReg; 				
			extern void vTaskSwitchContext(void);   				
																
			// Critical section used, because vTaskSwitchContext handles FreeRTOS internal data structures.
 			uxSavedPmicCtrlReg = portSET_INTERRUPT_MASK_FROM_ISR(); 
			intTaskSwitchPending = 0;								
			vTaskSwitchContext();                   				
			portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedPmicCtrlReg);
		}                                          					
	}


	//-----------------------------------------------------------
	//
	// This routine is only an indirect call to vTaskIncrementTick because
	// a local variable is necessary. Local variables cannot be used directly
	// in a naked isr because of the lack of the frame pointer.
	//
	static void portTaskIncrementTick( void )
	{
		register unsigned portBASE_TYPE uxSavedPmicCtrlReg;

 		uxSavedPmicCtrlReg = portSET_INTERRUPT_MASK_FROM_ISR();
		vTaskIncrementTick();
 		portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedPmicCtrlReg );
	}


	//-----------------------------------------------------------
	// Tick ISR for the preemptive scheduler.  We can use a naked attribute as
	// the context is saved at the start of vPortYieldFromTick().  The tick
	// count is incremented after the context is saved.
	//
	ISR(TCC0_OVF_vect, ISR_NAKED)
	{
		portSTART_ISR();
		portTaskIncrementTick();
		//always try a task switch, but only if on kernel interrupt level
		portEND_SWITCHING_ISR(pdTRUE); 
	}

#else // configMAX_SYSCALL_INTERRUPT_PRIORITY == 0

	ISR(TCC0_OVF_vect, ISR_NAKED)
	{
		portSAVE_CONTEXT();
		vTaskIncrementTick();
		vTaskSwitchContext();                         
		portRESTORE_CONTEXT();                            
		asm volatile ( "reti" );
	}

#endif

#else

	/*
	 * Tick ISR for the cooperative scheduler.  All this does is increment the
	 * tick count.  We don't need to switch context, this can only be done by
	 * manual calls to taskYIELD();
	 */
	ISR(TCC0_OVF_vect, ISR_NAKED)
	{
		vTaskIncrementTick();
		asm volatile ( "reti" );
	}

#endif

//-----------------------------------------------------------
//
// Setup of 16bit timer C0 to generate a tick interrupt in case of overflow.
//
static void prvSetupTimerInterrupt( void )
{

	unsigned portLONG ulOvfMatch;

	ulOvfMatch = configCPU_CLOCK_HZ / configTICK_RATE_HZ;

	// We only have 16bits so have to scale to get our required tick rate.
	ulOvfMatch /= portCLOCK_PRESCALER_TIMER0;

	// Adjust for correct value. (because overflow clear if CNT > PER)
	ulOvfMatch -= ( unsigned portLONG ) 1;

	// Setup overflow value
	TC_SetPeriod( &TCC0, ulOvfMatch);
		
	// Setup clock source and compare match behaviour.
	TC0_ConfigClockSource( &TCC0, TC_CLKSEL_DIV64_gc);

	// Enable the interrupt - this is okay as interrupts are currently globally disabled.
	// The tick interrupt may run in one of the three levels.
#if   configKERNEL_INTERRUPT_PRIORITY == 0
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_LO_gc);	//low interrupt priority
#elif configKERNEL_INTERRUPT_PRIORITY == 1
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_MED_gc);	//medium interrupt priority
#elif configKERNEL_INTERRUPT_PRIORITY == 2
	TC0_SetOverflowIntLevel( &TCC0, TC_OVFINTLVL_HI_gc);	//high interrupt priority
#endif
}

// EOF file port.c



	
