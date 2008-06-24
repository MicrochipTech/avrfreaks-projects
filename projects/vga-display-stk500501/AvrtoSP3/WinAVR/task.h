#ifndef TASK__H
#define TASK__H

#include "pq.h"

#define LO(x)		(x & 0x0ff)
#define HI(x)		((x >> 8) & 0x0ff)

#define STACK_SIZE			96		//number of 32 bit words in stack

#define EVENT_NOERROR		0
#define	EVENT_NOTASKS		-1
#define EVENT_OVERFLOW		-2
#define EVENT_TIMEOUT		-3
#define EVENT_DELETED		-4



/*------------------------------------------------------------------
** Definition of a Task Control Block (TCB)
**----------------------------------------------------------------*/

typedef struct tcb {
	char *stack;			/*	points to current position in stack	*/
	char *stacktop;		/*	points to the end of the stack		*/
	int stacksize;		/*	SIZE of the stack in words			*/
	int priority;		/*	task priority	*/
	int status;			/*	task status, set by semaphore mostly	*/
	unsigned misc;
	unsigned TimeStamp;		/*	system time stamp, set when swapped	*/
	unsigned TcbSwaps;		/*	number of times tasks swapped in	*/
	int timeout;        /*	timeout counter to trigger event	*/
	char *name;			/*	name of task	*/
	struct tcb *next;
	struct tcb *list;	/*	master linked list of tasks	*/
} TCB;

/*-------------------------------------------------------------------
** Semaphore Object
**-----------------------------------------------------------------*/
typedef struct ecb {
	TCB *task_h;		//head pointer for list of waiting tasks
	TCB *task_t;		//tail pointer for list of waiting tasks
	struct ecb *Tnext,*Tprev;	//Link list pointers for timeout list
	int SemaphoreMode;		//mode of semaphore
	int EventCount;			//counter for semaphore
	char name[20];
} ECB;

#define SEMAPHORE_MODE_BLOCKING		0
#define SEMAPHORE_MODE_TIMEOUT		1

/*-------------------------------------------------------------------
** Function Prototypes
**------------------------------------------------------------------*/

extern ECB *MasterList;	//head pointer for master semaphore list
extern ECB *TimeoutList;	//head pointer for timeout semaphore list
extern PQ ActiveTasks;		//priority task queue
extern TCB *CurrentTask,*NextTask;
extern ECB *LCDBlocker;
extern int OsRunning;
extern volatile int TStamp;

extern volatile int InterruptCount;
extern volatile int Blocking;

extern void CreateTask(TCB *t, void (*task)(void *),char *stack, int stacksize,int priority,char *name,void *arg);
extern void IrqSwap(void);
extern void InitTask(void);
extern void Yeild(void);
extern ECB *NewSemaphore(int InitCount,int Mode,char *name);
extern void CreateSemaphore(ECB *e,int InitCount,int Mode,char *n);
extern int PendSemaphore(ECB *e, int Timeout);	//wait for semaphore availiable
extern int PostSemaphore(ECB *e, int Value);	//signal semaphore availiable
extern int PostSemaphoreWithData(ECB *e, int Value,unsigned data);	//signal semaphore availiable
extern void TimerTicker(void);
extern void vPortYieldProcessor(void);
extern void vPortStartFirstTask(void);
extern int TimeDelay(int mSec);
extern void Enable(char sr);
extern char Disable(void);
extern void StartOS(void);
extern void DoSwap(void);
extern void ExitInterrupt(void);

//----------------------------------------------------------------
// Macros that are used in interrupt routines for saving
// and restoring the context
// it should be noted in the save IRQ sequence that we
// set the interrupt enable bit in the value we get from
// the status register.  If we got here, interrupts were
// enabled, so we want to make sure that is how we
// exit.
//----------------------------------------------------------------

#define SAVE_IRQ()									\
	asm volatile (	"push	r31						\n\t"	\
					"in		r31, __SREG__			\n\t"	\
					"ori	r31,0x80					\n\t"	\
					"push	r31						\n\t"	\
					"push	r30						\n\t"	\
					"push	r29						\n\t"	\
					"push	r28						\n\t"	\
					"push	r27						\n\t"	\
					"push	r26						\n\t"	\
					"push	r25						\n\t"	\
					"push	r24						\n\t"	\
					"push	r23						\n\t"	\
					"push	r22						\n\t"	\
					"push	r21						\n\t"	\
					"push	r20						\n\t"	\
					"push	r19						\n\t"	\
					"push	r18						\n\t"	\
					"push	r17						\n\t"	\
					"push	r16						\n\t"	\
					"push	r15						\n\t"	\
					"push	r14						\n\t"	\
					"push	r13						\n\t"	\
					"push	r12						\n\t"	\
					"push	r11						\n\t"	\
					"push	r10						\n\t"	\
					"push	r9						\n\t"	\
					"push	r8						\n\t"	\
					"push	r7						\n\t"	\
					"push	r6						\n\t"	\
					"push	r5						\n\t"	\
					"push	r4						\n\t"	\
					"push	r3						\n\t"	\
					"push	r2						\n\t"	\
					"push	r1						\n\t"	\
					"clr	r1						\n\t"	\
					"push	r0						\n\t"	\
				);

#define RESTORE_IRQ()								\
	asm volatile (	"pop	r0						\n\t"	\
					"pop	r1						\n\t"	\
					"pop	r2						\n\t"	\
					"pop	r3						\n\t"	\
					"pop	r4						\n\t"	\
					"pop	r5						\n\t"	\
					"pop	r6						\n\t"	\
					"pop	r7						\n\t"	\
					"pop	r8						\n\t"	\
					"pop	r9						\n\t"	\
					"pop	r10						\n\t"	\
					"pop	r11						\n\t"	\
					"pop	r12						\n\t"	\
					"pop	r13						\n\t"	\
					"pop	r14						\n\t"	\
					"pop	r15						\n\t"	\
					"pop	r16						\n\t"	\
					"pop	r17						\n\t"	\
					"pop	r18						\n\t"	\
					"pop	r19						\n\t"	\
					"pop	r20						\n\t"	\
					"pop	r21						\n\t"	\
					"pop	r22						\n\t"	\
					"pop	r23						\n\t"	\
					"pop	r24						\n\t"	\
					"pop	r25						\n\t"	\
					"pop	r26						\n\t"	\
					"pop	r27						\n\t"	\
					"pop	r28						\n\t"	\
					"pop	r29						\n\t"	\
					"pop	r30						\n\t"	\
					"pop	r31						\n\t"	\
					"out	__SREG__, r31			\n\t"	\
					"pop	r31						\n\t"	\
					"reti							\n\t"	\
				);
//-------------------------------------------------------------------
// macros used for saving and restoring context from a task level
//-------------------------------------------------------------------

#define SAVE_CONTEXT()									\
	asm volatile (	"push	r31						\n\t"	\
					"in		r31, __SREG__			\n\t"	\
					"cli							\n\t"	\
					"push	r31						\n\t"	\
					"push	r30						\n\t"	\
					"push	r29						\n\t"	\
					"push	r28						\n\t"	\
					"push	r27						\n\t"	\
					"push	r26						\n\t"	\
					"push	r25						\n\t"	\
					"push	r24						\n\t"	\
					"push	r23						\n\t"	\
					"push	r22						\n\t"	\
					"push	r21						\n\t"	\
					"push	r20						\n\t"	\
					"push	r19						\n\t"	\
					"push	r18						\n\t"	\
					"push	r17						\n\t"	\
					"push	r16						\n\t"	\
					"push	r15						\n\t"	\
					"push	r14						\n\t"	\
					"push	r13						\n\t"	\
					"push	r12						\n\t"	\
					"push	r11						\n\t"	\
					"push	r10						\n\t"	\
					"push	r9						\n\t"	\
					"push	r8						\n\t"	\
					"push	r7						\n\t"	\
					"push	r6						\n\t"	\
					"push	r5						\n\t"	\
					"push	r4						\n\t"	\
					"push	r3						\n\t"	\
					"push	r2						\n\t"	\
					"push	r1						\n\t"	\
					"clr	r1						\n\t"	\
					"push	r0						\n\t"	\
					"lds	r26, CurrentTask		\n\t"	\
					"lds	r27, CurrentTask + 1	\n\t"	\
					"in		r0, 0x3d				\n\t"	\
					"st		x+, r0					\n\t"	\
					"in		r0, 0x3e				\n\t"	\
					"st		x+, r0					\n\t"	\
				);

/* 
 * Opposite to SAVE_CONTEXT().  Interrupts will have been disabled during
 * the context save so we can write to the stack pointer. 
 */

#define RESTORE_CONTEXT()								\
	asm volatile (	"lds	r26, NextTask		\n\t"	\
					"lds	r27, NextTask + 1	\n\t"	\
					"ld		r28, x+					\n\t"	\
					"out	__SP_L__, r28			\n\t"	\
					"ld		r29, x+					\n\t"	\
					"out	__SP_H__, r29			\n\t"	\
					"pop	r0						\n\t"	\
					"pop	r1						\n\t"	\
					"pop	r2						\n\t"	\
					"pop	r3						\n\t"	\
					"pop	r4						\n\t"	\
					"pop	r5						\n\t"	\
					"pop	r6						\n\t"	\
					"pop	r7						\n\t"	\
					"pop	r8						\n\t"	\
					"pop	r9						\n\t"	\
					"pop	r10						\n\t"	\
					"pop	r11						\n\t"	\
					"pop	r12						\n\t"	\
					"pop	r13						\n\t"	\
					"pop	r14						\n\t"	\
					"pop	r15						\n\t"	\
					"pop	r16						\n\t"	\
					"pop	r17						\n\t"	\
					"pop	r18						\n\t"	\
					"pop	r19						\n\t"	\
					"pop	r20						\n\t"	\
					"pop	r21						\n\t"	\
					"pop	r22						\n\t"	\
					"pop	r23						\n\t"	\
					"pop	r24						\n\t"	\
					"pop	r25						\n\t"	\
					"pop	r26						\n\t"	\
					"pop	r27						\n\t"	\
					"pop	r28						\n\t"	\
					"pop	r29						\n\t"	\
					"pop	r30						\n\t"	\
					"pop	r31						\n\t"	\
					"out	__SREG__, r31			\n\t"	\
					"pop	r31						\n\t"	\
				);

/*
**
*/

#endif
