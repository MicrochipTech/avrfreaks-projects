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

#ifndef __OS_MUTEX_H__
#define __OS_MUTEX_H__

/* Types **********************************************************************/

/*! \brief Mutex Structure
 */
struct os_mutex {
	/*! \brief Defines if a mutex is locked or not
	 */
	bool is_locked;
	/*! \brief The process which locked the mutex
	 */
	struct os_process *process;
	/*! \brief Next processes on the waiting list
	 */
	struct os_queue_process *queue;
};

/* Internal API ***************************************************************/

/*! \brief Declaration of the event trigger function for a mutex
 * \ingroup group_os_internal_api
 * \param proc The process associated with this event
 * \param args Extra arguments
 * \return Returned if the event triggered or not
 */
enum os_event_status __os_event_mutex_is_triggered(struct os_process *proc,
		os_ptr_t args);

/* Public API *****************************************************************/

/*! \name Mutex
 *
 * Set of functions to create and control mutex
 *
 * \{
 */

/*! \brief Creates a mutex
 * \ingroup group_os_public_api
 * \param mutex The un-initialized mutex structure
 */
static inline void os_mutex_create(struct os_mutex *mutex) {
	mutex->is_locked = false;
	mutex->queue = NULL;
}

/*! \brief Creates an event from a mutex. The mutex must have been
 * previously created before using this function
 * \ingroup group_os_public_api
 * \param event The un-initialized event structure
 * \param mutex The mutex which will be linked to this event
 * \pre \ref CONFIG_OS_USE_EVENTS must be set
 */
static inline void os_mutex_create_event(struct os_event *event,
		struct os_mutex *mutex) {
	const struct os_event_descriptor mutex_event_descriptor = {
		.is_triggered = __os_event_mutex_is_triggered
	};
	__os_event_create(event, &mutex_event_descriptor, (os_ptr_t) mutex);
}

/*! \brief Get a mutex out of a \ref os_event structure.
 * \ingroup group_os_public_api
 * \param event The mutex event
 * \return The \ref os_mutex structure
 * \pre The event must have been generate from \ref os_mutex_create_event
 */
static inline struct os_mutex *os_event_get_mutex(struct os_event *event) {
	return (struct os_mutex *) event->args;
}

/*! \brief Lock a mutex. If the mutex is already locked, wait until it gets
 * unlocked
 * \ingroup group_os_public_api
 * \param mutex The mutex to be locked
 * \pre The mutex must have previously been created
 */
void os_mutex_lock(struct os_mutex *mutex);

/*! \brief Un-lock a mutex.
 * \ingroup group_os_public_api
 * \param mutex The mutex to be unlocked
 * \pre The mutex must have previously been created
 */
void os_mutex_unlock(struct os_mutex *mutex);

/*!
 * \}
 */

#endif // __OS_MUTEX_H__
