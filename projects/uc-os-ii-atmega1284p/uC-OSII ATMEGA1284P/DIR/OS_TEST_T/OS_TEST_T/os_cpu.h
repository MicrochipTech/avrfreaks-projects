/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                                         AVR Specific code
*
* File         : OS_CPU.H
* By           : Ole Saether
* Port Version : V1.01
*
* AVR-GCC port version : 1.0 	2001-04-02 modified/ported to avr-gcc by Jesper Hansen (jesperh@telia.com)
*
*
*********************************************************************************************************
*/

/* Definition moved here so it can be used in the assembler file OS_CPU_A.ASM 							*/
/* See below for the meaning of this define 															*/

#define  OS_CRITICAL_METHOD    1

#ifndef OS_CPU_A	/* skip the rest if we're including from the assembler file 						*/

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*
* IMPORTANT: These prototypes MUST be placed in OS_CPU.H
*********************************************************************************************************
*/
//Prototipos
// En ucos_ii.h dice que estos prototipos deben ir aqui
void          OSStartHighRdy          (void);
void          OSIntCtxSw              (void);
void          OSCtxSw                 (void);

/*
**********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
**********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                       /* Unsigned  8 bit quantity                         */
typedef signed   char  INT8S;                       /* Signed    8 bit quantity                         */
typedef unsigned int   INT16U;                      /* Unsigned 16 bit quantity                         */
typedef signed   int   INT16S;                      /* Signed   16 bit quantity                         */
typedef unsigned long  INT32U;                      /* Unsigned 32 bit quantity                         */
typedef signed   long  INT32S;                      /* Signed   32 bit quantity                         */
typedef float          FP32;                        /* Single precision floating point                  */

typedef unsigned char  OS_STK;                      /* Each stack entry is 8-bit wide                   */

/*
**********************************************************************************************************
*                                       Atmel AVR
*
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts. In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section. Here's what happens in the macro below :
*
*             OS_ENTER_CRITICAL:
*
*               input SREG to a free register, Rn
*				push Rn
*
*             OS_EXIT_CRITICAL:
*
*				pop	to a free register, Rn
*				output Rn to SREG
*
*
**********************************************************************************************************
*/

#if      OS_CRITICAL_METHOD == 1
#define  OS_ENTER_CRITICAL()    asm volatile ("cli")		/* Disable interrupts						*/
#define  OS_EXIT_CRITICAL()     asm volatile ("sei")        /* Enable  interrupts						*/
#endif

#if      OS_CRITICAL_METHOD == 2
#define  OS_ENTER_CRITICAL()    {	asm volatile (		\
									"in %0,63" "\n\t"	\
									"cli" "\n\t" 		\
									"push %0" "\n\t" 	\
									: /*no outputs*/	\
									: "r" (0) );		\
								 }
								 
#define  OS_EXIT_CRITICAL()     {	asm volatile (		\
									"pop %0"   	"\n\t"	\
									"out 63,%0" "\n\t"	\
									: /*no outputs*/	\
									: "r" (0) );		\
								  }
#endif

/*
**********************************************************************************************************
*                           AVR Miscellaneous
**********************************************************************************************************
*/

#define  OS_STK_GROWTH      1                       /* Stack grows from HIGH to LOW memory on AVR       */

#define  OS_TASK_SW()       OSCtxSw()


#endif //OS_CPU_A
