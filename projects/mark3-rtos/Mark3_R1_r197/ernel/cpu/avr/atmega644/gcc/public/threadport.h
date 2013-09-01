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

    \file   threadport.h    

    \brief  ATMega644 Multithreading support.
*/

#ifndef __THREADPORT_H_
#define __THREADPORT_H_

#include "kerneltypes.h"
#include "thread.h"

#include <avr/io.h>
#include <avr/interrupt.h>

//---------------------------------------------------------------------------
//! ASM Macro - simplify the use of ASM directive in C
#define ASM(x)      asm volatile(x);
//! Status register define - map to 0x003F
#define SR_         0x3F
//! Stack pointer define
#define SPH_        0x3E
#define SPL_        0x3D
#define RAMPZ_        0x3B

//---------------------------------------------------------------------------
//! Macro to find the top of a stack given its size and top address
#define TOP_OF_STACK(x, y)        (K_UCHAR*) ( ((K_USHORT)x) + (y-1) )
//! Push a value y to the stack pointer x and decrement the stack pointer
#define PUSH_TO_STACK(x, y)        *x = y; x--;

//---------------------------------------------------------------------------
//! Save the context of the Thread
#define Thread_SaveContext() \
ASM("push r0"); \
ASM("in r0, __SREG__"); \
ASM("cli"); \
ASM("push r0"); \
ASM("push r1"); \
ASM("clr r1"); \
ASM("push r2"); \
ASM("push r3"); \
ASM("push r4"); \
ASM("push r5"); \
ASM("push r6"); \
ASM("push r7"); \
ASM("push r8"); \
ASM("push r9"); \
ASM("push r10"); \
ASM("push r11"); \
ASM("push r12"); \
ASM("push r13"); \
ASM("push r14"); \
ASM("push r15"); \
ASM("push r16"); \
ASM("push r17"); \
ASM("push r18"); \
ASM("push r19"); \
ASM("push r20"); \
ASM("push r21"); \
ASM("push r22"); \
ASM("push r23"); \
ASM("push r24"); \
ASM("push r25"); \
ASM("push r26"); \
ASM("push r27"); \
ASM("push r28"); \
ASM("push r29"); \
ASM("push r30"); \
ASM("push r31"); \
ASM("lds r26, g_pstCurrent"); \
ASM("lds r27, g_pstCurrent + 1"); \
ASM("adiw r26, 4"); \
ASM("in    r0, 0x3D"); \
ASM("st    x+, r0"); \
ASM("in    r0, 0x3E"); \
ASM("st    x+, r0");

//---------------------------------------------------------------------------
//! Restore the context of the Thread
#define Thread_RestoreContext() \
ASM("lds r26, g_pstCurrent"); \
ASM("lds r27, g_pstCurrent + 1");\
ASM("adiw r26, 4"); \
ASM("ld     r28, x+"); \
ASM("out 0x3D, r28"); \
ASM("ld     r29, x+"); \
ASM("out 0x3E, r29"); \
ASM("pop r31"); \
ASM("pop r30"); \
ASM("pop r29"); \
ASM("pop r28"); \
ASM("pop r27"); \
ASM("pop r26"); \
ASM("pop r25"); \
ASM("pop r24"); \
ASM("pop r23"); \
ASM("pop r22"); \
ASM("pop r21"); \
ASM("pop r20"); \
ASM("pop r19"); \
ASM("pop r18"); \
ASM("pop r17"); \
ASM("pop r16"); \
ASM("pop r15"); \
ASM("pop r14"); \
ASM("pop r13"); \
ASM("pop r12"); \
ASM("pop r11"); \
ASM("pop r10"); \
ASM("pop r9"); \
ASM("pop r8"); \
ASM("pop r7"); \
ASM("pop r6"); \
ASM("pop r5"); \
ASM("pop r4"); \
ASM("pop r3"); \
ASM("pop r2"); \
ASM("pop r1"); \
ASM("pop r0"); \
ASM("out __SREG__, r0"); \
ASM("pop r0");

//------------------------------------------------------------------------
//! These macros *must* be used in pairs !
//------------------------------------------------------------------------
//! Enter critical section (copy status register, disable interrupts)
#define CS_ENTER()    \
{ \
volatile K_UCHAR x; \
x = _SFR_IO8(SR_); \
ASM("cli");
//------------------------------------------------------------------------
//! Exit critical section (restore status register)
#define CS_EXIT() \
_SFR_IO8(SR_) = x;\
}

//------------------------------------------------------------------------
//! Initiate a contex switch without using the SWI
#define ENABLE_INTS()        ASM("sei");
#define DISABLE_INTS()       ASM("cli");

//------------------------------------------------------------------------
class Thread;
/*!
    Class defining the architecture specific functions required by the 
    kernel.  
    
    This is limited (at this point) to a function to start the scheduler,
    and a function to initialize the default stack-frame for a thread.
*/
class ThreadPort
{
public:
    /*!        
        \fn void StartThreads()
       
        Function to start the scheduler, initial threads, etc.
    */
    static void StartThreads();
    friend class Thread;
private:

    /*!
        \fn void InitStack(Thread *pstThread_)
        
        Initialize the thread's stack.
        
        \param pstThread_ Pointer to the thread to initialize
    */
    static void InitStack(Thread *pstThread_);
};

#endif //__ThreadPORT_H_
