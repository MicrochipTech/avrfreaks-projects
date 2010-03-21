//---------------------------------------------------------------------------
// FunkOS(++) Copyright 2010, Funkenstein Software Consulting
//---------------------------------------------------------------------------
//
// FunkOS R3 preview.
//
// C++ kernel demo app: 	
//
// This app uses the new FunkOS++ kernel to implement a simple example 
// application, demonstrating the use of a number of the C++ kernel's 
// features, including: Tasks, semaphores, device drivers (+mutexes), 
// and timers.
//
// The application task is set up to wait for a timer-triggered semaphore
// to be posted, after which, an NES joypad is read.  The data from the
// joypad is then copied into a local structure, after which the thread
// pends on the semaphore again.
//
// While the application thread is idle, the idle thread runs, which is
// used to simply increment a counter at this point.
// 
// Platform: atmega328p, 16MHz
// Compiler: WinAVR
//
//-----------------------------------------------------------------------

#include "types.h"
#include "kernelcfg.h"
#include "task.h"
#include "scheduler.h"
#include "timer.h"
#include "mutex.h"
#include "semaphore.h"
#include "driver.h"
#include "drvNESjoy_in.h"

//-----------------------------------------------------------------------
// This is the semaphore that is used to trigger the application thread
//-----------------------------------------------------------------------
static FunkOS_Semaphore g_Task1Sem;

//-----------------------------------------------------------------------
// Task class declaration - any thread must inherit FunkOS_Task publicly
// in order for the kernel to work properly (we rely on the memory alignment
// of the base class)
//-----------------------------------------------------------------------
class Task1 : public FunkOS_Task
{
public:
	Task1();
private:
	WORD m_awStack[128];			// Declare the thread stack
	static void RunMe(void*);		// Declare the thread's entry function
	NESi m_clNES;					// Any other data used by the task...

};

//-----------------------------------------------------------------------
// The constructor of any task must do the following:
// 1) assign the entry function
// 2) set the stack size
// 3) assign the task stack
//-----------------------------------------------------------------------
Task1::Task1()
{
	m_pfFunc = (&Task1::RunMe);
	m_usStackSize = 128;
	m_pwStack = m_awStack;
}

//-----------------------------------------------------------------------
// Task1::RunMe is a static function which is set by our constructor as 
// the thread's entry function.  Because it's a static function (and needs
// to be, otherwise we can't get the address of the function), the kernel
// passes in a pointer to the task.  Static functions don't get a "this" 
// pointer, so passing in the function pointer is a clever hack to work 
// around the limitation.
//-----------------------------------------------------------------------
void Task1::RunMe(void *pvThis_)
{
	volatile int i = 0;
	Task1 *pstThis = (Task1*)pvThis_;
	JOYSTICK_REPORT_STRUCT stReport;
	
	pstThis->m_clNES.Start();
	
	while(1)
	{
		g_Task1Sem.Pend(TIME_FOREVER);
		pstThis->m_clNES.Control(JOY_IN_READ, NULL);
		pstThis->m_clNES.Control(JOY_IN_COPY, &stReport);
	}
}

//-----------------------------------------------------------------------
// Idle thread class - must inherit FunkOS_Task as public
//-----------------------------------------------------------------------
class IdleTask: public FunkOS_Task
{
public:
	IdleTask();
private:
	WORD m_awStack[128];
	static void RunMe(void*);
};
//-----------------------------------------------------------------------
IdleTask::IdleTask()
{
	m_pfFunc = (&IdleTask::RunMe);
	m_usStackSize = 128;
	m_pwStack = m_awStack;
}
//-----------------------------------------------------------------------
void IdleTask::RunMe(void *pvThis_)
{
	volatile int i = 0;

	while(1)
	{
		i++;
	}
}

//-----------------------------------------------------------------------
// Timer callback (lightweight thread) used to post the semaphore that 
// triggers the application thread
//-----------------------------------------------------------------------
static void Timer1(void)
{
	g_Task1Sem.Post();
}


//-----------------------------------------------------------------------
// Entry point for the application - never returns.
//-----------------------------------------------------------------------
int main(void)
{
	Task1 clTask1;							// Application task instance
	IdleTask clIdle;						// Idle task instance
	FunkOS_Timer *pclTimer;					// timer pointer used in the application

	FunkOS_Scheduler::Init();				// initialize the scheduler (must be first)
	FunkOS_Scheduler::Add(&clTask1);		// Add the first task to the scheduler
	FunkOS_Scheduler::Add(&clIdle);			// Add the idel task to the scheduler

	clTask1.CreateTask("MyTask", 1);		// initialize the main task to priority 1
	clIdle.CreateTask("IdleTask", 0);		// initialize the idle task to priority 0

	clTask1.Start();						// Start the two tasks
	clIdle.Start();

	pclTimer = FunkOS_TimerList::Add(3, 0, Timer1);	// Add a 3ms timer to trigger the application thread
	pclTimer->Start();						// Start the timer

	FunkOS_Scheduler::StartTasks();			// Start the scheduler (never returns)

	return 0;								// Main never returns
}
