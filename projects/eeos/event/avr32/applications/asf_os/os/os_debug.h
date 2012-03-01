/*! \file
 * \brief eeOS Debug
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

#ifndef __OS_DEBUG_H__
#define __OS_DEBUG_H__

#if CONFIG_OS_DEBUG == true

/*! \defgroup group_os_debug Debug Mode
 * The debug mode will do the following:
 * - Initially fill the stack with a predefined pattern
 * (\ref CONFIG_OS_DEBUG_UINT8_PATTERN).
 * - Catch stack overflow exceptions.
 * - Trace the execution of the program (need to enable
 * \ref CONFIG_OS_DEBUG_USE_TRACE)
 */

/* Configuration options ******************************************************/

/*! \def CONFIG_OS_DEBUG_USE_TRACE
 * \ingroup group_os_config
 * \brief Activates the trace functionnality
 * \pre Used when \ref CONFIG_OS_DEBUG is set to true
 */
#ifndef CONFIG_OS_DEBUG_USE_TRACE
	#define CONFIG_OS_DEBUG_USE_TRACE false
#endif

/*! \def CONFIG_OS_DEBUG_UINT8_PATTERN
 * \ingroup group_os_config
 * \brief Defines the default character used to initialy fill the stack
 * \pre Used when \ref CONFIG_OS_DEBUG is set to true
 */
#ifndef CONFIG_OS_DEBUG_UINT8_PATTERN
	#define CONFIG_OS_DEBUG_UINT8_PATTERN 0xaa
#endif

/* Hooks **********************************************************************/

/*! \def HOOK_OS_DEBUG_STACK_OVERFLOW
 * \brief This hook is called when a stack overflow error is detected.
 * \ingroup os_hook
 * \pre \ref CONFIG_OS_DEBUG must be set to true
 */
#ifndef HOOK_OS_DEBUG_STACK_OVERFLOW
	#define HOOK_OS_DEBUG_STACK_OVERFLOW()
#endif

/*! \def HOOK_OS_DEBUG_TRACE
 * \ingroup os_hook
 * \brief Hook called before each trace is logged
 * \param event The event to be traced (value from \ref os_debug_trace_event)
 * \param data Data associated to the event. It depends on the event.
 * \pre \ref CONFIG_OS_DEBUG_USE_TRACE must be set
 */
#ifndef HOOK_OS_DEBUG_TRACE
	#define HOOK_OS_DEBUG_TRACE(event, data)
#endif

/* Macros *********************************************************************/

/*! \brief Trace a code execution.
 * This macro is called to trace the CPU execution.
 * \param event Event identifier (\ref os_debug_trace_event) corresponding to
 * the code to be trace.
 * \param data Extra data to associate with the trace
 */
#if CONFIG_OS_DEBUG_USE_TRACE == true
	#define __HOOK_OS_DEBUG_TRACE_LOG(event, data) \
		do { \
			HOOK_OS_DEBUG_TRACE(event, (os_ptr_t) data); \
			__os_debug_trace_log(event, (os_ptr_t) data); \
		} while (false)
#endif

/*! \brief Application stack initialization. This macro is called once at the
 * beginning of the system to fill the stack of the application process with a
 * custom pattern. This is used to follow the consumption of the stack.
 */
#define __HOOK_OS_DEBUG_APP_INIT() \
		do { \
		} while (false);

/*! \brief Task stack initialization. This will fill the stack of a task with
 * a known pattern.
 */
#define __HOOK_OS_DEBUG_TASK_ADD() \
		do { \
			int i; \
			for (i=0; i<stack_size; i++) { \
				((uint8_t *) task->stack)[i] = \
						CONFIG_OS_DEBUG_UINT8_PATTERN; \
			} \
		} while (false);

/*! \brief Macro called during each tick interrupt. This macro will check the
 * status of the stack to detect if it has overflown.
 */
#define __HOOK_OS_DEBUG_TICK() \
		do { \
			struct os_task *current_task = os_task_get_current(); \
			if (current_task != NULL && \
					*((uint8_t *) current_task->stack) != \
					CONFIG_OS_DEBUG_UINT8_PATTERN) { \
				HOOK_OS_DEBUG_STACK_OVERFLOW(); \
				while (true); \
			} \
		} while (false);

/* Types **********************************************************************/

/*! \brief Identifier of function entry points used by the trace functionality.
 */
enum os_debug_trace_event {
	/* OS core related */
	/*! \brief When \ref __os_scheduler is called */
	OS_DEBUG_TRACE_CONTEXT_SWITCH = 0x01,
	/*! \brief When \ref os_yield is called */
	OS_DEBUG_TRACE_YIELD = 0x02,

	/* Task related */
	/*! \brief When \ref os_task_create is called */
	OS_DEBUG_TRACE_TASK_CREATE = 0x10,
	/*! \brief When \ref os_task_set_priority is called */
	OS_DEBUG_TRACE_TASK_SET_PRIORITY = 0x11,
	/*! \brief When \ref os_task_get_priority is called */
	OS_DEBUG_TRACE_TASK_GET_PRIORITY = 0x12,
	/*! \brief When \ref os_task_delete is called */
	OS_DEBUG_TRACE_TASK_DELETE = 0x13,
	/*! \brief When \ref os_task_enable is called */
	OS_DEBUG_TRACE_TASK_ENABLE = 0x14,
	/*! \brief When \ref os_task_disable is called */
	OS_DEBUG_TRACE_TASK_DISABLE = 0x15,
	/*! \brief When \ref os_task_delay is called */
	OS_DEBUG_TRACE_TASK_DELAY_START = 0x16,
	/*! \brief When \ref os_task_delay is done */
	OS_DEBUG_TRACE_TASK_DELAY_STOP = 0x17,

	/* Software interrupt related */
	/*! \brief When \ref os_interrupt_create is called */
	OS_DEBUG_TRACE_INTERRUPT_CREATE = 0x20,
	/*! \brief When \ref os_interrupt_trigger is called */
	OS_DEBUG_TRACE_INTERRUPT_TRIGGER = 0x21,
	/*! \brief When \ref os_interrupt_set_priority is called */
	OS_DEBUG_TRACE_INTERRUPT_SET_PRIORITY = 0x22,
	/*! \brief When \ref os_interrupt_get_priority is called */
	OS_DEBUG_TRACE_INTERRUPT_GET_PRIORITY = 0x23,
};

/*! \brief Trace structure. This structure contains the format of a trace entry.
 */
struct os_trace {
	/*! \brief The time (in cycle counter) when the trace has been taken */
	os_cy_t time;
	/*! \brief The event identifier associated with this trace */
	enum os_debug_trace_event event;
	/*! \brief Data associated with this trace */
	os_ptr_t data;
};

/* Internal API ***************************************************************/

/*! \brief This function is called to log a trace entry.
 * \ingroup group_os_internal_api
 * \param event The event identifier to be logged
 * \param data The data associated with the trace
 * \pre \ref os_debug_start_trace must have been called first.
 */
void __os_debug_trace_log(enum os_debug_trace_event event, os_ptr_t data);

/* Public API *****************************************************************/

/*! \name Debug
 *
 * Useful function set for debugging purpose.
 * \pre \ref CONFIG_OS_DEBUG must be set
 *
 * \{
 */

/*! \brief Stop tracing the operating system execution.
 * \ingroup group_os_public_api
 * \pre \ref os_debug_start_trace must be called first
 * \pre \ref CONFIG_OS_DEBUG_USE_TRACE must be set
 */
static inline void os_debug_stop_trace(void) {
	extern bool os_debug_trace_flag;
	/* Disable the trace */
	os_debug_trace_flag = false;
}

/*! \brief Start tracing the operation system execution. The trace will be
 * saved inside a buffer and will be wrapped if overflowed.
 * \ingroup group_os_public_api
 * \param buffer Memory buffer to hold the trace
 * \param size The size in bytes of the memory buffer
 * \pre \ref CONFIG_OS_DEBUG_USE_TRACE must be set
 */
void os_debug_start_trace(os_ptr_t buffer, int size);

/*! \brief Get the current trace pointer. This will point on the next trace
 * entry to be filled. It is part of the memory previously allocated by
 * \ref os_debug_start_trace.
 * \ingroup group_os_public_api
 * \return A pointer on the next trace entry.
 * \pre \ref os_debug_start_trace must be called first
 * \pre \ref CONFIG_OS_DEBUG_USE_TRACE must be set
 */
static inline struct os_trace *os_debug_trace_get_pointer(void) {
	extern struct os_trace *os_debug_trace_ptr;
	return os_debug_trace_ptr;
}

/*!
 * \}
 */

#endif // CONFIG_OS_DEBUG == true

#ifndef __HOOK_OS_DEBUG_TRACE_LOG
	#define __HOOK_OS_DEBUG_TRACE_LOG(event, data)
#endif

#endif // __OS_DEBUG_H__
