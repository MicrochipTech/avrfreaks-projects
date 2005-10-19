#ifndef __AVRXINC
#define __AVRXINC
#include "avrx-ctoasm.inc"
/*

Copyright ©1998-2002 Larry Barello

Author: Larry Barello
        larry@barello.net

20020907 - Added _IOMACRO_H_ definition to prevent older GCC io.h files
	   from including C only header information

	 - Added #defines for _SFR_IO_ADDR() macro to make it a NOP for
	   pre GCC 3.2 compilers and for IAR (I wonder what they do for
	   extended I/O access?)

	   THIS works because avrx.inc loads "io.h" which loads "sfr-defs.h"
	   which defines _SFR_IO_ADDR() macro on the 3.2 compiler.
*/
// Define _IOMACROS_H_ to prevent GCC 3.02 io.h from including this
// C only file when using io.h for assembly code.  io.h called within
// avrx-io.h
#define _IOMACROS_H_
#include "avrx-io.h"

#ifdef __IAR_SYSTEMS_ASM__

__global  MACRO Lable, Size
//        EXTERN  `?<Segment init: NEAR_Z>`
//	REQUIRE `?<Segment init: NEAR_Z>`
        PUBLIC Lable
        RSEG NEAR_Z:DATA:NOROOT(0)
Lable:  DS Size
        ENDM

__static MACRO Lable, Size
//        EXTERN  `?<Segment init: NEAR_Z>`
//	REQUIRE `?<Segment init: NEAR_Z>`
        RSEG NEAR_Z:DATA:NOROOT(0)
Lable:  DS Size
        ENDM

#  define _GLOBAL(Lable, Size) __global Lable, Size
#  define _STATIC(Lable, Size) __static Lable, Size
#  define _DATASECTION RSEG DATA:DATA
#  define _CODESECTION RSEG CODE:CODE
#  define _FUNCTION(A) PUBLIC A
#  define _ENDFUNC
#  define _PUBLIC(A) PUBLIC A
#  define _EXTERN(A) EXTERN A
#  define _MODULE(A) MODULE A
#  define _ENDMOD ENDMOD
#  define _END END
#  define lo8_pm(A) LOW(A/2)
#  define hi8_pm(A) HIGH(A/2)
#  define lo8(A) LOW(A)
#  define hi8(A) HIGH(A)
#  define _DB DB
#  define _EQU(A,B) A: EQU B
#  define _ALIGN EVEN
#  define _SFR_IO_ADDR(A) (A)   /* IAR does not use this */

#else   /*avr-gcc*/

#  if !defined(_SFR_IO_ADDR)      /* GCC 3.2 compatibility */
#    define _SFR_IO_ADDR(A) (A)   /* earlier compilers do not use this */
#  endif
#  define _GLOBAL(Lable, Size) .comm Lable, Size
#  define _STATIC(Lable, Size) .lcomm Lable, Size
#  define _DATASECTION
#  define _CODESECTION
#  define _FUNCTION(A) \
        .global A $ \
        .func   A
#  define _ENDFUNC .endfunc
#  define _PUBLIC(A) .global A
#  define _EXTERN(A)
#  define _MODULE(A) .section .text.avrx.A
#  define _ENDMOD
#  define _END
#  define _DB .asciz
#  define MACRO .macro
#  define ENDM .endm
#  define _EQU(A,B) .equ A, B
#  define _ALIGN .balign 2

#endif

#ifndef BV
#define BV(A) (1<<A)
#endif

/*
 Frame Pointer Offsets for various registers after the return from
 a call to _Prolog

 Used mainly to restore registers used internally (rather than saving
 and restoring) and to stuff return values across _Epilog.

*/
#define _RetLo  33      /* Return address on stack big endian */
#define _R31    33      /* Registers saved in order (little endian) */
#define _RetHi  32
#define _R30    32
#define _R29    31
#define _R28    30
#define _R27    29
#define _R26    28
#define _R25    27
#define _R24    26
#define _R23    25
#define _R22    24
#define _R0     2
#define _SREG   1       /* SP points below last saved item */

#ifdef __IAR_SYSTEMS_ASM__

#  define _p1l     18
#  define _p1h     19
#  define _p2l     22
#  define _p2h     23
#  define _r1l     18
#  define _r1h     19

#else   /* avr-gcc */

#  define _p1l     26
#  define _p1h     27
#  define _p2l     24
#  define _p2h     25
#  define _r1l     26
#  define _r1h     27

#endif

/* ******** AVRX kernel data structure offsets
   These are offsets into a slab of memory called "AvrXKernelData"
   defined in avrx_tasking.s

   NB: CODE depends upon RunQueue being the FIRST item!
*/

#define RunQueue 0	/* Head of the run queue */
#define Running 2	/* Current running task */
#define AvrXStack 4	/* User defined stack location */
#define SysLevel 6	/* re-entry counter into kernel context */
#define AvrXCStack 7	/* IAR/ICC C Parameter stack */

#ifdef  __IAR_SYSTEMS_ASM__
#	define AvrXKernelDataSz 9
#else
#	define AvrXKernelDataSz 7
#endif

/* ******** TCB (Task Control Block) offsets */

#define TaskSP          0       /* Stack pointer */
#define TaskMain        2       /* Entry point of process */
#define TaskPid         4       /* Pointer to Process ID */
#define TaskFlags       6       /* Priority */

#define TaskSz  7

/* All objects have same link list structure
   Change this to convert to big-endian for direct C access
*/

#define NextL 0		/* Big-endian for C compatibility */
#define NextH 1

/******** PID (Process ID) block offsets */

#define PidNext         0       /* Next item on list (semaphore, run) */
#define PidState        2       /* Upper Nibble: Task flags, Lower Nibble :Priority */
#define PidPriority     3
#define PidSP           4       /* Context Pointer */
#ifdef SINGLESTEPSUPPORT
#   define bp1          6       /* Make room for two breakpoints/task */
#   define bp2          8
#   define PidSz        10
#else
#   define PidSz        6
#endif

/* ******* PID (Process ID) BLOCK BIT DEFINITIONS ******* */

/* PID.State  */

#define IdleBit         4       /* Dead Task, don't schedule, resume or step */
#define SuspendBit      5       /* Mark task for suspension (may be blocked elsewhere) */
#define SuspendedBit    6       /* Mark task suspended (it was removed from the run queue) */
#define SingleStep      7       /* Enable single step debug support */

/*+ --------------------------------------------------
SEMAPHORE BIT DEFINITIONS

 General Semaphore Flags.  When a task is waiting on the
 semaphore, the semaphore is set to the TASK ID.  Otherwise
 _PEND means the semaphore is set and _DONE means it has
 been triggered.  A task will only block if it tries to
 grab a _PEND semaphore.  Semaphores are initialized in the
 _PEND state and are left in the _PEND state after each
 complete operation.  Processes are allowed to queue on
 a semaphore.  This effectively make the semaphore a Mutex
 when it has been "set" prior to the first call.  When a process
 done with the mutex, it releases it by "setting" it again.
-*/
#define _PEND           0
#define _DONE           1

#define MtxSz           2

#define MUTEX(A) _GLOBAL(A, 2)

/*+ --------------------------------------------------

 Events

 Events replace PID's in semaphores.  The kernel recognizes
 semaphore values (contents) < 0x60 as events, indexes into
 a table and calls a routine.  Macros and segments are used
 to build the table.

 */
#define TIMERMESSAGE_EV 2
#define PERIODICTIMER_EV 3      /* Not implemented */
#define _LASTEV 0x5F

/* Timers */

#define TcbNext         0       /* Pointer in linked list */
#define TcbSemaphore    2       /* Associated semaphore */
#define TcbCount        4       /* Timer ticks till expired */
#define TcbQueue        6
#define TcbSz           6       /* Primitive Timer */
#define TmbSz           8       /* Timer Message */

#define TIMER(A) _GLOBAL(A, TcbSz)

#define TIMERMESSAGE(A) _GLOBAL(A, TmbSz)

/* Message Queue */

#define MsqSz           4       /* Head of message queue */

#define MsqMessage      0       /* Head of list of messages */
#define MsqPid          2       /* Head of list of waiting processes */

#define QcbSz           4       /* Queue Block Size (No data) */

#define QcbNext         0
#define QcbSemaphore    2       /* Return Receipt Semaphore */
#define QcbData         4       /* pointer to data/or data */

#define MESSAGEQ(A) _GLOBAL(A, MsqSz)

/* Fifos */

#define FifoSz 4        /* Size of AvrX Fifo Data Struct */

#define WriteSemaphore 0
#define ReadSemaphore 2
#define fBuffer 4

/* User Section */

#define f_buffersz 4    /* Size of buffer header */

#define f_Size 0
#define f_Count 1
#define f_In 2
#define f_Out 3
#define f_Data 4        /* Variable length */

#define FIFO(A, B, Size) \
        _GLOBAL(A, FifoSz) \
        _GLOBAL(B, (Size+f_Buffersz))

#ifndef AVRXCHEADER
/*+ --------------------------------------------------
 Handy Macros

 DEBUG statements are diagnostics for twiddling and output port bit
 to reflect the I bit state (interrupt latency) and when AvrX is in
 the Idle loop
-*/
#ifdef  DEBUG           /* DEBUG is set to a PORT{ABCD} value (for twiddling output bits) */

#ifndef ILED
#define ILED            PB0
#endif

#ifndef IDLELED
#define IDLELED         PB4
#endif

#define DEBUG_DDR       (DEBUG-1)

#endif  /* DEBUG */

        MACRO  Breakpoint
#ifdef  DEBUG
        rcall   AvrXBreakpoint
#endif  /* DEBUG */
        ENDM

        MACRO  BeginInterrupt
#ifdef  DEBUG
        cbi     DEBUG, ILED
        sbi     DEBUG, IDLELED  /* Obviously not idle... */
#endif  /* DEBUG */
        ENDM

        MACRO  EndInterrupt
#ifdef  DEBUG
        sbi     DEBUG, ILED
#endif  /* DEBUG */
        reti
        ENDM

        MACRO BeginCritical
#ifdef  DEBUG
        cbi     DEBUG, ILED
#endif  /* DEBUG */
        cli     ;
        ENDM

        MACRO  EndCritical
#ifdef  DEBUG
        sbi     DEBUG, ILED
#endif  /* DEBUG */
        sei
        ENDM

#define EndCriticalReturn EndInterrupt

        MACRO  BeginIdle
snore:
#ifdef  DEBUG
        cbi     DEBUG, IDLELED
#endif  /* DEBUG */
        ENDM

        MACRO  EndIdle
snooze:
#ifdef  DEBUG
        sbi     DEBUG, IDLELED
#endif  /* DEBUG */
        ENDM

        MACRO  INIT_DEBUG
#ifdef  DEBUG
        ldi     Xl, (1<<ILED | 1<<IDLELED)
        out     DEBUG_DDR, Xl
        out     DEBUG, Xl
#endif  /* DEBUG */
        ENDM

/*+ --------------------------------------------------
 Handy macros for common assembly chores
-*/
        MACRO  SignExtend Dest, Source
        clr     Dest
        sbrc    Source, 7
        com     Dest
        ENDM

        MACRO  addi reg, val
        subi    reg, -(val)
        ENDM

        MACRO  adci reg, val
        sbci    reg, -(val)
        ENDM

        MACRO  tstw reg
        adiw    reg, 0
        ENDM

        MACRO  negw regh, regl
        com     regh
        neg     regl
        sbci    regh, -1
        ENDM
/*
 Use this macro rather than a call to _Prolog, see
 version notes in AvrX.asm
*/
        MACRO  AVRX_Prolog
        BeginCritical
        rcall   IntProlog
        EndCritical
        ENDM
/*
 These register definitions are just handy aliases for the
 various index and word math registers
 */
#define		Wl 		R24
#define		Wh 		R25
#define		Xl 		R26
#define		Xh 		R27
#define		Yl 		R28
#define		Yh 		R29
#define		Zl 		R30
#define		Zh 		R31

#endif /*AVRXCHEADER */
#endif  /* __AVRXINC */
