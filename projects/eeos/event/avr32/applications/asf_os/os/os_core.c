/*! \file
 * \brief eeOS Core
 * \author Blaise Lengrand (blaise.lengrand@gmail.com)
 * \version 0.1
 * \date 2011
 *
 * \section eeos_license License
 * \ref group_os is provided in source form for FREE evaluation, for
 * educational use or for peaceful research. If you plan on using \ref group_os
 * in a commercial product you need to contact the author to properly license
 * its use in your product. The fact that the  source is provided does
 * NOT mean that you can use it without paying a licensing fee.
 */

#include "compiler.h"
#include "os_core.h"

/*! Internal hooks
 * \{
 */
#ifndef OS_SCHEDULER_PRE_INTERRUPT_HOOK
	#define OS_SCHEDULER_PRE_INTERRUPT_HOOK()
#endif
/*!
 * \}
 */

/*! \brief Context of the application task. This contex is initially filled
 * as an active task with the highest priority. This task is shared with the
 * event task, therefore changing the priority of the event task will change
 * the priority of this task as well.
 * \note This context is available only after the call of \ref os_start
 */
struct os_process os_app = {
	.next = &os_app,
	.status = OS_PROCESS_ACTIVE,
	.type = OS_PROCESS_TYPE_APPLICATION,
#if CONFIG_OS_USE_PRIORITY == true
	.priority = OS_PRIORITY_1,
	.priority_counter = OS_PRIORITY_1,
#endif
#if CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH == true
	.cycle_counter = 0,
#endif
};

/*! \brief Pointer to keep track of the current process running. This is also
 * the entry point of the active process list.
 */
struct os_process *__os_current_process = &os_app;

#if CONFIG_OS_USE_TICK_COUNTER == true
/*! \brief This variable will count the number of ticks from the begining of the
 * application.
 */
volatile os_tick_t os_tick_counter = 0;
#endif

#if CONFIG_OS_USE_PRIORITY == true
struct os_process *__os_scheduler(void)
{
	do {
		/* Get the next process in the active process list */
		__os_current_process = __os_current_process->next;
		/* Check its priority counter and defines if it will run or not
		 */
		if (__os_current_process->priority_counter == 0) {
			/* Reload the priority counter */
			__os_current_process->priority_counter =
					__os_current_process->priority;
			/* Interrupt hook, will be used only if interrupt are
			 * enabled.
			 */
			OS_SCHEDULER_PRE_INTERRUPT_HOOK();
			/* Log the context switch event. Active only if the
			 * trace functionality is enabled.
			 */
			__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_CONTEXT_SWITCH,
					__os_current_process);
			/* Return the new process to be executed. The context
			 * will be switch to enable this process.
			 */
			return __os_current_process;
		}
		/* If the scheduler did not decide to run this process due to
		 * its priority level, then decrease its priority counter.
		 */
		__os_current_process->priority_counter--;
	/* This is a infinite loop why exists only if a process has the right
	 * priority to be execute. It also means that at least 1 process should
	 * be on the active process list all the time.
	 */
	} while (true);
}
#else
struct os_process *__os_scheduler(void)
{
	/* This scheduler is very basic and will take the next process in the
	 * chain list to be executed.
	 */
	__os_current_process = __os_current_process->next;
	/* Interrupt hook, will be used only if interrupt are
	 * enabled.
	 */
	OS_SCHEDULER_PRE_INTERRUPT_HOOK();
	/* Log the context switch event. Active only if the
	 * trace functionality is enabled.
	 */
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_CONTEXT_SWITCH,
			__os_current_process);
	/* Return the new process to be executed. The context
	 * will be switch to enable this process.
	 */
	return __os_current_process;
}
#endif

void __os_process_enable_naked(struct os_process *proc)
{
	struct os_process *last_proc = proc;

	/* Get the next process after the current one. This because the current
	 * process can never be removed from the active process list because it
	 * needs to store its context on its stack before beiing disbaled.
	 */
	last_proc = __os_current_process->next;
#if CONFIG_OS_PROCESS_ENABLE_FIFO == true
	/* Look for the last process in the circular chain list of active
	 * processes.
	 */
	while (last_proc->next != __os_current_process->next) {
		/* If this process is _not_ equal to the first process we
		 * checked, it means we did not do a loop yet. So continue.
		 */
		last_proc = last_proc->next;
	}
#endif
	/* If the application process is running, remove it from the active
	 * process list. The application process will run only if no process
	 * are running.
	 */
	if (__os_process_is_application(last_proc)) {
		/* This process (proc) is then the only process that will be
		 * running since we want to remove the application process. So
		 * loop this process with himself (circular chain list).
		 */
		proc->next = proc;
		/* Change the status of the application process to idle. */
		os_app.status = OS_PROCESS_IDLE;
	}
	else {
		/* The application process is _not_ running, this is a normal
		 * case. Insert the process (proc) inside the circular chain
		 * list.
		 */
		proc->next = last_proc->next;
	}
	/* Finalize the insertion of this process to the circular chain list. */
	last_proc->next = proc;
	/* Change the status of the process (proc) to active */
	proc->status = OS_PROCESS_ACTIVE;
}

void __os_process_enable(struct os_process *proc)
{
	/* Check if the current process is running inside a critical region. */
	bool is_critical = os_is_critical();
	/* The following code is critical, so enter in the critial region if
	 * not already done.
	 */
	if (!is_critical) {
		os_enter_critical();
	}
	/* Make sure the task is not already enabled */
	if (!__os_process_is_enabled(proc)) {
		/* If not, enable this process */
		__os_process_enable_naked(proc);
	}
	/* Leave the critical region if the process was not previously in. */
	if (!is_critical) {
		os_leave_critical();
	}
}

void __os_process_disable_naked(struct os_process *proc)
{
	struct os_process *last_proc = proc;

	/* Look for the process before proc. A research is necessary at this
	 * point since we are using a uni-directional circular chain list to
	 * keep track of the active processes.
	 */
	while (last_proc->next != proc) {
		/* If the process after last_proc is not equal to the proc, it
		 * means we did not do a loop yet. So continue.
		 */
		last_proc = last_proc->next;
	}
	/* If last_proc == proc, it means that there is only 1 process in the
	 * active process list. In that case, we need to replace the current
	 * process with the application process.
	 */
	if (last_proc == proc) {
		/* Loop the application process over itslef. */
		os_app.next = &os_app;
		/* Set this process as the application process. This is
		 * necessary since this process is shared with the event
		 * process.
		 */
		os_app.type = OS_PROCESS_TYPE_APPLICATION;
		/* Change the application process to active */
		os_app.status = OS_PROCESS_ACTIVE;
		/* Insert the application process in the active process list
		 * after the current process.
		 */
		proc->next = &os_app;
	}
	else {
		/* There is at least 2 processes in the active process list.
		 * Remove the process (proc).
		 */
		last_proc->next = proc->next;
	}
	/* Change the status of this process (proc) to idle. */
	proc->status = OS_PROCESS_IDLE;
}

void __os_process_disable(struct os_process *proc)
{
	/* Check if the current process is running inside a critical region. */
	bool is_critical = os_is_critical();
	/* The following code is critical, so enter in the critial region if
	 * not already done.
	 */
	if (!is_critical) {
		os_enter_critical();
	}
	/* Make sure the task is not already disabled */
	if (__os_process_is_enabled(proc)) {
		/* If not, disable this process */
		__os_process_disable_naked(proc);
	}
	/* Manually switch context to stop the execution of this process if it
	 * is the current process.
	 */
	if (proc == __os_current_process) {
		os_switch_context(false);
	}
	/* Leave the critical region if the process was not previously in. */
	if (!is_critical) {
		os_leave_critical();
	}
}

void os_yield(void)
{
	/* Check if the current process is running inside a critical region. */
	bool is_critical = os_is_critical();
	/* The following code is critical, so enter in the critial region if
	 * not already done.
	 */
	if (!is_critical) {
		os_enter_critical();
	}
	/* Log the yield event to be visible in the trace. This is done only if
	 * the trace is activated.
	 */
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_YIELD,
			__os_current_process);
	/* Manually switch context to execute another process. */
	os_switch_context(false);
	/* Leave the critical region if the process was not previously in. */
	if (!is_critical) {
		os_leave_critical();
	}
}
