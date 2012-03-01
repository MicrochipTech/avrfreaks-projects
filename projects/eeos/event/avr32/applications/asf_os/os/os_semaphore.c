/*! \file
 * \brief eeOS Semaphores
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

enum os_event_status __os_event_sempahore_is_triggered(struct os_process *proc,
		os_ptr_t args)
{
	/* Initial status set to "no event" */
	enum os_event_status status = OS_EVENT_NONE;
	/* Cast the arguments into a os_semaphore structure */
	struct os_semaphore *sem = (struct os_semaphore *) args;
	/* Save the critical region status */
	bool is_critical = os_is_critical();

	/* Enter in a critical region if not already in */
	if (!is_critical) {
		os_enter_critical();
	}
	/* If there is only 1 semaphore left */
	if (sem->counter == 1) {
		/* Take it */
		sem->counter = 0;
		/* It is the last semaphore so no further check is necessary */
		status = OS_EVENT_OK_STOP;
	}
	/* If there is more than 1 semaphore available */
	else if (sem->counter > 1) {
		/* Take it */
		sem->counter--;
		/* Another process can be triggered with this sempahore, so
		 * set the status as "continue"
		 */
		status = OS_EVENT_OK_CONTINUE;
	}
	/* Leave the critical region */
	if (!is_critical) {
		os_leave_critical();
	}

	return status;
}

void os_semaphore_take(struct os_semaphore *sem)
{
	/* Save the critical region status */
	bool is_critical = os_is_critical();

	/* Enter in a critical region if not already in */
	if (!is_critical) {
		os_enter_critical();
	}
	/* If all the semaphores are not taken, take one */
	if (sem->counter > 0) {
		/* Decrease the semaphore counter */
		sem->counter--;
	}
	/* If the all the semaphores are taken, suspend this task */
	else {
		/* Queue element used to hold the process in the waiting list */
		struct os_queue_process queue_elt;
		/* Disable this process */
		__os_process_disable_naked(__os_process_get_current());
		/* Assign the data associated to this queue entry */
		queue_elt.proc = __os_process_get_current();
		/* Add this process to the event list of the sempahore */
		os_queue_process_add(&sem->queue, &queue_elt);
		/* Manually switch the process context */
		os_switch_context(false);
	}
	/* Leave the critical region unless the CPU was previously in */
	if (!is_critical) {
		os_leave_critical();
	}
}

void os_semaphore_release(struct os_semaphore *sem)
{
	/* Save the critical region status */
	bool is_critical = os_is_critical();

	/* Enter in a critical region if not already in */
	if (!is_critical) {
		os_enter_critical();
	}
	/* Check if there is another process in the waiting list */
	if (sem->queue) {
		struct os_process *proc;
		/* Pop the next process in the waiting list */
		proc = os_queue_process_pop(&sem->queue)->proc;
		/* Enable this process */
		__os_process_enable_naked(proc);
	}
	/* Else check if the sempahore counter is not above the limit */
	else if (sem->counter < sem->max) {
		/* Increase the semaphore counter, in other word, release the
		 * semaphore previously taken.
		 */
		sem->counter++;
	}
	/* Leave the critical region unless the CPU was previously in */
	if (!is_critical) {
		os_leave_critical();
	}
}

