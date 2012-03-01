/*! \file
 * \brief eeOS Tasks
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

#ifndef __OS_TASK_H__
#define __OS_TASK_H__

/*! \addtogroup group_os
 * \section section_os_process_task Task
 *
 * A task (\ref os_task) is a process which can be interrupted by the process
 * scheduler at any time. Therefore its execution time is not predictable
 * without a complete view of the active process list.
 */

/* Configuration options ******************************************************/

/*! \def CONFIG_OS_TASK_DEFAULT_PRIORITY
 * \brief Default priority assgined to a task
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_TASK_DEFAULT_PRIORITY
	#define CONFIG_OS_TASK_DEFAULT_PRIORITY OS_PRIORITY_1
#endif

/* Macros *********************************************************************/

/*! \brief Allocate memory for the stack
 *
 * This macro can be used with \ref OS_TASK_USE_CUSTOM_STACK in order to
 * manually allocate some memory for the stack.
 * Here is an example code to use this macro:
 * \code
 * void my_func(os_ptr_t args)
 * {
 *	...
 * }
 * static OS_MALLOC_STACK(my_stack, 1024);
 * struct os_task my_task;
 * my_task.stack = my_stack;
 * os_task_create(&my_task, my_func, NULL, 0, OS_TASK_USE_CUSTOM_STACK);
 * \endcode
 * \ingroup group_os_public_api
 * \param stack_symbol The symbol name used to refer to this stack
 * \param stack_size The size of the stack in bytes
 */
#define OS_MALLOC_STACK(stack_symbol, stack_size) \
		uint8_t (stack_symbol)[(stack_size)]

/* Types **********************************************************************/

enum os_task_option {
	/*! \brief Default options
	 */
	OS_TASK_DEFAULT = 0,
	/*! \brief Disable the task before its execution.\n
	 * It can be enable at any time using \ref os_task_enable
	 */
	OS_TASK_DISABLE = 1,
	/*! \brief Use a custom stack for this task. The user must previously allocate
	 * memory for \ref os_task::stack. This option is available only if
	 * \ref CONFIG_OS_USE_MALLOC is set.
	 */
	OS_TASK_USE_CUSTOM_STACK = 2,
};

/*! Structure holding the context of a task
 */
struct os_task {
	/*! \brief Minimal context
	 */
	struct os_process core;
	/*! \brief A pointer on a memory space reserved for the stack
	 */
	uint8_t *stack;
	/*! \brief Task options
	 */
	enum os_task_option options;
};

/* Internal API ***************************************************************/

/*! \brief Get the task associated with a process
 * \ingroup group_os_internal_api
 * \param proc the process
 * \return The task pointer
 */
static inline struct os_task *__os_task_from_process(struct os_process *proc) {
	return OS_CONTAINER_OF(proc, struct os_task, core);
}

/*! \brief Get the task process
 * \ingroup group_os_internal_api
 * \param task The task
 * \return The process of the task
 */
static inline struct os_process *__os_task_get_process(struct os_task *task) {
	return &task->core;
}

/* Public API *****************************************************************/

/*! \name Tasks
 *
 * Set of functions to manage a task
 *
 * \{
 */

/*! \brief Create a new task. By default, the new task will be automatically
 * added to the active task list unless specified.
 * \ingroup group_os_public_api
 * \param task A pointer on an empty structure which will contain the context of
 * the current task.
 * \param task_ptr Entry point of the task to be run.
 * \param args Arguments to pass to the task
 * \param stack_size The size of the stack in byte
 * \param options Specific options for the task (see \ref os_task_option)
 * \return true if the task has been correctly registered, false otherwise.
 */
bool os_task_create(struct os_task *task, os_proc_ptr_t task_ptr, os_ptr_t args,
		int stack_size, enum os_task_option options);

#if CONFIG_OS_USE_TICK_COUNTER == true
/*! \brief Block the execution of a task until a number of ticks have passed.
 * \ingroup group_os_public_api
 * \ref CONFIG_OS_TICK_HZ can be used to estimate a time delay.
 * \param tick_nb The number of ticks to wait for
 * \pre \ref CONFIG_OS_USE_TICK_COUNTER needs to be set first.
 * \warning This functon needs the preemptive scheduler to run. Therefore, it
 * cannot be used inside an interrupt or any other piece of code where the
 * tick interrupt is disabled.
 */
void os_task_delay(os_tick_t tick_nb);
#endif

#if CONFIG_OS_USE_PRIORITY == true
/*! \brief Set a priority to a task
 * \ingroup group_os_public_api
 * \param task The task
 * \param priority The priority to set
 */
static inline void os_task_set_priority(struct os_task *task,
		enum os_priority priority) {
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_TASK_SET_PRIORITY, priority);
	__os_process_set_priority(__os_task_get_process(task), priority);
}
/*! \brief Get the priority of a task
 * \ingroup group_os_public_api
 * \param task The task
 * \return The priority of the task
 */
static inline enum os_priority os_task_get_priority(struct os_task *task) {
	enum os_priority priority;
	priority = __os_process_get_priority(__os_task_get_process(task));
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_TASK_GET_PRIORITY, priority);
	return (enum os_priority) priority;
}
#endif

/*! \brief Delete a task
 * \ingroup group_os_public_api
 * \param task The task to be deleted
 */
static inline void os_task_delete(struct os_task *task) {
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_TASK_DELETE, task);
	__os_process_disable(__os_task_get_process(task));
	// Free the task stack if needed
	if (!(task->options & OS_TASK_USE_CUSTOM_STACK)) {
		os_free(task->stack);
	}
}

/*! \brief Enable the execution a task
 * \ingroup group_os_public_api
 * \param task The task to be enabled
 */
static inline void os_task_enable(struct os_task *task) {
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_TASK_ENABLE, task);
	__os_process_enable(__os_task_get_process(task));
}

/*! \brief Disable the execution of a task
 * \ingroup group_os_public_api
 * \param task The task to be disabled
 */
static inline void os_task_disable(struct os_task *task) {
	__HOOK_OS_DEBUG_TRACE_LOG(OS_DEBUG_TRACE_TASK_DISABLE, task);
	__os_process_disable(__os_task_get_process(task));
}

/*! \brief Check wether a task is enabled or not
 * \ingroup group_os_public_api
 * \param task The task to be checked
 * \return true if enabled, false otherwise
 */
static inline bool os_task_is_enabled(struct os_task *task) {
	return __os_process_is_enabled(__os_task_get_process(task));
}

/*! \brief Get the current running task
 * \ingroup group_os_public_api
 * \return the current task. NULL if none is running.
 */
static inline struct os_task *os_task_get_current(void) {
	if (__os_process_is_task(__os_process_get_current())) {
		return __os_task_from_process(__os_process_get_current());
	}
	return NULL;
}

#if CONFIG_OS_USE_EVENTS == true

/*! \brief Send the task to sleep and wake it up uppon a specific event
 * \ingroup group_os_public_api
 * \param ... List of events (\ref os_event) used to wakeup the task
 * \pre \ref CONFIG_OS_USE_EVENTS needs to be set
 */
#define os_task_sleep(...) \
		do { \
			struct os_queue_event __queue_elt[OS_NB_ARGS(__VA_ARGS__)]; \
			__os_process_sleep(__os_process_get_current(), \
				__queue_elt, OS_NB_ARGS(__VA_ARGS__), \
				__VA_ARGS__); \
		} while (false);

/*! \brief Send the task to sleep and wake it up uppon a specific event
 * \ingroup group_os_public_api
 * \param event_triggered An empty \ref os_event pointer which will point on
 * the event which triggered the wake up of the process.
 * \param ... List of events (\ref os_event) used to wakeup the task
 * \pre \ref CONFIG_OS_USE_EVENTS needs to be set
 */
#define os_task_sleep_ex(event_triggered, ...) \
		do { \
			struct os_queue_event __queue_elt[OS_NB_ARGS(__VA_ARGS__)]; \
			event_triggered = __os_process_sleep( \
					__os_process_get_current(), \
				__queue_elt, OS_NB_ARGS(__VA_ARGS__), \
				__VA_ARGS__); \
		} while (false);

#endif

/*!
 * \}
 */

#endif // __OS_TASK_H__
