/*! \file
 * \brief eeOS Software Interrupts
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

#ifndef __OS_INTERRUPT_H__
#define __OS_INTERRUPT_H__

/*! \addtogroup group_os
 * \section section_os_process_interrupt Software Interrupt
 *
 * A software interrupt (\ref os_interrupt) is a process which will not be
 * interrupted by the process scheduler. An interrupt uses the same stack as the
 * application process.
 */

/* Configuration options ******************************************************/

/*! \def CONFIG_OS_USE_SW_INTERRUPTS
 * \brief Use this option to enable software interrupts.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_USE_SW_INTERRUPTS
	#define CONFIG_OS_USE_SW_INTERRUPTS false
#endif

/*! \def CONFIG_OS_INTERRUPT_DEFAULT_PRIORITY
 * \brief Default priority assgined to an interrupt
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_INTERRUPT_DEFAULT_PRIORITY
	#define CONFIG_OS_INTERRUPT_DEFAULT_PRIORITY OS_PRIORITY_1
#endif

/* Macros *********************************************************************/

#if CONFIG_OS_USE_SW_INTERRUPTS == true
	#define OS_SCHEDULER_PRE_INTERRUPT_HOOK() \
		do { \
			if (__os_current_process->sp) { \
				__HOOK_OS_DEBUG_TRACE_LOG( \
						OS_DEBUG_TRACE_CONTEXT_SWITCH, \
						__os_current_process); \
				return __os_current_process; \
			} \
			__os_current_process->sp = os_app.sp; \
			os_process_context_load(__os_current_process, \
					__os_interrupt_handler, \
					(os_ptr_t) __os_current_process); \
		} while (false)

	#define OS_SCHEDULER_POST_INTERRUPT_HOOK() \
		do { \
			if (__os_process_is_interrupt( \
					__os_process_get_current())) { \
				__os_process_get_current()->sp = NULL; \
			} \
		} while (false)
#endif

/* Types **********************************************************************/

/*! \brief Software interrupt structure
 */
struct os_interrupt {
	/*! \brief Task context of the interrupt
	 */
	struct os_process core;
	/*! \brief Pointer in the interrupt handler
	 */
	os_proc_ptr_t int_ptr;
	/*! \brief Arguments to pass to the interrupt handler
	 */
	os_ptr_t args;
};

/* Internal API ***************************************************************/

/*! \brief Get the interrupt associated with a process
 * \ingroup group_os_internal_api
 * \param proc the process
 * \return The interrupt pointer
 */
static inline struct os_interrupt *__os_interrupt_from_process(
		struct os_process *proc) {
	return OS_CONTAINER_OF(proc, struct os_interrupt, core);
}

/*! \brief Get the interrupt process
 * \ingroup group_os_internal_api
 * \param interrupt The interrupt
 * \return The process of the interrupt
 */
static inline struct os_process *__os_interrupt_get_process(
		struct os_interrupt *interrupt) {
	return &interrupt->core;
}

/*! \brief Software interrupt handler
 * \ingroup group_os_internal_api
 * \param args A pointer on a \ref os_interrupt structure
 */
void __os_interrupt_handler(os_ptr_t args);

/* Public API *****************************************************************/

/*! \name Software Interrupts
 *
 * A software interrupt is a task which runs from the application context and
 * it cannot be interrupted by the scheduler.
 * Software interrupts are scheduled with the task scheduler. They inherit
 * from the same priority scheme than a task.
 *
 * \pre \ref CONFIG_OS_USE_SW_INTERRUPTS needs to be set
 *
 * \{
 */

/*! \brief Setup a software interrupt
 * \ingroup group_os_public_api
 * \param interrupt The non-initialized structure to hold the context of the
 * software interrupt
 * \param int_ptr A pointer on the interrupt handler (a interrupt handler is a
 * normal function which follow the \ref os_proc_ptr_t prototype)
 * \param args Arguments to pass to the inerrupt handler
 */
void os_interrupt_create(struct os_interrupt *interrupt, os_proc_ptr_t int_ptr,
		os_ptr_t args);

/*! \brief Manually trigger a software interrupt.
 * \ingroup group_os_public_api
 * \param interrupt The interrupt to trigger
 * \pre The interrupt must be previously setup with \ref os_interrupt_create
 */
static inline void os_interrupt_trigger(struct os_interrupt *interrupt) {
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_INTERRUPT_TRIGGER, interrupt);
	__os_process_enable(__os_interrupt_get_process(interrupt));
}

#if CONFIG_OS_USE_PRIORITY == true
/*! \brief Change the priority of a software interrupt
 * \ingroup group_os_public_api
 * \param interrupt The interrupt which needs some update
 * \param priority The new priority
 * \pre \ref CONFIG_OS_USE_PRIORITY needs to be set first
 */
static inline void os_interrupt_set_priority(struct os_interrupt *interrupt,
		enum os_priority priority) {
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_INTERRUPT_SET_PRIORITY,
			priority);
	__os_process_set_priority(__os_interrupt_get_process(interrupt),
			priority);
}
/*! \brief Get the priority of a software interrupt
 * \ingroup group_os_public_api
 * \param interrupt The interrupt which priority is requested
 * \return The interrupt priority
 * \pre \ref CONFIG_OS_USE_PRIORITY needs to be set first
 */
static inline enum os_priority os_interrupt_get_priority(
		struct os_interrupt *interrupt) {
	enum os_priority priority;
	priority = __os_process_get_priority(__os_interrupt_get_process(
			interrupt));
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_INTERRUPT_GET_PRIORITY,
			priority);
	return (enum os_priority) priority;
}
#endif

#if CONFIG_OS_USE_EVENTS == true
/*! \brief Trigger an interrupt on a specific event
 * \ingroup group_os_public_api
 * \param interrupt The interrupt to wakeup
 * \param event The event used to trigger the interrupt
 * \pre \ref CONFIG_OS_USE_EVENTS needs to be set
 */
void os_interrupt_trigger_on_event(struct os_interrupt *interrupt,
		struct os_event *event);
#endif

/*!
 * \}
 */

#endif // __OS_INTERRUPT_H__
