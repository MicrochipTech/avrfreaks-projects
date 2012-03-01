/*! \file
 * \brief eeOS Statistics
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

#ifndef __OS_STATISTICS_H__
#define __OS_STATISTICS_H__

/* Configuration options ******************************************************/

/*! \def CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH
 * \brief Give statistics about the task switching.
 * This enables the following functions:
 * - \ref os_statistics_get_task_switch_time
 * - \ref os_statistics_get_task_switch_time_jitter
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH
	#define CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH false
#endif

/* Macros *********************************************************************/

#if CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH == true
	/*! \brief Hook used to get the time information at the entrance of
	 * a context task switch (tick interrupt handler).
	 * \param offset_cy The number of cycles between the interrupt call and
	 * the call of
	 * \ref __HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_START
	 */
	#define __HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_START(offset_cy) \
		do { \
			__os_statistics_switch_context_tick_handler_start(offset_cy); \
		} while (false)
	/*! \brief Hook used to get the time information at the exit of a
	 * context task switch (tick interrupt handler).
	 * \param offset_cy The number of cycles between the call of
	 * \ref __HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_STOP and the
	 * exit of the interrupt handler.
	 */
	#define __HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_STOP(offset_cy) \
		do { \
			__os_statistics_switch_context_tick_handler_stop(offset_cy); \
		} while (false)
	/*! \brief Hook used to get the time information at the entrance of
	 * a context task switch (when using \ref os_yield).
	 * \param offset_cy The number of cycles between the function call and
	 * the call of \ref __HOOK_OS_STATISTICS_SWITCH_CONTEXT_START
	 */
	#define __HOOK_OS_STATISTICS_SWITCH_CONTEXT_START(offset_cy) \
		do { \
			__os_statistics_switch_context_start(offset_cy); \
		} while (false)
	/*! \brief Hook used to get the time information at the exit of a
	 * context task switch (when using \ref os_yield).
	 * \param offset_cy The number of cycles between the call of
	 * \ref __HOOK_OS_STATISTICS_SWITCH_CONTEXT_STOP and the
	 * exit of the function.
	 */
	#define __HOOK_OS_STATISTICS_SWITCH_CONTEXT_STOP(offset_cy) \
		do { \
			__os_statistics_switch_context_stop(offset_cy); \
		} while (false)
#else
	#define __HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_START(offset_cy)
	#define __HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_STOP(offset_cy)
	#define __HOOK_OS_STATISTICS_SWITCH_CONTEXT_START(offset_cy)
	#define __HOOK_OS_STATISTICS_SWITCH_CONTEXT_STOP(offset_cy)
#endif

/* Internal API ***************************************************************/

#if CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH == true
	/*! \copydoc __HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_START
	 */
	void __os_statistics_switch_context_tick_handler_start(os_cy_t offset_cy);
	/*! \copydoc __HOOK_OS_STATISTICS_SWITCH_CONTEXT_TICK_HANDLER_STOP
	 */
	void __os_statistics_switch_context_tick_handler_stop(os_cy_t offset_cy);
	/*! \copydoc __HOOK_OS_STATISTICS_SWITCH_CONTEXT_START
	 */
	void __os_statistics_switch_context_start(os_cy_t offset_cy);
	/*! \copydoc __HOOK_OS_STATISTICS_SWITCH_CONTEXT_STOP
	 */
	void __os_statistics_switch_context_stop(os_cy_t offset_cy);
#endif

/* Public API *****************************************************************/

/*! \name Statistics
 *
 * This set of function gives real-time statistics on the current operating
 * system setup.
 *
 * \{
 */

/*!
 * \brief Get the average time of the context task switching in number of cycles
 * \ingroup group_os_public_api
 * \return the number of cycles of the average context switching time
 * \pre \ref CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH must be set
 */
os_cy_t os_statistics_get_task_switch_time(void);

/*!
 * \brief Get the jitter of the context task switching in number of cycles
 * \ingroup group_os_public_api
 * \return the number of cycles of the switching time jitter
 * \pre \ref CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH must be set
 */
os_cy_t os_statistics_get_task_switch_time_jitter(void);

/*!
 * \brief Theoretical estimation of the CPU load of a task.
 * It is based on the number of current active tasks in the list and their
 * priority.
 * The ratio of the CPU ressources is calculated as follow:
 * \code ratio = (100 / (priority level)) / SUM(100 / (each priority level)) \endcode
 * \ingroup group_os_public_api
 * \param task The task to evaluate
 * \return The allocation time in percent of the CPU assigned to this task
 */
uint8_t os_statistics_task_cpu_allocation(struct os_task *task);

/*!
 * \}
 */

void os_statistics_monitor_ram(void);

#endif // __OS_STATISTICS_H__
