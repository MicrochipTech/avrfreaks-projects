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

    \file   threadport.cpp   

    \brief  ARM Cortex-M0 Multithreading

*/

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "thread.h"
#include "threadport.h"
#include "kernelswi.h"
#include "kerneltimer.h"
#include "timerlist.h"
#include "quantum.h"

//---------------------------------------------------------------------------
static void ThreadPort_StartFirstThread( void ) __attribute__ (( naked ));
void SVC_Handler( void ) __attribute__ (( naked ));
void PendSV_Handler( void ) __attribute__ (( naked ));
void SysTick_Handler( void );

//---------------------------------------------------------------------------
volatile K_ULONG g_ulCriticalCount;

//---------------------------------------------------------------------------
/*
    1) Setting up the thread stacks

    Stack consists of 2 separate frames mashed together.
    a) Exception Stack Frame

    Contains the 8 registers the CPU pushes/pops to/from the stack on execution
    of an exception:

    [ XPSR ]
    [ PC   ]
    [ LR   ]
    [ R12  ]
    [ R3   ]
    [ R2   ]
    [ R1   ]
    [ R0   ]

    XPSR - Needs to be set to 0x01000000; the "T" bit (thumb) must be set for
           any thread executing on an ARMv6-m processor
    PC - Should be set with the initial entry point for the thread
    LR - The base link register.  We can leave this as 0, and set to 0xD on
         first context switch to tell the CPU to resume execution using the
         stack pointer held in the PSP as the regular stack.

    This is done by the CPU automagically- this format is part of the
    architecture, and there's nothing we can do to change or modify it.

    b) "Other" Register Context

    [ R11   ]
    ...
    [ R4    ]

    These are the other GP registers that need to be backed up/restored on a
    context switch, but aren't by default on a CM0 exception.  If there were
    any additional hardware registers to back up, then we'd also have to
    include them in this part of the context.

    These all need to be manually pushed to the stack on stack creation, and
    puhsed/pop as part of a normal context switch.
*/
void ThreadPort::InitStack(Thread *pclThread_)
{
	K_ULONG *pulStack;
	K_ULONG *pulTemp;
	K_ULONG ulAddr;
	K_USHORT i;

	// Get the entrypoint for the thread
	ulAddr = (K_ULONG)(pclThread_->m_pfEntryPoint);

	// Get the top-of-stack pointer for the thread
	pulStack = (K_ULONG*)pclThread_->m_pwStackTop;

	// Initialize the stack to all FF's to aid in stack depth checking
	pulTemp = (K_ULONG*)pclThread_->m_pwStack;
	for (i = 0; i < pclThread_->m_usStackSize / sizeof(K_ULONG); i++)
	{
		pulTemp[i] = 0xFFFFFFFF;
	}

	PUSH_TO_STACK(pulStack, 0);				// We need one word of padding, apparently...
	
	//-- Simulated Exception Stack Frame --
	PUSH_TO_STACK(pulStack, 0x01000000);	// XSPR
	PUSH_TO_STACK(pulStack, ulAddr);		// PC
    PUSH_TO_STACK(pulStack, 0);             // LR
	PUSH_TO_STACK(pulStack, 0x12);
	PUSH_TO_STACK(pulStack, 0x3);
    PUSH_TO_STACK(pulStack, 0x2);
	PUSH_TO_STACK(pulStack, 0x1);
	PUSH_TO_STACK(pulStack, (K_ULONG)pclThread_->m_pvArg);	// R0 = argument

	//-- Simulated Manually-Stacked Registers --
    PUSH_TO_STACK(pulStack, 0x11);
	PUSH_TO_STACK(pulStack, 0x10);
	PUSH_TO_STACK(pulStack, 0x09);
	PUSH_TO_STACK(pulStack, 0x08);
	PUSH_TO_STACK(pulStack, 0x07);
	PUSH_TO_STACK(pulStack, 0x06);
	PUSH_TO_STACK(pulStack, 0x05);
	PUSH_TO_STACK(pulStack, 0x04);
	pulStack++;

	pclThread_->m_pwStackTop = pulStack;
}

//---------------------------------------------------------------------------
void Thread_Switch(void)
{
	g_pstCurrent = g_pstNext;
}

//---------------------------------------------------------------------------
void ThreadPort::StartThreads()
{
    KernelSWI::Config();             // configure the task switch SWI
    KernelTimer::Config();           // configure the kernel timer

    Scheduler::SetScheduler(1);      // enable the scheduler
    Scheduler::Schedule();           // run the scheduler - determine the first thread to run

    Thread_Switch();                 // Set the next scheduled thread to the current thread

    KernelTimer::Start();            // enable the kernel timer
    KernelSWI::Start();              // enable the task switch SWI

    ThreadPort_StartFirstThread();	 // Jump to the first thread (does not return)
}

//---------------------------------------------------------------------------
/*
    The same general process applies to starting the kernel as per usual

    We can either:
        1) Simulate a return from an exception manually to start the first
           thread, or..
        2) Use a software exception to trigger the first "Context Restore
            /Return from Interrupt" that we have otherwised used to this point.

    For 1), we basically have to restore the whole stack manually, not relying
    on the CPU to do any of this for us.  That's certainly doable, but not all
    Cortex parts support this (due to other members of the family supporting
    priveleged modes).  So, we will opt for the second option.

    So, to implement a software interrupt to restore our first thread, we will
    use the SVC instruction to generate that exception.

    At the end of thread initialization, we have to do 2 things:

    -Enable exceptions/interrupts
    -Call SVC

    Optionally, we can reset the MSP stack pointer to the top-of-stack.
    Note, the default stack pointer location is stored at address
    0x00000000 on all ARM Cortex M0 parts

    (While Mark3 avoids assembler code as much as possible, there are some
    places where it cannot be avoided.  However, we can at least inline it
    in most circumstances.)
*/
void ThreadPort_StartFirstThread( void )
{
    ASM (
        " mov r0, #0 \n"
        " ldr r1, [r0] \n"
        " msr msp, r1 \n"
        " cpsie i \n"
        " svc 0 \n"
    );
}

//---------------------------------------------------------------------------
/*
    The SVC Call

    This handler has the job of taking the first thread object's stack, and
    restoring the default state data in a way that ensures that the thread
    starts executing when returning from the call.

    We also keep in mind that there's an 8-byte offset from the beginning of
    the thread object to the location of the thread stack pointer.  This 
	offset is a result of the thread object inheriting from the linked-list
	node class, which has 8-bytes of data.  This is stored first in the 
	object, before the first element of the class, which is the "stack top"
	pointer.

    get_thread_stack:
        ; Get the stack pointer for the current thread
        ldr r0, g_pstCurrent
        ldr r1, [r0]
        add r1, #8
        ldr r2, [r1]         ; r2 contains the current stack-top

    load_manually_placed_context_r11_r8:
        ; Handle the bottom 32-bytes of the stack frame
        ; Start with r11-r8, because only r0-r7 can be used
        ; with ldmia on CM0.
        add r2, #16
        ldmia r2!, {r4-r7}
        mov r11, r7
        mov r10, r6
        mov r9, r5
        mov r8, r4

    set_psp:
        ; Since r2 is coincidentally back to where the stack pointer should be,
        ; Set the program stack pointer such that returning from the exception handler
        msr psp, r2

    load_manually_placed_context_r7_r4:
        ; Get back to the bottom of the manually stacked registers and pop.
        sub r2, #32
        ldmia r2!, {r4-r7}  ; Register r4-r11 are restored.

    ** Note - Since we don't care about these registers on init, we could take a shortcut if we wanted to **
    shortcut_init:
        add r2, #32
        msr psp, r2

    set_lr:
        ; Set up the link register such that on return, the code operates in thread mode using the PSP
        ; To do this, we or 0x0D to the value stored in the lr by the exception hardware EXC_RETURN.
        ; Alternately, we could just force lr to be 0xFFFFFFFD (we know that's what we want from the hardware, anyway)
        mov  r0, #0x0D
        mov  r1, lr
        orr r0, r1

    exit_exception:
        ; Return from the exception handler.  The CPU will automagically unstack R0-R3, R12, PC, LR, and xPSR
        ; for us.  If all goes well, our thread will start execution at the entrypoint, with the user-specified
        ; argument.
        bx r0

		This code is identical to what we need to restore the context, so
		we'll just make it a macro and be done with it.
*/
void SVC_Handler(void)
{
	ASM(
	// Get the pointer to the first thread's stack
	" mov r3, %[CURRENT_THREAD]\n "
	" add r3, #8 \n "
	" ldr r2, [r3] \n "
	// Stack pointer is in r2, start loading registers from the "manually-stacked" set
	// Start with r11-r8, since these can't be accessed directly.
	" add r2, #16 \n "
	" ldmia r2!, {r4-r7} \n "
	" mov r11, r7 \n "
	" mov r10, r6 \n "
	" mov r9, r5 \n "
	" mov r8, r4 \n "
	// After subbing R2 #16 manually, and #16 through ldmia, our PSP is where it
	// needs to be when we return from the exception handler
	" msr psp, r2 \n "
	// Pop manually-stacked R4-R7
	" sub r2, #32 \n "
	" ldmia r2!, {r4-r7} \n "
	// Also modify the control register to force use of thread mode as well
	// For CM3 forward-compatibility, also set user mode.
	" mrs r0, control \n"
	" mov r1, #0x03 \n"
	" orr r0, r1 \n"
	" msr control, r0 \n"	
	// Return into thread mode, using PSP as the thread's stack pointer
	// To do this, or 0x0D into the current lr.
	" mov r0, #0x0D \n "
	" mov r1, lr \n "
	" orr r0, r1 \n "
	" bx r0 \n "
	: : [CURRENT_THREAD] "r" (g_pstCurrent)
	);
}

//---------------------------------------------------------------------------
/*
	Context Switching:
	
	On ARM Cortex parts, there's dedicated hardware that's used primarily to 
	support RTOS (or RTOS-like) funcationlity.  This functionality includes
	the SysTick timer, and the PendSV Exception.  SysTick is used for the 
	kernel timer (I need to learn how to use it to see whether or not I can
	do tickless timers), while the PendSV exception is used for triggering
	context switches.  In reality, it's a "special SVC" call that's designed
	to be lower-overhead, in that it isn't mux'd with a bunch of other system
	or application functionality.
	
	Alright, so how do we go about actually implementing a context switch here?
	There are a lot of different parts involved, but it essentially comes down
	to 3 steps:
	
	1) Save Context
	2) Swap "current" and "next" thread pointers
	3) Restore Context
	
1) Saving the context.

	Alright, so when we enter the exception handler, We should expect that the 
	exception stack frame is stored to our PSP.  This takes care of everything
	but r4-r11.  Similar to the "restore context" code, we'll have to break up
	the register storage into multiple chunks.
	
	; Get address of current thread stack
	ldr r0, g_pstCurrentThread
	ldr r1, [r0]
	add r1, #8

	; Grab the psp and adjust it by 32 based on the extra registers we're going
	; to be manually stacking.
	mrs r2, psp
	sub r2, #32
	
	; While we're here, store the new top-of-stack value
	str r2, [r1]
	
	; And, while r2 is at the bottom of the stack frame, stack r7-r4
	stmia r2!, {r4-r7}
		
	; Stack r11-r8
	mov r7, r11	
	mov r6, r10
	mov r5, r9
	mov r4, r8			
	stmia r2!, {r4-r7}
	
	; Done!
		
	Thread's top-of-stack value is stored, all registers are stacked.  We're good to go!

2)  Swap threads

	This is the easy part - we just call a function to swap in the thread "current" thread	
	from the "next" thread.
	
3)	Restore Context

	This is more or less identical to what we did when restoring the first context. 
	Small optimization - we don't bother explicitly setting the 
	
*/	
void PendSV_Handler(void)
{	
	ASM(
	// Thread_SaveContext()
	" ldr r1, CURR_ \n"
	" ldr r1, [r1] \n "
	" mov r3, r1 \n "
	" add r3, #8 \n "
	
	//  Grab the psp and adjust it by 32 based on the extra registers we're going
	// to be manually stacking.
	" mrs r2, psp \n "
	" sub r2, #32 \n "
	
	// While we're here, store the new top-of-stack value
	" str r2, [r3] \n "
	
	// And, while r2 is at the bottom of the stack frame, stack r7-r4
	" stmia r2!, {r4-r7} \n "
	
	// Stack r11-r8
	" mov r7, r11 \n "
	" mov r6, r10 \n "
	" mov r5, r9 \n "
	" mov r4, r8 \n "
	" stmia r2!, {r4-r7} \n "
		
	// Equivalent of Thread_Swap()
	" ldr r1, CURR_ \n"
	" ldr r0, NEXT_ \n"
	" ldr r0, [r0] \n"
	" str r0, [r1] \n"
	
	// Get the pointer to the next thread's stack	
	" add r0, #8 \n "
	" ldr r2, [r0] \n "
	
	// Stack pointer is in r2, start loading registers from the "manually-stacked" set
	// Start with r11-r8, since these can't be accessed directly.
	" add r2, #16 \n "
	" ldmia r2!, {r4-r7} \n "
	" mov r11, r7 \n "
	" mov r10, r6 \n "
	" mov r9, r5 \n "
	" mov r8, r4 \n "
	
	// After subbing R2 #16 manually, and #16 through ldmia, our PSP is where it
	// needs to be when we return from the exception handler
	" msr psp, r2 \n "
	
	// Pop manually-stacked R4-R7
	" sub r2, #32 \n "
	" ldmia r2!, {r4-r7} \n "
		
	// lr contains the proper EXC_RETURN value, we're done with exceptions.
	" bx lr \n "
	" nop \n "
	
	// Must be 4-byte aligned.  Also - GNU assembler, I hate you for making me resort to this.
	" NEXT_: .word g_pstNext \n"
	" CURR_: .word g_pstCurrent \n"	
	);
}

//---------------------------------------------------------------------------
void SysTick_Handler(void)
{
#if KERNEL_USE_TIMERS
	TimerScheduler::Process();
#endif
#if KERNEL_USE_QUANTUM
	Quantum::UpdateTimer();
#endif

	// Clear the systick interrupt pending bit.
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
}
