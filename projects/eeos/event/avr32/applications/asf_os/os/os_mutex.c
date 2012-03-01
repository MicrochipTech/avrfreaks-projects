/*! \file
 * \brief eeOS Mutex
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

#include "os_core.h"

enum os_event_status __os_event_mutex_is_triggered(struct os_process *proc,
		os_ptr_t args)
{
	/* Output status of the event */
	enum os_event_status status = OS_EVENT_NONE;
	/* Cast the arguments to a \ref os_mutex structure */
	struct os_mutex *mutex = (struct os_mutex *) args;
	/* Save the critical region status */
	bool is_critical = os_is_critical();

	/* Enter in a critical region if not already in */
	if (!is_critical) {
		os_enter_critical();
	}
	/* Test if the mutex is not locked */
	if (!mutex->is_locked) {
		/* lock it */
		mutex->is_locked = true;
		/* Associate a process with this mutex */
		mutex->process = proc;
		/* Set the status of this event to triggered */
		status = OS_EVENT_OK_STOP;
	}
	/* Leave the critical section */
	if (!is_critical) {
		os_leave_critical();
	}

	/* Return the event status */
	return status;
}

void os_mutex_lock(struct os_mutex *mutex)
{
	/* Save the critical region status */
	bool is_critical = os_is_critical();

	/* Enter in a critical region if not already in */
	if (!is_critical) {
		os_enter_critical();
	}
	/* If the mutex is not locked, lock it */
	if (!mutex->is_locked) {
		mutex->is_locked = true;
		mutex->process = __os_process_get_current();
	}
	/* If the mutex is already locked, suspend this task */
	else {
		/* Create a queue element, it will be stored on the stack. */
		struct os_queue_process queue_elt;
		/* Disable this process */
		__os_process_disable_naked(__os_process_get_current());
		/* Set the data associated to this queue entry */
		queue_elt.proc = __os_process_get_current();
		/* Add this process to the event list of the mutex */
		os_queue_process_add(&mutex->queue, &queue_elt);
		/* Manually switch the process context */
		os_switch_context(false);
	}
	/* Leave the critical region unless the CPU was previously in */
	if (!is_critical) {
		os_leave_critical();
	}
}

void os_mutex_unlock(struct os_mutex *mutex)
{
	/* Only the process which locked the mutex can unlock it */
	if (__os_process_get_current() == mutex->process) {
		/* Save the critical region status */
		bool is_critical = os_is_critical();
		/* Enter in a critical region if not already in */
		if (!is_critical) {
			os_enter_critical();
		}
		/* Check if there is another process in the waiting list */
		if (mutex->queue) {
			struct os_process *proc;
			/* Pop the next process in the waiting list */
			proc = os_queue_process_pop(&mutex->queue)->proc;
			/* Lock the mutex for this process */
			mutex->process = proc;
			/* Enable this process */
			__os_process_enable_naked(proc);
		}
		/* Else unlock the mutex */
		else {
			mutex->is_locked = false;
		}
		/* Leave the critical region unless the CPU was previously in */
		if (!is_critical) {
			os_leave_critical();
		}
	}
}
