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

#ifndef __OS_EVENT_H__
#define __OS_EVENT_H__

/*! \addtogroup group_os
 * \section section_os_event Events
 *
 * Events (\ref os_event) are used to wake up or call one or multiple processes.
 * When a process is sleeping it will be removed from the active process list,
 * therefore the performance will not be decreased.
 *
 * An event can wake up one or multiple process. A process can be waken up by
 * one or multiple events as well. If priorities are enabled, the first process
 * waken up by an event will be the one with the highest priority.
 *
 * Events are stored in a chain list as follow, and each evetn has a list of
 * associated processes:
 * \image html event.png
 *
 * When an event has no associated process, it is considered as inactive and is
 * removed from the active event list.
 *
 * The event scheduler runs with the application process. They share the same
 * context and therefore the same stack. You can change the priority of the
 * event process (in other word, you can change the priority of the event
 * scheduler). This will not affect the priority of the application process
 * since this process is active only when no other processes are actives.
 */

/* Configuration options ******************************************************/

/*! \def CONFIG_OS_USE_EVENTS
 * \brief Use this option to enable event support.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_USE_EVENTS
	#define CONFIG_OS_USE_EVENTS false
#endif

/* Types **********************************************************************/

/*! \brief Status returned by an event
 */
enum os_event_status {
	/*! \brief No event has occured
	 */
	OS_EVENT_NONE = -1,
	/*! \brief An event has been triggered for this process. If another
	 * process is sharing the same event, there is no need to test if this
	 * event is also valid for it. In other word, this event is only valid
	 * for 1 process.
	 */
	OS_EVENT_OK_STOP = 0,
	/*! \brief An event has been triggered for this process. If another
	 * process is sharing the same event, we might want to also chech if this
	 * event is also valid. In other word, this event might be valid for
	 * more than 1 process.
	 */
	OS_EVENT_OK_CONTINUE = 1,
};

/*! \brief Event descriptor
 */
struct os_event_descriptor {
	/*! \brief Sorting function. This function compare 2 processes.
	 * The following helper functions are available:
	 * - \ref os_queue_sort_fifo
	 * - \ref os_queue_sort_lifo
	 * \note This function is optional. By default the new process will added
	 * using the \ref os_queue_sort_fifo algorithm or the
	 * \ref os_queue_doubly_process_sort_priority alorithm, depending if priorities
	 * are enabled or not.
	 */
	os_queue_doubly_sort_t sort;
	/*! \brief This function will handle the setup of the event. For
	 * example, if the event is a timer, the timer will start to run after
	 * the call of this function.
	 * \param proc The process associated with this event
	 * \param args Argument passed to the event during its creation
	 */
	void (*start)(struct os_process *proc, os_ptr_t args);
	/*! \brief Check the status of an event
	 * \note This function is optional.
	 * \param proc The process of the process which is intented to triggered
	 * \param args Argument passed to the event during its creation
	 * \return The current event status (\ref os_event_status)
	 */
	enum os_event_status (*is_triggered)(struct os_process *proc, os_ptr_t args);
};

/*! \struct os_queue_event
 * \brief Event queue structure definition (see
 * \ref OS_QUEUE_DOUBLY_DEFINE for more details). This structure is used
 * to hold and link a process with an event.
 */
OS_QUEUE_DOUBLY_DEFINE(event,
	/*! Process associated with the event */
	struct os_process *proc;
	/*! Pointer on a variable to be notified when the event triggers
	 */
	struct os_event **event_triggered;
	/*! To keep track of the similar process entries created and associated
	 * with other processes
	 */
	struct os_queue_event *relation;
);

/*! \brief Event structure
 */
struct os_event {
	/*! \brief This structure contains the event descriptor functions. These
	 * functions will define the behavior of this event.
	 */
	struct os_event_descriptor desc;
	/*! \brief This is the starting point of the process chain list associated
	 * with this event. The last process is followed by a NULL pointer.
	 */
	struct os_queue_doubly queue;
	/*! \brief Next event in the chain list. Last event is followed by a
	 * NULL pointer.
	 */
	struct os_event *next;
	/*! \brief Extra arguments used to define this event.
	 */
	os_ptr_t args;
};

/*! \brief Internal structure to fit a boolean function in a event
 */
struct __os_event_custom_function_args {
	/* Pointer on the boolean function */
	bool (*trigger)(os_ptr_t);
	/* Arguments that will be passed to the boolean function */
	os_ptr_t args;
};

/* Internal API ***************************************************************/

/*! \brief Create a new event
 * \ingroup group_os_internal_api
 * \param event A non-initialized event structure to hold the context of this
 * event
 * \param descriptor The event description structure which defines the type of
 * event to be used.
 * \param args Argument which will be passed to the event descriptor functions.
 */
void __os_event_create(struct os_event *event,
		const struct os_event_descriptor *descriptor, os_ptr_t args);

/*! \brief Internal handler for the boolean function
 * \ingroup group_os_internal_api
 * \param proc Process associated to the event
 * \param args Extra arguments to pass to this function. Arguments are a pointer
 * of type \ref __os_event_custom_function_args
 * \return The status of the trigger function
 */
enum os_event_status __os_event_custom_function_handler(struct os_process *proc,
		os_ptr_t args);

/*! \brief Event scheduler
 * \ingroup group_os_internal_api
 */
void __os_event_scheduler(void);

/*! \brief This function test a event structure and notifies if it has any
 * remaining process in its queue.
 * \ingroup group_os_internal_api
 * \param event The event to be tested
 * \return true if there are no process in the queue, false otherwise.
 */
static inline bool __os_event_is_empty(struct os_event *event) {
	return (bool) !(event->queue.next);
}

/*! \brief Checks if an event is enabled, in other word, test if it is in the
 * active event list.
 * \ingroup group_os_internal_api
 * \param event The event to be checked
 * \return true if the event is enabled, false otherwise.
 */
static inline bool __os_event_is_enabled(struct os_event *event) {
	return (bool) (event->queue.next);
}

/*! \brief Generic function to send a processus to sleep. The process can be
 * waken up by one or more events passed in parameter.
 * \ingroup group_os_internal_api
 * \param proc The processus to send to sleep
 * \param queue_elt The empty \ref os_queue_event structure to hold each
 * process instance in the events used to wake up the process.
 * \param nb_events The number of events defined to wake up this process
 * \param ... A list of \ref os_event which can wake up this process
 * \return A pointer on the event which woke up the process
 */
struct os_event *__os_process_sleep(struct os_process *proc,
		struct os_queue_event *queue_elt, int nb_events, ...);

/*! \brief Associate a process with an event and enable the event
 * \ingroup group_os_internal_api
 * \param event The event to receive the process
 * \param queue_elt A unitialized queue structure used to hold the process
 * in the queue.
 * \param proc The process to add
 * \param event_triggered A pointer to update which will reflect the event that
 * has been triggered
 */
void __os_event_register(struct os_event *event, struct os_queue_event *queue_elt,
		struct os_process *proc, struct os_event **event_triggered);

/*! \brief Get the \ref os_queue_event pointer associated with an event
 * \ingroup group_os_internal_api
 * \param event The event from which we want to get the structure
 * \return The \ref os_queue_event pointer, or NULL if no processes are
 * associated with this event
 */
static inline struct os_queue_event *__os_event_get_queue(struct os_event *event) {
	return (struct os_queue_event *) event->queue.next;
}

/*! \brief Get the pointer on the variable that hold the \ref os_queue_event
 * structure associated with an event.
 * \ingroup group_os_internal_api
 * \param event The event we need to get data from
 * \return A pointer on the variable which holds the \ref os_queue_event
 * structure associated with this event
 */
static inline struct os_queue_event **__os_event_get_queue_ptr(struct os_event *event) {
	return (struct os_queue_event **) &event->queue.next;
}

/* Public API *****************************************************************/

/*! \name Events
 *
 * Set of functions to create and manage events
 *
 * \{
 */

/*! \brief Create a custom event from a function
 * \ingroup group_os_public_api
 * \param event The un-initialized event structure to be filled
 * \param trigger The boolean function to trigger the event. The event will be
 * triggered when this function returns true.
 * \param args Arguments to pass to the trigger function
 */
static inline void os_event_create_from_function(struct os_event *event,
		bool (*trigger)(os_ptr_t), os_ptr_t args) {
	/* Create the event descriptor function */
	const struct os_event_descriptor descriptor = {
		.is_triggered = __os_event_custom_function_handler
	};
	/* Create the boolean function */
	struct __os_event_custom_function_args custom_args = {
		.trigger = trigger,
		.args = args
	};
	/* Create an event out of this */
	__os_event_create(event, &descriptor, (os_ptr_t) &custom_args);
}

/*!
 * \}
 */

#endif // __OS_EVENT_H__
