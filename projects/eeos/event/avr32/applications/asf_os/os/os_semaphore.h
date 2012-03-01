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

#ifndef __OS_SEMAPHORE_H__
#define __OS_SEMAPHORE_H__

/* Configuration options ******************************************************/

/*! \def CONFIG_OS_SEMAPHORE_USE_8BIT_COUNTER
 * \brief Use a 8-bit variable to count the number of sempahore available. In
 * other words, if this option is set to true, the user can use up to 255
 * semaphores. If set to false, a 16-bit variable will be used instead alowing
 * up to 65,535 sempahores.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_SEMAPHORE_USE_8BIT_COUNTER
	#define CONFIG_OS_SEMAPHORE_USE_8BIT_COUNTER true
#endif

/* Types **********************************************************************/

/*! \brief Type of the sempahore counter. Its size can vary from 8-bits to
 * 16-bits using the option \ref CONFIG_OS_SEMAPHORE_USE_8BIT_COUNTER.
 */
#if CONFIG_OS_SEMAPHORE_USE_8BIT_COUNTER == true
typedef uint8_t os_semaphore_counter_t;
#else
typedef uint16_t os_semaphore_counter_t;
#endif

/*! \brief Semaphore Structure
 */
struct os_semaphore {
	/*! \brief Counter to hold the current number of free semaphores
	 */
	os_semaphore_counter_t counter;
	/*! \brief Maximum semaphore available
	 */
	os_semaphore_counter_t max;
	/*! \brief Next processes on the waiting list
	 */
	struct os_queue_process *queue;
};

/* Internal API ***************************************************************/

/*! \brief Event trigger function. Take the semphore, use by the event.
 * \param proc The process waiting for the event.
 * \param args Extra arguments send with this event.
 * \return OS_EVENT_NONE if no ressource is available. OS_EVENT_OK_STOP if
 * only 1 is available, OS_EVENT_OK_CONTINUE if more than one.
 */
enum os_event_status __os_event_sempahore_is_triggered(struct os_process *proc,
		os_ptr_t args);

/* Public API *****************************************************************/

/*! \name Semaphores
 *
 * Set of functions to create and control sempahores
 *
 * \{
 */

/*! \brief Creates a counting semaphore
 * \ingroup group_os_public_api
 * \param sem The un-initialized sempahore structure
 * \param counter The maximum count value that can be reached
 * \param initial_count The count value assigned to the semaphore when it is
 * created
 */
static inline void os_semaphore_create(struct os_semaphore *sem,
		os_semaphore_counter_t counter,
		os_semaphore_counter_t initial_count) {
	sem->counter = initial_count;
	sem->max = counter;
	sem->queue = NULL;
}

/*! \brief Creates a binary semaphore
 * \ingroup group_os_public_api
 * \param sem The un-initialized sempahore structure
 */
static inline void os_binary_semaphore_create(struct os_semaphore *sem) {
	os_semaphore_create(sem, 1, 1);
}

/*! \brief Creates an event from a semaphore. The sempahore must have been
 * previously created before using this function
 * \ingroup group_os_public_api
 * \param event The un-initialized event structure
 * \param sem The semaphore which will be linked to this event
 * \pre \ref CONFIG_OS_USE_EVENTS must be set
 */
static inline void os_semaphore_create_event(struct os_event *event,
		struct os_semaphore *sem) {
	/* Event descriptor for the semaphore */
	const struct os_event_descriptor __semaphore_event_descriptor = {
		.is_triggered = __os_event_sempahore_is_triggered
	};
	__os_event_create(event, &__semaphore_event_descriptor, (os_ptr_t) sem);
}

/*! \brief Get a semaphore out of a \ref os_event structure.
 * \ingroup group_os_public_api
 * \param event The sempahore event
 * \return The \ref os_semaphore structure
 * \pre The event must have been generate from \ref os_semaphore_create_event
 */
static inline struct os_semaphore *os_event_get_semaphore(struct os_event *event) {
	return (struct os_semaphore *) event->args;
}

/*! \brief Take a semaphore. If no semaphore is available, wait until it gets
 * free
 * \ingroup group_os_public_api
 * \param sem The semaphore to take
 * \pre The semaphore must have previously been created
 */
void os_semaphore_take(struct os_semaphore *sem);

/*! \brief Releases a semaphore.
 * \ingroup group_os_public_api
 * \param sem The semaphore to release
 * \pre The semaphore must have previously been created
 */
void os_semaphore_release(struct os_semaphore *sem);

/*!
 * \}
 */

#endif // __OS_SEMAPHORE_H__
