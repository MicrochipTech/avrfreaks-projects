/*
**********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                                           AVR Specific code
*
* File         : AVR_ISR.H
* By           : Julius Luukko (Julius.Luukko@lut.fi)
* Compiler     : avr-gcc version 3.x (tested with 3.3)
*
* This file containes macros for using ISRs with C.
*
* Modifications by Julius Luukko 2003-03-18 (Julius.Luukko@lut.fi):
*
* - I bit of SREG is set in the macro PUSHRS
*
* Modifications by Julius Luukko 2003-06-24 (Julius.Luukko@lut.fi):
*
* - RAMPZ is only pushed and popped if it is defined, i.e. with chips that have it
*
**********************************************************************************************************
*/


/*
 * Macros for declaring a "naked" ISR: registers are not saved and restored and a ret command is missing.
 *
 */
#define UCOSISR(signame)			\
void signame (void) __attribute__ ((naked));	\
void signame (void)

/*
 * Macros for pushing and popping registers in application ISRs.
 *
 * Do not use pushregs(), pushrampz(), pushsreg() directly.
 *
 * Use PushRS() and PopRS() in application programs.
 */

#define pushregs() \
        __asm__ __volatile__ ( \
        "push   r0"    "\n\t" \
        "push   r1"    "\n\t" \
        "push   r2"    "\n\t" \
        "push   r3"    "\n\t" \
        "push   r4"    "\n\t" \
        "push   r5"    "\n\t" \
        "push   r6"    "\n\t" \
        "push   r7"    "\n\t" \
        "push   r8"    "\n\t" \
        "push   r9"    "\n\t" \
        "push   r10"    "\n\t" \
        "push   r11"    "\n\t" \
        "push   r12"    "\n\t" \
        "push   r13"    "\n\t" \
        "push   r14"    "\n\t" \
        "push   r15"    "\n\t" \
        "push   r16"    "\n\t" \
        "push   r17"    "\n\t" \
        "push   r18"    "\n\t" \
        "push   r19"    "\n\t" \
        "push   r20"    "\n\t" \
        "push   r21"    "\n\t" \
        "push   r22"    "\n\t" \
        "push   r23"    "\n\t" \
        "push   r24"    "\n\t" \
        "push   r25"    "\n\t" \
        "push   r26"    "\n\t" \
        "push   r27"    "\n\t" \
        "push   r28"    "\n\t" \
        "push   r29"    "\n\t" \
        "push   r30"    "\n\t" \
        "push   r31"    "\n\t" \
        ::)

#ifdef RAMPZ
#define pushrampz() __asm__ __volatile__ ( \
        "in     r16,0x3b"    "\n\t" \
        "push   r16"    "\n\t" \
        ::)
#endif

#define pushsreg() __asm__ __volatile__ ( \
        "in            r16,__SREG__"    "\n\t" \
        "sbr    r16,0x80"    "\n\t" \
        "push   r16"    "\n\t" \
        ::)


#define popregs() \
        __asm__ __volatile__ ( \
        "pop        r31"    "\n\t" \
        "pop        r30"    "\n\t" \
        "pop        r29"    "\n\t" \
        "pop        r28"    "\n\t" \
        "pop        r27"    "\n\t" \
        "pop        r26"    "\n\t" \
        "pop        r25"    "\n\t" \
        "pop        r24"    "\n\t" \
        "pop        r23"    "\n\t" \
        "pop        r22"    "\n\t" \
        "pop        r21"    "\n\t" \
        "pop        r20"    "\n\t" \
        "pop        r19"    "\n\t" \
        "pop        r18"    "\n\t" \
        "pop        r17"    "\n\t" \
        "pop        r16"    "\n\t" \
        "pop        r15"    "\n\t" \
        "pop        r14"    "\n\t" \
        "pop        r13"    "\n\t" \
        "pop        r12"    "\n\t" \
        "pop        r11"    "\n\t" \
        "pop        r10"    "\n\t" \
        "pop        r9"    "\n\t" \
        "pop        r8"    "\n\t" \
        "pop        r7"    "\n\t" \
        "pop        r6"    "\n\t" \
        "pop        r5"    "\n\t" \
        "pop        r4"    "\n\t" \
        "pop        r3"    "\n\t" \
        "pop        r2"    "\n\t" \
        "pop        r1"    "\n\t" \
        "pop        r0"    "\n\t" \
    "ret"          "\n\t" \
        ::)

#ifdef RAMPZ
#define poprampz() \
        __asm__ __volatile__ ( \
        "pop        r16"    "\n\t" \
        "out        0x3b,r16"    "\n\t" \
        ::)
#endif

#define popsreg() \
        __asm__ __volatile__ ( \
        "pop        r16"    "\n\t" \
        "out        __SREG__,r16"    "\n\t" \
        ::)

/*
 * Macros for pushing and popping registers in application ISRs.
 *
 */

#ifdef RAMPZ
#define PushRS() {\
   pushregs();\
   pushrampz (); \
   pushsreg();}
#else
#define PushRS() {\
   pushregs();\
   pushsreg();}
#endif

#ifdef RAMPZ
#define PopRS() {\
   popsreg();\
   poprampz (); \
   popregs();}
#else
#define PopRS() {\
   popsreg();\
   popregs();}
#endif


/*
 * Macro for saving the stack pointer into the current stack's TCB. Used in application ISRs.
 * Just could as well be done directly in C: 
 *
 * OSTCBCur->OSTCBStkPtr = (OS_STK *)SP;
 *
 * provided that avr/io.h is included.
 *
 */
#define SaveSP() \
        __asm__ __volatile__ ( \
     "in    %A0,__SP_L__"    "\n\t" \
     "in    %B0,__SP_H__"    "\n\t" \
        :"=r" (OSTCBCur->OSTCBStkPtr):)

