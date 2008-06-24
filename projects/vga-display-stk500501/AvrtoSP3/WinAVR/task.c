/////////////////////////////////////////////////////////////////////////////////////////////
// This file is compiled with WinAVR version 4.1.2
//
// These routines fuck with the stack
//
// you must use either of these optimizations:
// -O1
// -O3
// -O2
//
// Whatever you do, do NOT use -O0....it will NOT work
//
// If in the future, a newer version (or older version for that matter) does not
// work properly, the code might have to be changed.  In particular, void IrqSwap(void)
//
// The following line in IrqSwap may need to be changed, in particular:
//		"adiw r24,6				\n\t"			//add 6 (remove irrelavent calls)
//
// This line adds 6 to the stack pointer, and depending on how much the compiler 
// pushes on stacks, this value could change.  For instance, to make the -O0 option
// work correctly, you need to change this value to 8
// /////////////////////////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "task.h"
#include "pq.h"
#include "blockalloc.h"

extern volatile int Tick;
volatile int TStamp;
volatile int InterruptCount;
volatile int Blocking;
TCB *CurrentTask,*NextTask;
int OS_IntNesting;
PQ ActiveTasks;
ECB *TimeoutList;
static ECB *ECBPool;
int OsRunning;


//volatile int Seconds = 999;
void DoSwap(void) __attribute__ ( ( naked ) );
void IrqSwap(void) __attribute__ ( ( naked ) );
void StartOS(void) __attribute__ ( ( naked ) );


/***********************************************************************
** TimerTicker                                                        **
**                                                                    **
**	This function is called by the real time interrupt handler        **
**	This function Increments the clock, scans through the Event       **
**  Control Blocks looking for timed out events.                      **
**                                                                    **
***********************************************************************/


void TimerTicker(void)
{
	//we need to process the list of tasks waiting
	//for queues
	ECB *e;
	TCB *t,**prev;


	++TStamp;
	for(e = TimeoutList;e;e=e->Tnext)	//go through whole list
	{
		for(prev = &(e->task_h),t = e->task_h;t;)
		{
			if(t->timeout > 0)	//if less than zero, infinate timeout
			{
				if( --(t->timeout) <= 0)	//decrement timeout counter
				{
					e->EventCount++;	//pretend to post
					t->status = EVENT_TIMEOUT;	//event caused by semaphore
					*prev = t->next;
					t->next = 0;
					Insert(&ActiveTasks,t);
				}
			}
			prev = &(t->next);
			t = t->next;
		}
	}
}

//----------------------------------------------------------------------
//
// This function is used to compare two items in the priority queue
// to see which one has the highest priority
//
// The function is passes two pointers that point to the pointers
// that point to the actual objects
//
//----------------------------------------------------------------------
//
static int PriorityCompare(void **s1, void **s2)
{
	int r;

	//check priority levels
	if( (r = ((TCB *)*s1)->priority - ((TCB *)*s2)->priority) != 0)
		return r;
	//if priority levels are the same, then check time stamps
	return (int)(((TCB *)*s2)->TimeStamp - ((TCB *)*s1)->TimeStamp);
}
/*
static int TaskCompare(void **s1,void **s2)
{
	int retval = 1;

	if(*s1 == *s2) retval = 0;
	return retval;
}
*/
void FillWithDeadBeef(long *s,int l)
{
	int i;

	for(i=0;i<l;++i)
		s[i] = 0xaddeefbe;
}

void CreateTask(TCB *t, void (*task)(void *),char *stack, int stacksize,int priority,char *name,void *arg)
{
	/***************************************************************************************
	** Create a task
	** parameter:
	**	t...............pointer to the task control block
	**	task............pointer to function that will be the task
	**	stack...........pointer to memory block to use as stack
	**	stacksize.......size of stack in number of longs
	**	priority........task priority
	**	name............tag to identify task control block for debug purposes
	**	arg.............argument to be passed to function...it chan either be 
	**  a value, or a pointer to some data
	***************************************************************************************/
	char *stk;
	char sr;

	FillWithDeadBeef((long *)stack,stacksize >> 2);
	sr = Disable();
	if(t)
	{
		t->TcbSwaps = 0;
		t->stacksize = stacksize;
		t->priority = priority;
		t->TimeStamp = 0;
		t->name = name;
		t->stacktop = stack;
		t->next = (TCB *)0;
		t->list = (TCB *)0;
		stk = stack + stacksize-1;
		*stk-- = (char)LO( (unsigned)task);/*	return address	*/
		*stk-- = (char)HI( (unsigned)task);
		*stk-- = 0;		//top byte of address is 0...bummer
		*stk-- = 31;		/*	r31 = 0	*/
		*stk-- = 0x80;	/*	SREG Interrupts enabled	*/
		*stk-- = 30;		/*	r30 = 0	*/
		*stk-- = 29;		/*	r29 = 0	*/
		*stk-- = 28;		/*	r28 = 0	*/
		*stk-- = 27;		/*	r27 = 0	*/
		*stk-- = 26;		/*	r26 = 0	*/
		*stk-- = (char)HI( (unsigned)arg);		/*	r25 = 0	*/
		*stk-- = (char)LO( (unsigned)arg);		/*	r24 = 0	*/
		*stk-- = 23;		/*	r23 = 0	*/
		*stk-- = 22;		/*	r22 = 0	*/
		*stk-- = 21;		/*	r21 = 0	*/
		*stk-- = 20;		/*	r20 = 0	*/
		*stk-- = 19;		/*	r19 	*/
		*stk-- = 18;		/*	r18 	*/
		*stk-- = 17;		/*	r17 	*/
		*stk-- = 16;		/*	r16 	*/
		*stk-- = 15;		/*	r15 	*/
		*stk-- = 14;		/*	r14 	*/
		*stk-- = 13;		/*	r13 	*/
		*stk-- = 12;		/*	r12 	*/
		*stk-- = 11;		/*	r11 	*/
		*stk-- = 10;		/*	r10 	*/
		*stk-- = 9;			/*	r9 		*/
		*stk-- = 8;			/*	r8 		*/
		*stk-- = 7;			/*	r7 		*/
		*stk-- = 6;			/*	r6 		*/
		*stk-- = 5;			/*	r5 		*/
		*stk-- = 4;			/*	r4 		*/
		*stk-- = 3;			/*	r3 		*/
		*stk-- = 2;			/*	r2 		*/
		*stk-- = 0;			/*	r1 must be ZERO 	*/
		*stk-- = 0;			/*	r0 		*/
		t->stack = stk;
	}
	Enable(sr);
}

/************************************************************************
**                                                                     **
** ExitInterrupt                                                       **
**                                                                     **
** This function keeps track of the nesting level of interrupts        **
** If nesting level is == 0, then do a context swap to the highest     **
** priority task                                                       **
**                                                                     **
** WARNING!                                                            **
** 	Don't mess with this function, you will be sorry                   **
**  Adding any local variables or passed parameters                    **
**  Messes with the stack.  IrqSwap alters the stack and               **
**  if it is different from what it expects, it will mess              **
**  up the stack.                                                      **
**                                                                     **
** Hey, you don't even need to add any local variables, just add a     **
** Little bit more code and it will mess things up.  Always check      **
** the listing file to see how much might get pushed onto the stack	   **
**                                                                     **
**                                                                     **
** ExitInterrupt will alocate at least 4 bytes on the stack probably   **
** for sr, depending on how it does this.                              **
** Currently ExitInterrupt uses 4 bytes on the stack to save various   **
** registers that it doesn't really have to                            **
**                                                                     **
** Currently IrqSwap() needs to remove 12 bytes from stack			   **
**				                                                       **
************************************************************************/

void ExitInterrupt(void)
{
	--InterruptCount;
	if(!InterruptCount && !Blocking)	//don't swap if nested
	{
		CurrentTask->TimeStamp = TStamp;
		Replace(&ActiveTasks,(void **)&NextTask,CurrentTask);
		if(NextTask != CurrentTask)
		{
			NextTask->TcbSwaps++;
			IrqSwap();	//interrupts will be re-enabled right after this
		}
	}
}

/*********************************************************************
** This function is used to yield (please note that the function name
** is misspelled on purpose, there is a Yield funciton somewhere in the
** IAR library) control of the processor to another task, but only if
** we are at the task level.  We cannot yield if we are in an interrupt
** routine
*********************************************************************/

void Yeild(void)
{
	char sr;

	if(!InterruptCount)
	{
		sr = Disable();
		CurrentTask->TimeStamp = TStamp;
		Replace(&ActiveTasks,(void **)&NextTask,(void *)CurrentTask);
		NextTask->TcbSwaps++;
		DoSwap();
		Enable(sr);
	}
}

static ECB *AllocECB(void)
{
	//--------------------------------------------------------------------------
	// Allocate an event control Block
	//	Check the ECB pool, if it is empty, then allocate a new ECB from heap
	//
	// return value:
	//	pointer to ECB on success
	//	NULL on fail
	//--------------------------------------------------------------------------
	ECB *rv;
	
	char sr = Disable();
	if (ECBPool)	//check the pool
	{
		rv = ECBPool;
		ECBPool = ECBPool->Tnext;
		Enable(sr);
	}
	else
	{
		Enable(sr);
		rv = (ECB *)malloc(sizeof(ECB));	//or allocate from the heap
	}
	return rv;
}

static void FreeECB(ECB *e)
{
	//--------------------------------------------------------------------------
	// put ECB block into ECB pool (don't bother to return it to the heap
	//
	// parameter:
	//	e...........pointer to ECB block to free
	//--------------------------------------------------------------------------
	char sr;

	sr = Disable();
	if(e->SemaphoreMode)	//is it in timeout list?
	{
		if(e == TimeoutList)
			TimeoutList = e->Tnext;
		if(e->Tprev)
			e->Tprev->Tnext = e->Tnext;
		if(e->Tnext)
			e->Tnext->Tprev = e->Tprev;
	}
	e->Tnext = ECBPool;
	ECBPool = e;
	Enable(sr);
}

ECB *NewSemaphore(int InitCount,int Mode,char *name)
{
	ECB *r;

	if((r = AllocECB()) != NULL)
		CreateSemaphore(r,InitCount,Mode,name);
	return r;
}

/***************************************************************************
**                                                                        **
** Semaphore objects
****************************************************************************
** CreateSemaphore                                                        **
** parameters:                                                            **
**	e............pointer to an event control block to initialize          **
**	InitCount....Initial value of semaphore count                         **
**	Mode.........0 = Semaphore doesn't time out, 1 = Semaphore Times out  **
**	n............name of semaphore (DEBUG only)                           **
**                                                                        **
***************************************************************************/

void CreateSemaphore(ECB *e,int InitCount,int Mode,char *n)
{
	char sr;

	e->EventCount = InitCount;
	e->SemaphoreMode = Mode;
	strcpy(e->name,n);					//copy name in
	e->task_h = 0;						//no tasks waiting
	e->task_t = 0;						//no tasks waiting
	//-------------------------------------------------------
	// if Mode==1, then put this semaphore into timeout list
	//-------------------------------------------------------
	sr = Disable();
	if(e->SemaphoreMode == 1)
	{
		if(TimeoutList)	//is list already started?
		{
			e->Tnext = TimeoutList;	//make current head next in line
			e->Tprev = 0;			//these is no previous entry
			TimeoutList->Tprev = e;	//this will be prev for next entry
			TimeoutList = e;			//this is now the new head of list
		}
		else			//no start list
		{
			TimeoutList = e;
			e->Tnext = 0;
			e->Tprev = 0;
		}
	}
	Enable(sr);	//restore interrupts
}

/*******************************************************************************
** Delete Semaphore
**	This function is used to free a Semaphore.  Rather than return the memory
** to the heap, it is put into a pool where it can be reused later without
** having to do another malloc.
**
**	parameters:
**		e..........pointer to ECB block that describes the semaphore
**
*******************************************************************************/

void DeleteSemaphore(ECB *e)
{
	char sr;
	TCB *t;

	//are there any tasks waiting for this sucker?
	sr = Disable();
	while(e->task_h)		//remove all tasks
	{
		t = e->task_h;		//get first task in
		e->task_h = t->next;       //remove from list
		t->next = (TCB *)0;
		t->status = EVENT_DELETED;			//return this value
		Insert(&ActiveTasks,t);	//put task back onto active list
	}
	FreeECB(e);		//put semaphore back in pool
	Enable(sr);
}

/**************************************************************************
** PendSemaphore (Lock)
**		This function checks the value of the semaphore.
**		1. Check Value EventCount
**		2. Decrement the value of the EventCount
**		3. If Value in Step 1 was greater than 0, then return
**		4. If Value in Step 1 was less than or equal to 0 THEN
**		5. Add task to list of pending tasks
**      6. Do a Context Switch
**
** parameters:
**	e.............pointer to event control block for semaphore
**	Timeout.......Number of system timer ticks to wait for semaphore to post
**
** return value:
**	Inidicates status of operation.  0 or positive is normal,
**		negative is error
**************************************************************************/

int PendSemaphore(ECB *e, int Timeout)	//wait for semaphore availiable
{
	char sr;
	int retval;
	sr = Disable();
	if(e->EventCount > 0)	/*	is resource availiable?	*/
	{
		/*	yes	*/
		--e->EventCount;
		Enable(sr);
		retval = EVENT_NOERROR;	//everything is peachy
	}
	else
	{
		/*	no	*/
		--e->EventCount;
		if(!e->task_h)		//make linked list of waiting tasks
			e->task_h = CurrentTask;
		else
			e->task_t->next = CurrentTask;
		e->task_t = CurrentTask;
		CurrentTask->status = EVENT_NOERROR;
		if(e->SemaphoreMode)
			CurrentTask->timeout = Timeout;

		//swap out current task

		CurrentTask->TimeStamp = TStamp;
		if(Delete(&ActiveTasks,(void **)&NextTask))	//get highest priority task
		{
			NextTask->TcbSwaps++;
			DoSwap();                    //do context swap
		}
		else
		{
			CurrentTask->status = EVENT_NOTASKS;
		}
		//-----------------------
		// Pend is over, check status
		//-----------------------

		Enable(sr);
		retval = CurrentTask->status;	//return this value
	}
	return retval;
}

/***************************************************************************
** PostSemaphore (Unlock)
**
**		This function is used to incrment the value of a sempaphore
**		1. Check value of event flag
**		2. Increment event flag
**		3. If value is greater than or equal to 0, return
**		4. Else remove task in pending list and put it into the active task queue
**		5. return
**
** parameters:
**	e.................pointer to event control block for semaphore
**	Value.............Value to return to pending task (should be positive)
**
** return value: 0 = No Error, negative indicates error
***************************************************************************/

int PostSemaphore(ECB *e, int Value)	//signal semaphore availiable
{
	TCB *t;
	char sr;

	sr = Disable();
	if(e->EventCount < 32766)	//is it going to overflow??
	{
		if(e->EventCount >= 0)	//increment semaphore
		{
			++e->EventCount;
			Enable(sr);
		}
		else
		{
			++e->EventCount;
			if(e->task_h)				//is there really a task waiting?
			{
				t = e->task_h;		//get first task in
				e->task_h = t->next;       //remove from list
				t->next = (TCB *)0;
				t->status = Value;			//return this value
				Insert(&ActiveTasks,t);	//put task back onto active list
				Enable(sr);
				Yeild();
			}
			else
			{
				Enable(sr);
			}
		}
		return EVENT_NOERROR;
	}
	else
	{
		Enable(sr);
		return(EVENT_OVERFLOW);
	}
}

int PostSemaphoreWithData(ECB *e, int Value,unsigned data)	//signal semaphore availiable
{
	TCB *t;
	int sr;

	sr = Disable();
	if(e->EventCount < 32766)	//is it going to overflow??
	{
		if(e->EventCount++ >= 0)	//increment semaphore
		{
			Enable(sr);
		}
		else
		{
			if(e->task_h)				//is there really a task waiting?
			{
				t = e->task_h;		//get first task in
				e->task_h = t->next;       //remove from list
				t->next = (TCB *)0;
				t->status = Value;			//return this value
				t->misc = data;				//and add this value in as well
				Insert(&ActiveTasks,t);	//put task back onto active list
				Enable(sr);
				Yeild();
			}
			else
			{
				Enable(sr);
			}
		}
		return EVENT_NOERROR;
	}
	else
	{
		Enable(sr);
		return(EVENT_OVERFLOW);
	}
}

int TimeDelay(int mSec)
{
	//--------------------------------------------------------------------------
	// Delay task for a time
	//	parameters:
	//	mSec.............number of milliseconds to delay for
	//
	// return value:
	//	returns non zero value if something happened out of the ordinary
	//--------------------------------------------------------------------------
	int retval;
	static int DCount = 0;	//keeps track of the number of times its called
	ECB *e;					//pointer to event control block for semaphore
	char sr;					//save area for status register

	sr = Disable();			//disable interrupts
	char *s = NewBlock();	//get a new block of memory
	Enable(sr);				//bring interrupts back up
	s[0] = 'D';				//create name for semaphore
	itoa(DCount++,&s[1],10);
	e = NewSemaphore(0,SEMAPHORE_MODE_TIMEOUT,s);	//create semaphore
	sr = Disable();
	DeleteBlock(s);			//return memory block
	Enable(sr);
	retval = PendSemaphore(e,mSec);	//pend for time delay period
	DeleteSemaphore(e);				//return semaphore back to pool
	return retval;					//exit
}

//--------------------------------------------------------------------------
// Initialize system
//--------------------------------------------------------------------------

void InitTask(void)
{
	InitPQ(&ActiveTasks,32,PriorityCompare);
}

//--------------------------------------------------------------------------
// Context switching functions
//--------------------------------------------------------------------------

void DoSwap(void)
{
	SAVE_CONTEXT();		//save the context of the current task
	CurrentTask = NextTask;	//set current task to next task
	RESTORE_CONTEXT();		//restore context of new task
	asm volatile ("ret");	//jump to new task via return
}

void IrqSwap(void)
{
	//-----------------------------
	// next, we need to adjust
	// the size of the stack
	// ignore return addresses on
	// stack to IrqSwap and 
	// Exit Interrupt
	//-----------------------------
	asm volatile (
		"in r25,__SP_H__		\n\t"			//get the stack pointer
		"in r24,__SP_L__		\n\t"
		"adiw r24,6				\n\t"			//add 6 (remove irrelavent calls)
		"lds	r26, CurrentTask		\n\t"	//get address of current task
		"lds	r27, CurrentTask + 1	\n\t"
		"st		x+, r24					\n\t"	//save stack pointer in TCB
		"st		x+, r25					\n\t"
	);
	CurrentTask = NextTask;
	RESTORE_CONTEXT();				//restore next task context
	asm volatile("ret");
}

//----------------------------------------------------------------------------
// Start the operating system
//----------------------------------------------------------------------------

void StartOS(void)
{
	//---------------------------------------
	// This is where we get the ball rolling
	// Remove task from priorty queue
	// Restore its context
	// and then jump to the first task using
	// return from subroutine
	//---------------------------------------
	Delete(&ActiveTasks,(void **)&NextTask);
	CurrentTask = NextTask;
	RESTORE_CONTEXT();
	asm volatile ("ret");
}

