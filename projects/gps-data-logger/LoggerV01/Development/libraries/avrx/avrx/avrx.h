#ifndef AVRXCHEADER
#define AVRXCHEADER

/*
avrx.h

Function prototypes for AvrX C library

Copyright ©1998 - 2002 Larry Barello (larry@barello.net)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.

http://www.gnu.org/copyleft/lgpl.html

Author: Larry Barello
        larry@barello.net

Revision History
    Sep 15 2002 - lb
        Modified declaration of TaskControlBlock and swapped the 'const' and 'FLASH'
        keywords as that was, apparently, not working in GCC 3.2
    Mar 05 2001 - lb
        Modified declaration of Epilog() to take in account larger chips and use
        the JMP instead of the RJMP instruction

        Added kernel variable AvrXStack and an API to initialize it a pointer to
        the current stack or a stack of your choosing.

    Aug 11 2000- lb
        The data structure for ProcessID used the macro define
        DEBUG to conditionally enable the breakpoint structure.  That should
        have been SINGLESTEPSUPPORT to match the assembly code.

        The data structure MessageControlBlock member Data was removed.  This
        is a vestigal data location.  See example "MessageAndData.c" for an
        example of how to extend the MCB with user data structures.

*/
#if !defined(BV)
#  define BV(A) (1<<A)
#endif

// copied here Feb05 - as not in WinAVR Feb05

#  define outp(A,B) B = (A)
#  define inp(A) A
#  define cbi(P, B) P &= ~BV(B)
#  define sbi(P, B) P |= BV(B)

#ifdef __IAR_SYSTEMS_ICC__

#  define FLASH  __flash
#  define EEPROM __eeprom
#  define NAKEDFUNC(A) __C_task __root void A(void)
#  define INTERFACE __version_1

/* Transform GCC input and output routines into direct access for IAR */

#  define outp(A,B) B = (A)
#  define inp(A) A
#  define cbi(P, B) P &= ~BV(B)
#  define sbi(P, B) P |= BV(B)
#  define BeginCritical() __disable_interrupt()
#  define EndCritical()   __enable_interrupt()

#else   /* Gcc */


#  define FLASH  __attribute__ ((progmem))
#  define EEPROM __attribute__ ((section(".eeprom")))
#  define NAKED  __attribute__ ((naked))
#  define NAKEDFUNC(A) void A(void) NAKED;\
    void A(void)
#  define INTERFACE
#  include <avr\io.h>  // was <iomacros.h> (jun04)
#  define BeginCritical() asm ("cli\n")
#  define EndCritical()   asm ("sei\n")


#endif

/*
    void * AvrXSetKernelStack(char *newstack)

    Sets AvrX Stack to "newstack" or, if NULL then to the current stack
*/
#ifdef __cplusplus
extern "C" {
#endif

void *AvrXSetKernelStack(void *bval);

#ifdef __cplusplus
}
#endif

typedef struct AvrXKernelData
{
    struct ProcessID *RunQueue;
    struct ProcessID *Running;
    void             *AvrXStack;
    unsigned char    SysLevel;
#ifdef __IAR_SYSTEMS_ICC__
    unsigned void    *AvrXCStack;
#endif
} AvrXKernelData;

/*
    The process id is a chunk of eram that contains the state
    of a process.
*/
typedef struct ProcessID
{
    struct ProcessID  *next;
    unsigned char flags, priority;
    void *ContextPointer;
#ifdef SINGLESTEPSUPPORT
    unsigned char *bp1;
    unsigned char *bp2;
#endif
}
* pProcessID, ProcessID;

#define NOMESSAGE ((pMessageControlBlock)0)
#define NOTIMER ((pTimerControlBlock)0)
#define NOPID ((pProcessID)0)
#define SEM_PEND ((Mutex)0)
#define SEM_DONE ((Mutex)1)
#define SEM_WAIT ((Mutex)2)     // AvrXTestSemaphore(): Something waiting

/*
 Mutex semaphores are a simple linked list of waiting
 processes.  The mutex may have the following values:

 SEM_PEND         // Semaphore is reset waiting for a signal
 SEM_DONE         // Semaphore has been triggered.
                  // Any other value is the address of a processID
*/
typedef pProcessID Mutex, *pMutex;     /* A mutex is basically a pointer to a process */

#define AVRX_MUTEX(A)\
        Mutex A

#ifdef __cplusplus
extern "C" {
#endif




INTERFACE void AvrXSetSemaphore(pMutex);
INTERFACE void AvrXIntSetSemaphore(pMutex);
INTERFACE void AvrXWaitSemaphore(pMutex);
INTERFACE Mutex AvrXTestSemaphore(pMutex);
INTERFACE Mutex AvrXIntTestSemaphore(pMutex);
INTERFACE void AvrXResetSemaphore(pMutex);
INTERFACE void AvrXResetObjectSemaphore(pMutex);

#ifdef __cplusplus
}
#endif
/*
    Message Queues are really an extension of Semaphore
    queue.  They can be used for mutual exlusion (have a
    message be the baton) or as a general purpose message
    passing system.  This is needed for tasks that must block
    waiting for several things at once (timer, interrupt, etc)
*/
typedef struct MessageControlBlock
{
    struct MessageControlBlock *next;
    Mutex semaphore;
/*    unsigned char data;     08/11/00 lb */
}
* pMessageControlBlock, MessageControlBlock;

// Lame macro...  Should just declare directly.

#define AVRX_MESSAGE(A) /* 08/11/00 lb */\
        MessageControlBlock A

typedef struct MessageQueue
{
    pMessageControlBlock message;    /* List of messages */
    pProcessID pid;        /* List of processes */
}
* pMessageQueue, MessageQueue;

#define AVRX_MESSAGEQ(A)\
        MessageQueue A

#ifdef __cplusplus
extern "C" {
#endif

INTERFACE pMessageControlBlock AvrXRecvMessage(pMessageQueue);
INTERFACE pMessageControlBlock AvrXWaitMessage(pMessageQueue);
INTERFACE void AvrXSendMessage(pMessageQueue, pMessageControlBlock);
INTERFACE void AvrXIntSendMessage(pMessageQueue, pMessageControlBlock);
INTERFACE void AvrXAckMessage(pMessageControlBlock);
INTERFACE void AvrXWaitMessageAck(pMessageControlBlock);
INTERFACE Mutex AvrXTestMessageAck(pMessageControlBlock);

#ifdef __cplusplus
}
#endif


/*
    The timer queue manager is a service run in kernel mode and is tuned
    to minimize interrupt latency while queueing, tracking and dequeuing
    timers
*/
typedef struct TimerControlBlock
{
    struct TimerControlBlock *next;
    Mutex semaphore;
    unsigned short count;
}
* pTimerControlBlock, TimerControlBlock;
/*
    A special version of timers that send messages rather than firing
    a semaphore.
*/
typedef struct TimerMessageBlock
{
    union
    {
        struct MessageControlBlock mcb;
        struct TimerControlBlock tcb;
    } u;
    struct MessageQueue *queue;
}
* pTimerMessageBlock, TimerMessageBlock;


#ifdef __cplusplus
extern "C" {
#endif

#define AVRX_TIMER(A) TimerControlBlock A



INTERFACE void AvrXStartTimer(pTimerControlBlock, unsigned);
INTERFACE pTimerControlBlock AvrXCancelTimer(pTimerControlBlock);
INTERFACE void AvrXDelay(pTimerControlBlock, unsigned);
INTERFACE void AvrXWaitTimer(pTimerControlBlock);
INTERFACE Mutex AvrXTestTimer(pTimerControlBlock);

INTERFACE void AvrXTimerHandler(void);    // Kernel Funtion called by timer ISR

// Special versions of timer queue elements that get sent
// to a message queue when expired.

INTERFACE void AvrXStartTimerMessage(pTimerMessageBlock timer, unsigned timeout, pMessageQueue queue);
INTERFACE pMessageControlBlock AvrXCancelTimerMessage(pTimerMessageBlock, pMessageQueue);

#ifdef __cplusplus
}
#endif
/*
   The Task Control Block contains all the information needed
   to initialize and run a task.  It is stored in FLASH and is
   used only by AvrXInitTask()
*/
typedef struct TCB_s
{
    void *r_stack;          // Start of stack (top address-1)
    void (*start) (void);   // Entry point of code
#ifdef __IAR_SYSTEMS_ICC__
    void *c_stack;
#endif
    pProcessID pid;         // Pointer to Process ID block
    unsigned char priority;       // Priority of task (0-255)
}
FLASH const TaskControlBlock;
/*
    A series of macro's to ease the declaration of tasks
    and access to the resulting data structures.

AVRX_TASK(start, stacksz, priority)
	Declare task data structures and forward reference to task
AVRX_TASKDEF(start, stacksz, priority)
	Declare task data structure and the top level C
	declaration (AVRX_TASK + C function declaration)
AVRX_SIGINT(vector)
	Declare the top level C declaration for an
	interrupt handler
AVRX_EXTERNTASK(start)
	Declare external task data structures
PID(start)
	Return the pointer to the task PID
TCB(start)
	Return the pointer to the task TCB
*/

#define MINCONTEXT 35           // 32 registers, return address and SREG
#ifdef __IAR_SYSTEMS_ICC__
#  define AVRX_GCC_TASK(A, B, C)
#  define AVRX_IAR_TASK(start, c_stack, r_stack, priority) \
    char start ## Stk [c_stack + r_stack + MINCONTEXT ]; \
    NAKEDFUNC(start); \
    ProcessID start ## Pid; \
    TaskControlBlock start ## Tcb = \
    { \
        &start##Stk[sizeof(start##Stk)-1] , \
        start, \
        &start##Stk[c_stack] , \
        &start##Pid, \
        priority \
    }
#else
#  define AVRX_IAR_TASK(A, B, C, D)
#  define AVRX_GCC_TASK(start, c_stack, priority)	\
    char start ## Stk [c_stack + MINCONTEXT] ; \
    NAKEDFUNC(start); \
    ProcessID start ## Pid; \
    TaskControlBlock start ## Tcb = \
    { \
        &start##Stk[sizeof(start##Stk)-1] , \
        start, \
        &start##Pid, \
        priority \
    }
#endif

#ifdef __IAR_SYSTEMS_ICC__
#   define AVRX_GCC_TASKDEF(A, B, C)
#   define AVRX_IAR_TASKDEF(start, c_stack, r_stack, priority) \
    AVRX_IAR_TASK(start, c_stack, r_stack, priority); \
    NAKEDFUNC(start)
#else
#   define AVRX_IAR_TASKDEF(A, B, C, D)
#   define AVRX_GCC_TASKDEF(start, c_stack, priority) \
    AVRX_GCC_TASK(start, c_stack, priority); \
    NAKEDFUNC(start)
#endif

#define AVRX_SIGINT(vector)\
  NAKEDFUNC(vector)

#define PID(start) &start##Pid
#define TCB(start) (&start##Tcb)

#define AVRX_EXTERNTASK(start) \
  void start(void); \
  extern TaskControlBlock start##Tcb; \
  extern ProcessID start##Pid

#ifdef __cplusplus
extern "C" {
#endif


INTERFACE void AvrXRunTask(TaskControlBlock *);
INTERFACE unsigned char AvrXInitTask(TaskControlBlock *);

INTERFACE void AvrXResume(pProcessID);
INTERFACE void AvrXSuspend(pProcessID);
INTERFACE void AvrXBreakPoint(pProcessID);
INTERFACE unsigned char AvrXSingleStep(pProcessID);
INTERFACE unsigned char AvrXSingleStepNext(pProcessID);

INTERFACE void AvrXTerminate(pProcessID);
INTERFACE void AvrXTaskExit(void);
INTERFACE void AvrXHalt(void);     // Halt Processor (error only)

INTERFACE void AvrXWaitTask(pProcessID);
INTERFACE Mutex AvrXTestPid(pProcessID);

INTERFACE unsigned char AvrXPriority(pProcessID);
INTERFACE unsigned char AvrXChangePriority(pProcessID, unsigned char);
INTERFACE pProcessID AvrXSelf(void);
INTERFACE void IntProlog(void);

INTERFACE void _Epilog(void);    // Not to be used for C code
INTERFACE void Epilog(void);

/*
 AvrX provides some EEPROM access routines that
 control access to the hardware via a semaphore
 This semaphore needs to be "set" prior to using
 the access routines.
 */
extern Mutex EEPromMutex;

INTERFACE unsigned char AvrXReadEEProm(unsigned char *);
INTERFACE unsigned int AvrXReadEEPromWord(unsigned *);
INTERFACE void AvrXWriteEEProm(unsigned char *, char);

#ifdef __cplusplus
}
#endif

/*
 Declare internal AvrX data so the Debugger can display their contents.

 */
extern pTimerControlBlock _TimerQueue;
extern unsigned char SysLevel, _TimQLevel;

#endif /* AVRXCHEADER */
