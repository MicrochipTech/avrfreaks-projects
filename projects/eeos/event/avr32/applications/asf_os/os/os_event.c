/*! \file
 * \brief eeOS Events
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
#include <stdarg.h>

#if CONFIG_OS_USE_EVENTS == true

/*! \brief Active event list. This pointer points on the 1rst active event
 * registered in the evetn chain list.
 */
static struct os_event *__os_current_event = NULL;

void __os_event_create(struct os_event *event,
		const struct os_event_descriptor *descriptor, os_ptr_t args)
{
	/* Fill the event structure */
	event->desc.sort = descriptor->sort;
	event->desc.start = descriptor->start;
	event->desc.is_triggered = descriptor->is_triggered;
	/* Associate arguments to the event */
	event->args = args;
	/* Mark this event as disabled */
	event->queue.next = NULL;
	event->queue.prev = NULL;
}

/*! \brief Remove an event from the active event list.
 * \param event The event to be removed from the list
 * \warning The event must be in the event list
 */
static inline void __os_event_pop(struct os_event *event) {
	struct os_event *prev_event = __os_current_event;
	/* If the event is the 1rst one */
	if (event == __os_current_event) {
		/* Modify the active event pointer to point on the next element
		 */
		__os_current_event = event->next;
	}
	/* The event is not the 1rst one */
	else {
		/* Look for the event in the list */
		while (prev_event->next != event) {
			prev_event = prev_event->next;
		}
		/* Remove the event from the list */
		prev_event->next = event->next;
	}
}

/*! \brief Add a new event to the active event list
 * \param event The event to be added
 * \warning The event must not be active
 */
static inline void __os_event_enable(struct os_event *event) {
	/* If the active event list is not empty */
	if (__os_current_event) {
		event->next = __os_current_event;
	}
	/* The active event list is empty */
	else {
		event->next = NULL;
	}
	/* The new event will be the 1rst one in the list */
	__os_current_event = event;
}

/*! \brief Initialize the event by calling its setup function
 * \param event The event to start
 * \param proc The process associated with this event
 */
static inline void __os_event_start(struct os_event *event,
		struct os_process *proc) {
	/* If this event has a setup function */
	if (event->desc.start) {
		/* Call it with the appropriate arguments */
		event->desc.start(proc, event->args);
	}
}

/*! \brief Register an event into the active event list
 * \param event The event to register
 * \param queue_elt The queue element structure to add to the list
 * \param proc The process associated with this queue element
 * \param event_triggered The pointer on the variable to update to notify which
 * event has triggered
 * \warning This function must be called inside a critical section
 */
static void __os_event_register(struct os_event *event,
		struct os_queue_event *queue_elt, struct os_process *proc,
		struct os_event **event_triggered)
{
	os_queue_doubly_sort_t sort_fct;

	/* Get the appropriate sorting function */
#if CONFIG_OS_USE_PRIORITY == true
	sort_fct = os_queue_doubly_process_sort_priority;
#else
	sort_fct = (os_queue_doubly_sort_t) os_queue_sort_fifo;
#endif
	/* If a custom sorting function is defined for this event, use it */
	if (event->desc.sort) {
		sort_fct = event->desc.sort;
	}

	/* Enable the event process if not done already, before messing up
	 * with the process context
	 */
	__os_process_event_enable();

	/* Add this event to the event list if not done already */
	if (!__os_event_is_enabled(event)) {
		__os_event_enable(event);
	}

	/* Assign the process to the queue element */
	queue_elt->proc = proc;
	/* Assign the variable to update when the event has been triggered */
	queue_elt->event_triggered = event_triggered;

	/* Add the process to the event sorted process list */
	os_queue_event_add_sort(__os_event_get_queue_ptr(event),
			queue_elt, sort_fct);
}

/*! \brief Event scheduler. This function will check the evnet status and wake
 * up the associated processes accordingly. It will then make sure the active
 * event list is clean.
 */
void __os_event_scheduler(void)
{
	/* To hold the current event to process */
	struct os_event *event;
	/* Status of the current event */
	enum os_event_status status = OS_EVENT_NONE;

	/* All the following cannot be interrupted since manupulating the
	 * active event list is critical.
	 */
	os_enter_critical();

	/* Start with the first event to process */
	event = __os_current_event;

	/* If no events, disable the event process */
	if (!event) {
		/* Disable the event process */
		__os_process_event_disable();
	}

	/* Loop inside the active event list */
	while (event) {
		/* Loop until the status of the current event is "stop" */
		do {
			/* If the process queue of the current event is not
			 * empty
			 */
			if (!__os_event_is_empty(event)) {
				struct os_queue_event *queue_elt;
				/* Get the first process associated with the
				 * current event
				 */
				queue_elt = os_queue_event_head(
						__os_event_get_queue(event));
				/* Make sure the process is in pending state */
				if (__os_process_is_pending(queue_elt->proc)) {
					/* Check if the event has been triggered
					 */
					status = event->desc.is_triggered(
							queue_elt->proc,
							event->args);
					/* If the event has triggered */
					if (status != OS_EVENT_NONE) {
						struct os_queue_event *current_elt;
						/* Update the event feedback
						 * variable
						 */
						*queue_elt->event_triggered =
								event;
						/* Remove the process from the
						 * event list
						 */
						os_queue_event_pop(
								__os_event_get_queue_ptr(event));
						/* Activate the process */
						__os_process_enable_naked(queue_elt->proc);
						/* Garbage collect, remove the other
						 * queued entries associated to this process
						 */
						current_elt = queue_elt->relation;
						/* Loop through the other queued entries
						 * assicoated with this process and remove
						 * them
						 */
						while (current_elt != queue_elt) {
							os_queue_event_remove(current_elt);
							current_elt = current_elt->relation;
						}
					}
				}
				/* If a process is not pending, pop this process out
				 * of the event list. This should never happen.
				 */
				else {
					// while (true);
					/* Remove the not pending process from
					 * the list.
					 */
					os_queue_event_pop(__os_event_get_queue_ptr(event));
					/* Continue processing the next process
					 * in the list.
					 */
					status = OS_EVENT_OK_CONTINUE;
				}
			}

		/* Loop until the status is set to "continue" */
		} while (status == OS_EVENT_OK_CONTINUE);
		/* Get the next active event */
		event = event->next;
	}

	/* Start with the first event to process */
	event = __os_current_event;
	/* Garbage collector, test if there are empty events, if so remove them
	 */
	while (event) {
		/* If the process queue of the current event is empty,
		 * remove this event from the active event list.
		 */
		if (__os_event_is_empty(event)) {
			/* Change the status to exit the main loop */
			status = OS_EVENT_NONE;
			/* Remove this event from the active event list
			 */
			__os_event_pop(event);
		}
		event = event->next;
	}

	/* Manually call the scheduler to schedule the next process */
	os_switch_context(false);

	/* Leave the critical area */
	os_leave_critical();
}

enum os_event_status __os_event_custom_function_handler(struct os_process *proc,
		os_ptr_t args)
{
	/* The pointer is cast to a structure of type
	 * __os_event_custom_function_args
	 */
	struct __os_event_custom_function_args *custom_args =
			(struct __os_event_custom_function_args *) args;
	/* Call the trigger function to test if the event has been triggered */
	if (custom_args->trigger(custom_args->args)) {
		/* If yes, continue looking */
		return OS_EVENT_OK_CONTINUE;
	}
	/* If not, stop */
	return OS_EVENT_NONE;
}

struct os_event *__os_process_sleep(struct os_process *proc,
		struct os_queue_event *queue_elt, int nb_events, ...)
{
	int i;
	va_list ap;
	struct os_event *event, *event_triggered = NULL;
	/* Save the critical region status */
	bool is_critical = os_is_critical();

	/* Enter in a critical region if not already in */
	if (!is_critical) {
		os_enter_critical();
	}

	/* Disable the process (send it to sleep) */
	if (__os_process_is_enabled(proc)) {
		__os_process_disable_naked(proc);
	}

	/* Set the process status to pending */
	proc->status = OS_PROCESS_PENDING;

	/* No event has been triggered so far so update the feedback variable
	 * to NULL.
	 */
	event_triggered = NULL;

	/* Initialize relation pointer between the queue elements */
	queue_elt[0].relation = &queue_elt[0];

	va_start(ap, nb_events);
	i = nb_events;
	while (i--) {
		/* Get the next event from the list */
		event = va_arg(ap, struct os_event *);
		/* Start the event */
		__os_event_start(event, proc);
		/* Register the process in the active event list */
		__os_event_register(event, &queue_elt[i], proc, &event_triggered);
		/* Build a relation between the processes registered */
		if (i) {
			queue_elt[i].relation = queue_elt[0].relation;
			queue_elt[0].relation = &queue_elt[i];
		}
	}
	va_end(ap);

	/* If the process to be send to sleep is thye current process, stop it
	 * and use a garbage collector wipe out the extra events registered.
	 */
	if (proc == __os_process_get_current()) {
		/* Call the scheduler */
		os_switch_context(false);
	}

	/* Leave the critical region */
	if (!is_critical) {
		os_leave_critical();
	}

	/* Returns a pointer on the event which has woken up the process */
	return event_triggered;
}


#if CONFIG_OS_USE_SW_INTERRUPTS == true
	#warning TODO the interrupt trigger_on_event function
void os_interrupt_trigger_on_event(struct os_interrupt *interrupt,
		struct os_event *event)
{
	/* Save the critical region status */
	bool is_critical = os_is_critical();
	/* Enter in a critical region if not already in */
	if (!is_critical) {
		os_enter_critical();
	}
	//__os_event_start(event);
	//__os_event_register(event, __os_interrupt_get_process(interrupt));
	if (!is_critical) {
		os_leave_critical();
	}
}
#endif

#endif // CONFIG_OS_USE_EVENTS == true
