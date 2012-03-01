/*! \file
 * \brief eeOS Core
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

#ifndef __OS_CORE_H__
#define __OS_CORE_H__

/*! \defgroup group_os eeOS
 * \brief eeOS is a Embedded Event-driven Operating System.
 * This page contains all the documentation related to this operating
 * system (\ref OS_VERSION).
 * - Features:
 *   - Real-Time OS
 *   - Preemptive and/or cooperative round-robin multi-tasking
 *   - Very scalable
 *   - Task priority
 *   - Hook points
 *   - Software interrupt with priority
 *   - Doxygen documented
 *   - Advanced event system with priority
 *   - Semaphores with priority inheritance
 * - Targets:
 *   - MISRA-C 2004 (MISRA-C 2) compatible
 *
 * \section section_os_process Process
 *
 * A process (\ref os_process) is an instance of a piece of code with its own
 * context. A process can have a priority execution amongst other processes.
 * It can be enabled and be part of the active process list or disabled and
 * be completely removed from the \ref section_os_scheduler.
 *
 * A process is a private entity and must not be used as is.
 *
 * \ref section_os_process_task and \ref section_os_process_interrupt are
 * sub-categories of processses. Therefore they will inherit from all processes
 * features. These entities are public entities and should be used by the user.
 *
 * \section section_os_application Application Process
 *
 * The application process contains the initial context. During the execution
 * of the operating system, it is used only when no other processes are
 * running and can be controlled with \ref HOOK_OS_IDLE.
 *
 * \section section_os_scheduler Process Scheduler
 *
 * All the actives processes are stored in a circular chain list. The operating
 * system does not keep track of the non-active processes. This to keep the
 * context of this OS as small as possible.
 *
 * The first process to be present in the active process list is the application
 * process. This process will keep track of the intial context of
 * the application before the user starts the task scheduling (\ref os_start).
 * At this point the current process list will look like this:
 * \code (application context) -> (application context) -> ... \endcode
 *
 * When the users creates and enables a new process, it will be added to the
 * active process list.
 * \code (process 1) -> (process 1) -> (process 1) -> ... \endcode
 * 2 new processed will make the active process list look like this:
 * \code (process 1) -> (process 2) -> (process 1) -> ... \endcode
 *
 * After each ticks the scheduler will switch between the different processes
 * in the list. If all the processes share the same priority level, the
 * time in each processes will be equal. A process with higher priority will
 * receive more attention from the scheduler.
 *
 * \warning The priority level is assigned by \ref os_priority. The lower number
 * represents the higher priority.
 *
 * If the level of a process is X, it means that the process will be executed
 * every X iteration. An iteration represents a complete cycle of the circular
 * active process list.
 * For example, if there are only 2 processes in the active process list;
  Let's say process #1 has a priority level of 1 and process #2 has a priority
 * level of 2, then process #1 will have 66% of CPU while process #2, 33%.
 * The active process list can be seen as follow:
 * \code (process 1) -> (process 1) -> (process 2) -> (process 1) -> (process 1)
 * -> (process 2) -> ... \endcode
 *
 * \section section_os_usage Usage
 *
 * The operating system will be operational after the call of \ref os_start.
 * This function needs a frequency which is passed as an argument. This
 * frequency is used as a reference to generate the tick interrupt at
 * \ref CONFIG_OS_TICK_HZ.
 *
 * To calculate how much ressources are needed, you need to understand what your
 * application really needs. The size of the application stack also needs to be
 * carefully chosen. The application stack is used by different modules in the
 * operating system:
 * - \ref section_os_process_interrupt will runs within the application context.
 * - The \ref section_os_event scheduler will run within the application
 * context.
 * - Everything you put in \ref HOOK_OS_IDLE will also run within the
 * application context.
 */

/*! \defgroup group_os_public_api Public API
 * \brief Public application interface.
 * \ingroup group_os
 */

/*! \defgroup group_os_internal_api Internal API
 * \brief Internal API. These functions should not be used by the user.
 * \ingroup group_os
 */

/*! \defgroup os_port_group Porting functions
 * \brief Functions which should be implemented to port this operating system
 * onto another platform.
 * \ingroup group_os
 */

/*! \defgroup group_os_config Configuration
 * \brief Configuration flags available to customize the behavior of the OS.
 * \ingroup group_os
 */

/*! \defgroup os_scheduler_type Scheduler Type
 * \brief Configuration values for \ref CONFIG_OS_SCHEDULER_TYPE
 * \ingroup group_os_config
 */

/*! \defgroup os_hook Hooks
 * \brief Allow the user to insert software hooks
 *
 * All the hooks are expected to be macros and should follow the following
 * assignement:
 * \code #define HOOK_OS_EXAMPLE() my_hook() \endcode
 * \ingroup group_os
 */

/* Metadata *******************************************************************/

/*! \brief Current version of the operating system.
 */
#define OS_VERSION "0.1"

/* Configuration **************************************************************/

#include "os_port.h"
#include "conf_os.h"

/* Configuration options ******************************************************/

/*! \name Core OS
 *
 * \{
 */

/*! \def CONFIG_OS_USE_TICK_COUNTER
 * \brief This configuration will enable the tick counter.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_USE_TICK_COUNTER
	#define CONFIG_OS_USE_TICK_COUNTER true
#endif

/*! \def CONFIG_OS_USE_16BIT_TICKS
 * \brief Use a 16-bits variable to define the tick counter.
 * \ingroup group_os_config
 * \pre \ref CONFIG_OS_USE_TICK_COUNTER needs to be defined first.
 */
#ifndef CONFIG_OS_USE_16BIT_TICKS
	#define CONFIG_OS_USE_16BIT_TICKS false
#endif

/*! \def CONFIG_OS_SCHEDULER_TYPE
 * \brief Defines the type of scheduler to be used. Values can be found here
 * (\ref os_scheduler_type).
 *
 * This operating system can use cooperative and pre-emptive tasks together.
 * To allow \b only cooperative tasks, set the value to
 * \ref CONFIG_OS_SCHEDULER_COOPERATIVE.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_SCHEDULER_TYPE
	#define CONFIG_OS_SCHEDULER_TYPE CONFIG_OS_SCHEDULER_COOPERATIVE
#endif
#if CONFIG_OS_SCHEDULER_TYPE == CONFIG_OS_SCHEDULER_COOPERATIVE && \
		CONFIG_OS_USE_TICK_COUNTER == true
	#error The tick counter cannot be used when only cooperative scheduler\
			is used. CONFIG_OS_SCHEDULER_TYPE must be set to\
			something else than CONFIG_OS_SCHEDULER_COOPERATIVE.
#endif

/*!
 * \brief Use this option to use the OS in \b cooperative \b mode \b only.
 * \ingroup os_scheduler_type
 */
#define CONFIG_OS_SCHEDULER_COOPERATIVE 0
#ifndef CONFIG_OS_SCHEDULER_TYPE
	#error CONFIG_OS_SCHEDULER_TYPE must be set. It defines the type of \
			scheduler to use.
#endif

/*! \def CONFIG_OS_TICK_HZ
 * \brief Set the tick frequency in Hz. This configuration is not used if
 * \ref CONFIG_OS_SCHEDULER_COOPERATIVE is chosen.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_TICK_HZ
	#define CONFIG_OS_TICK_HZ 1000
#endif

/*! \def CONFIG_OS_USE_PRIORITY
 * \brief Set this config to \b true in order to enable task priority support.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_USE_PRIORITY
	#define CONFIG_OS_USE_PRIORITY true
#endif

/*! \def CONFIG_OS_DEBUG
 * \brief Set this config to \b true to activate the \ref group_os_debug.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_DEBUG
	#define CONFIG_OS_DEBUG false
#endif

/*! \def CONFIG_OS_USE_MALLOC
 * \brief Use \ref os_malloc and \ref os_free to allocate/free the memory
 * for the stack
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_USE_MALLOC
	#define CONFIG_OS_USE_MALLOC true
#endif

/*! \def CONFIG_OS_USE_CUSTOM_MALLOC
 * \brief Use a custom memory allocator. The user needs to define \ref os_malloc
 * and \ref os_free to allocate/free the memory
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_USE_CUSTOM_MALLOC
	#define CONFIG_OS_USE_CUSTOM_MALLOC false
#endif

/*! \def CONFIG_OS_PROCESS_ENABLE_FIFO
 * \brief Setting this define to true will bring enabled process at the end of
 * the active process list. The last process enabled will be the last called by
 * the scheduler.
 * For example, if you enable process_3 to the active process list.\n
 * if \ref CONFIG_OS_PROCESS_ENABLE_FIFO == true
 * \code process_1 -> process_2 -> process_3 -> process_1 -> ... \endcode
 * if \ref CONFIG_OS_PROCESS_ENABLE_FIFO == false
 * \code process_3 -> process_1 -> process_2 -> process_3 -> ... \endcode
 * \note Activating this define impacts a bit the CPU and memory ressources when
 * enabling a new process.
 * \ingroup group_os_config
 */
#ifndef CONFIG_OS_PROCESS_ENABLE_FIFO
	#define CONFIG_OS_PROCESS_ENABLE_FIFO false
#endif

/*!
 * \}
 */

/* Hooks **********************************************************************/

/*! \name OS Core
 *
 * \{
 */

/*! \def HOOK_OS_TICK
 * \brief This hook is called during each tick interrupt.
 * \ingroup os_hook
 */
#ifndef HOOK_OS_TICK
	#define HOOK_OS_TICK()
#endif

/*! \def HOOK_OS_IDLE
 * \brief This hook is called when no task is running and the application runs
 * in the IDLE loop.
 * \ingroup os_hook
 */
#ifndef HOOK_OS_IDLE
	#define HOOK_OS_IDLE()
#endif

/*!
 * \}
 */

/* Macros *********************************************************************/

/*! \brief Retrieve the pointer of the containing structure of the ptr element.
 * \ingroup group_os_public_api
 * \param ptr The ptr located inside the structure from which we want to
 * retreive the address.
 * \param type The type of the parent structure.
 * \param member The member symbol of the parent structure.
 * \return The address of the parent structure.
 */
#define OS_CONTAINER_OF(ptr, type, member) \
		((type *)((uint8_t *)(ptr) - offsetof(type, member)))

/*! \brief Align a pointer. The alignement is made forward compared to the
 * initial pointer.
 * \ingroup group_os_public_api
 * \param ptr The ptr to be aligned.
 * \return The pointer aligned.
 */
#define OS_ALIGN(ptr) \
		((os_ptr_t) ((((os_intptr_t) (ptr)) + OS_COMPILER_ALIGN - 1) & \
		~(OS_COMPILER_ALIGN - 1)))

/*! \brief Align a pointer. The alignement is made backward compared to the
 * initial pointer.
 * \ingroup group_os_public_api
 * \param ptr The ptr to be aligned.
 * \return The pointer aligned.
 */
#define OS_ALIGN_BACK(ptr) \
		((os_ptr_t) ((((os_intptr_t) (ptr))) & \
		~(OS_COMPILER_ALIGN - 1)))

/*! \brief Calculate the number of argument passed in argument to this macro.
 * \ingroup group_os_public_api
 * \param ... Arguments to be monitored
 * \return The number of arguments passed to this macro
 */
#define OS_NB_ARGS(...) \
		(sizeof((os_ptr_t []) {__VA_ARGS__}) / sizeof(os_ptr_t))

/*! \brief Convert a delay in milliseconds to a number of ticks.
 * \ingroup group_os_public_api
 * \param time_ms The time (in ms) to be converted
 * \return The number of ticks
 */
#define OS_MS_TO_TICKS(time_ms) \
		(((time_ms) * CONFIG_OS_TICK_HZ) / 1000)

/*! \brief Convert a delay in seconds to a number of ticks.
 * \ingroup group_os_public_api
 * \param time_s The time (in s) to be converted
 * \return The number of ticks
 */
#define OS_S_TO_TICK(time_s) \
		((time_s) * CONFIG_OS_TICK_HZ)

/* Types **********************************************************************/

#if CONFIG_OS_USE_PRIORITY == true
/*! \brief Priority values for a process.
 * The lower get the most priority.
 */
enum os_priority {
	/*! \brief Highest priority */
	OS_PRIORITY_1 = 0,
	/*! \brief Priority level of 2nd order */
	OS_PRIORITY_2 = 1,
	/*! \brief Priority level of 3rd order */
	OS_PRIORITY_3 = 2,
	/*! \brief Priority level of 4th order */
	OS_PRIORITY_4 = 3,
	/*! \brief Priority level of 5th order */
	OS_PRIORITY_5 = 4,
	/*! \brief Priority level of 10th order */
	OS_PRIORITY_10 = 9,
	/*! \brief Lowest priority */
	OS_PRIORITY_20 = 19,
};
#endif

/*! \brief Type to define a number of ticks
 */
#if CONFIG_OS_USE_16BIT_TICKS == true
/*! Use a 16-bit variable to store the tick numbers */
typedef uint16_t os_tick_t;
#else
/*! Use a 32-bit variable to store the tick numbers */
typedef uint32_t os_tick_t;
#endif

/*! \brief Process type
 */
enum os_process_type {
	/*! \brief Application process
	 */
	OS_PROCESS_TYPE_APPLICATION = 0,
	/*! \brief Task (\ref section_os_process_task)
	 */
	OS_PROCESS_TYPE_TASK = 1,
	/*! \brief Software interrupt (\ref section_os_process_interrupt)
	 */
	OS_PROCESS_TYPE_INTERRUPT = 2,
	/*! \brief Event scheduler
	 */
	OS_PROCESS_TYPE_EVENT = 3,
};

/*! \brief Status of the process
 */
enum os_process_status {
	/*! \brief The process is in idle state. It is not active nor pending,
	 * it is simply disabled and needs a manual action to bring it up.
	 */
	OS_PROCESS_IDLE = 0,
	/*! \brief The process is active. It is stored in the active process
	 * list and will be proceed by the scheduler.
	 */
	OS_PROCESS_ACTIVE = 1,
	/*! \brief The process is in pending mode. It is currently disabled,
	 * it does not appear in the active process list but is waiting for an
	 * event to trigger its re-activation.
	 */
	OS_PROCESS_PENDING = 2,
};

/*! \brief This structure represents a process context
 */
struct os_process {
	/*! \brief Stack pointer. Will always be the 1rst element of this
	 * structure to ensure the best optimization.
	 */
	os_ptr_t sp;
	/*! \brief Pointer of the next process in the list.
	 * Active processes are registered within a chain list.
	 */
	struct os_process *next;
	/*! \brief Indicates if the process is active or not.
	 * Values are part of \ref os_process_status
	 */
	uint8_t status;
	/*! \brief The type of the process
	 * Values are part of \ref os_process_type
	 */
	uint8_t type;
#if CONFIG_OS_USE_PRIORITY == true
	/*! \brief Priority of the process.
	 * Values are part of \ref os_priority
	 */
	uint8_t priority;
	/*! \brief Use to manage the process priorities
	 * Values are part of \ref os_priority
	 */
	uint8_t priority_counter;
#endif
#if CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH == true
	/*! \brief Internal cycle counter increased during the activity of the
	 * task. It is used for statistics only. Its value is increased only
	 * during context switches.
	 */
	os_cy_t cycle_counter;
#endif
};

/*! \brief Process function prototype
 * \param args Arguments passed to the process
 */
typedef void (*os_proc_ptr_t)(os_ptr_t args);

/* Porting functions **********************************************************/

/*!
 * \fn os_is_critical
 * \brief Indicates if the CPU is currently running inside a critical region.
 * \ingroup os_port_group
 * \return true if the CPU is running inside a critical region, false otherwise.
 */

/*!
 * \fn os_enter_critical
 * \brief Start of a critical code region. Preemptive context switches cannot
 * occur when in a critical region.\n
 * \ingroup os_port_group
 */

/*!
 * \fn os_leave_critical
 * \brief Exit a critical code region.\n
 * \ingroup os_port_group
 */

/*!
 * \brief Setup the task scheduler interrupt.
 * \param ref_hz The reference frequency used to clock the peripheral which will
 * generate the tick interrupt. Usually this frequency is equal to the CPU
 * frequency.
 * \ingroup os_port_group
 */
void os_setup_scheduler(uint32_t ref_hz);

/*!
 * \fn os_switch_context(bypass_context_saving)
 * \brief Context switch for a process.\n
 * Function used to schedule and switch between the processes.\n
 * This function will handle the return from a softwre interrupt. Therefore it
 * can be optimized to bypass the saving context part IF an interrupt is
 * running.
 * \ingroup os_port_group
 * \param bypass_context_saving If true, this function needs to bypass the
 * context saving.
 */

/*!
 * \fn os_switch_context_int_handler
 * \brief Context switch for a process.\n
 * Interrupt handler which is used to schedule and switch between the processes.
 * \ingroup os_port_group
 */

/*!
 * \brief Load the context of a task into the stack. this is the inital process
 * which will setup the stack before entering in the task function.
 * \ingroup os_port_group
 * \param proc The task
 * \param proc_ptr Pointer of the entry point of the process
 * \param args Parameters to pass to the task
 * \return true in case of success, false otherwise
 */
bool os_process_context_load(struct os_process *proc, os_proc_ptr_t proc_ptr,
		os_ptr_t args);

/*!
 * \fn os_ptr_t os_malloc(int stack_size)
 * \brief Allocate some memory for the stack of a task
 * \ingroup os_port_group
 * \param stack_size The size in byte of the stack
 * \return The pointer of the memory allocated, NULL in case of an error.
 */
#if CONFIG_OS_USE_CUSTOM_MALLOC == false
static inline os_ptr_t os_malloc(int stack_size) {
	return (os_ptr_t) malloc(stack_size);
}
#endif

/*!
 * \fn void os_free(os_ptr_t ptr)
 * \brief Free memory previously allocated by \ref os_malloc
 * \ingroup os_port_group
 * \param ptr The memory to free
 */
#if CONFIG_OS_USE_CUSTOM_MALLOC == false
static inline void os_free(os_ptr_t ptr) {
	free((void *) ptr);
}
#endif

/* Internal API ***************************************************************/

/*! \brief Get the current process
 * \ingroup group_os_internal_api
 * \return A pointer on the current procress
 */
static inline struct os_process *__os_process_get_current(void) {
	extern struct os_process *__os_current_process;
	return __os_current_process;
}

/*! \brief Check if a process is the application process
 * \ingroup group_os_internal_api
 * \param proc The process to be checked
 * \return true if this is the application process, false otherwise
 */
static inline bool __os_process_is_application(struct os_process *proc) {
	return (proc->type == OS_PROCESS_TYPE_APPLICATION);
}

/*! \brief Check if a process is a task
 * \ingroup group_os_internal_api
 * \param proc The process to be checked
 * \return true if this is a task, false otherwise
 */
static inline bool __os_process_is_task(struct os_process *proc) {
	return (proc->type == OS_PROCESS_TYPE_TASK);
}

/*! \brief Check if a process is a software interrupt
 * \ingroup group_os_internal_api
 * \param proc The process to be checked
 * \return true if this is a software interrupt, false otherwise
 */
static inline bool __os_process_is_interrupt(struct os_process *proc) {
#if CONFIG_OS_USE_SW_INTERRUPTS == true
	return (proc->type == OS_PROCESS_TYPE_INTERRUPT);
#else
	return false;
#endif
}

/*! \brief Check if a process is the event scheduler
 * \ingroup group_os_internal_api
 * \param proc The process to be checked
 * \return true if this is the event scheduler, false otherwise
 */
static inline bool __os_process_is_event(struct os_process *proc) {
#if CONFIG_OS_USE_EVENTS == true
	return (proc->type == OS_PROCESS_TYPE_EVENT);
#else
	return false;
#endif
}

/*! \brief Enable the execution a process
 * \ingroup group_os_internal_api
 * \param proc The process to be enabled
 */
void __os_process_enable(struct os_process *proc);

/*! \brief Disable the execution of a process
 * \ingroup group_os_internal_api
 * \param proc The process to be disabled
 */
void __os_process_disable(struct os_process *proc);

/*! \brief Check wether a process is active or not
 * \ingroup group_os_internal_api
 * \param proc The process to be checked
 * \return true if enabled, false otherwise
 */
static inline bool __os_process_is_enabled(struct os_process *proc) {
	return (proc->status == OS_PROCESS_ACTIVE);
}

/*! \brief Check wether a process is enabled or not
 * \ingroup group_os_internal_api
 * \param proc The process to be checked
 * \return true if enabled, false otherwise
 */
static inline bool __os_process_is_pending(struct os_process *proc) {
	return (proc->status == OS_PROCESS_PENDING);
}

#if CONFIG_OS_USE_PRIORITY == true
/*! \brief Change the priority of a process
 * \ingroup group_os_internal_api
 * \param proc The process which needs some update
 * \param priority The new priority
 * \pre \ref CONFIG_OS_USE_PRIORITY needs to be set first
 */
static inline void __os_process_set_priority(struct os_process *proc,
		enum os_priority priority) {
	/* Not critical so no need to use the os_enter_critical function */
	proc->priority = priority;
	proc->priority_counter = priority;
}
/*! \brief Get the priority of a process
 * \ingroup group_os_internal_api
 * \param proc The process which priority is requested
 * \return The process priority
 * \pre \ref CONFIG_OS_USE_PRIORITY needs to be set first
 */
static inline enum os_priority __os_process_get_priority(struct os_process *proc) {
	return (enum os_priority) proc->priority_counter;
}
#endif

/*! \brief This function will define the rules to change the task.
 * \ingroup group_os_internal_api
 * \return The new task context
 */
struct os_process *__os_scheduler(void);

/*! \brief This function must be called inside the
 * \ingroup group_os_internal_api
 * \ref os_switch_context function in order to switch process context.
 * \return The context of the new process
 */
static inline struct os_process *__os_switch_context_hook(void) {
#ifdef OS_SCHEDULER_POST_INTERRUPT_HOOK
	/* Clear the software interrupt if needed */
	OS_SCHEDULER_POST_INTERRUPT_HOOK();
#endif
#ifdef OS_SCHEDULER_POST_EVENT_HOOK
	/* Use the alternate task if any */
	OS_SCHEDULER_POST_EVENT_HOOK();
#endif
	/* Task switch context */
	return __os_scheduler();
}

/*! \brief Initializes a process
 * \ingroup group_os_internal_api
 * \param proc The process to be initialized
 * \param sp The stack pointer
 * \param type The type of process
 */
static inline void __os_process_create(struct os_process *proc, os_ptr_t sp,
		enum os_process_type type) {
	/* Align the stack pointer within the stack. */
	proc->sp = OS_ALIGN_BACK(sp);
	/* Set the type of the process (task, interrupt, ...) */
	proc->type = type;
	/* Set the status of the process. Initially it is set to idle. */
	proc->status = OS_PROCESS_IDLE;
#if CONFIG_OS_STATISTICS_MONITOR_TASK_SWITCH == true
	/* Update the activity cycle counter of this task. */
	proc->cycle_counter = 0;
#endif
}

/*! \brief Get the application process
 * \ingroup group_os_internal_api
 * \return the application process
 */
static inline struct os_process *__os_process_get_application(void) {
	extern struct os_process os_app;
	return &os_app;
}
/*! \brief Enable the application process
 * \ingroup group_os_internal_api
 */
static inline void __os_process_application_enable(void) {
	extern struct os_process os_app;
	__os_process_enable(&os_app);
	os_app.type = OS_PROCESS_TYPE_APPLICATION;
}
/*! \brief Disable the application process
 * \ingroup group_os_internal_api
 */
static inline void __os_process_application_disable(void) {
	extern struct os_process os_app;
	if (__os_process_is_application(&os_app)) {
		__os_process_disable(&os_app);
	}
}
/*! \brief Enable the event process
 * If the event process is enabled, the application process will be disabled
 * as they share the same process.
 * \ingroup group_os_internal_api
 */
static inline void __os_process_event_enable(void) {
	extern struct os_process os_app;
	__os_process_application_enable();
	os_app.type = OS_PROCESS_TYPE_EVENT;
}
/*! \brief Enable the event process
 * If the event process is enabled, the application process will be disabled
 * as they share the same process.
 * \ingroup group_os_internal_api
 */
static inline void __os_process_event_disable(void) {
	extern struct os_process os_app;
	if (__os_process_is_event(&os_app)) {
		__os_process_disable(&os_app);
	}
}

/*! \copydoc __os_process_enable
 * This function will push the task at the end of the chain list
 * \warning This function does not check if the task is already added to the
 * list and should be used inside a critical area.
 * \ingroup group_os_internal_api
 */
void __os_process_enable_naked(struct os_process *proc);
/*! \copydoc __os_process_disable
 * \warning This function does not check if the task is already disabled and
 * should be used inside a critical area. It also does not stop after the
 * execution of this function.
 * \ingroup group_os_internal_api
 */
void __os_process_disable_naked(struct os_process *proc);

/* Includes (which will impact the core) **************************************/

#include "os_debug.h"
#include "os_queue.h"
#include "os_event.h"
#include "os_task.h"

/* Internal API (impacted by the previous modules) ****************************/

/*!
 * \brief This function must be called inside the
 * \ref os_switch_context_int_handler function in order to switch task
 * context.
 * \return The context of the new process
 */
static inline struct os_process *__os_switch_context_int_handler_hook(void) {
#if CONFIG_OS_USE_TICK_COUNTER == true
	extern volatile os_tick_t os_tick_counter;
	/* Update the tick counter */
	os_tick_counter++;
#endif
#if CONFIG_OS_DEBUG == true
	__HOOK_OS_DEBUG_TICK();
#endif
	HOOK_OS_TICK();
	/* Task switch context */
	return __os_scheduler();
}

/* Public API *****************************************************************/

/*! \name Kernel Control
 *
 * Control the core of the operating system
 *
 * \{
 */

/*! \brief Call the scheduler to switch to a new task that is ready to run.
 * This function is useful for cooperative task swiching
 * \ingroup group_os_public_api
 */
void os_yield(void);

/*! \brief Start the task scheduling process
 * \ingroup group_os_public_api
 * \param ref_hz The frequency which runs the peripheral to generate
 * the ticks. Usually this frequency is equal to the CPU frequency.
 */
static inline void os_start(uint32_t ref_hz) {
	extern void os_setup_scheduler(uint32_t);
#if CONFIG_OS_SCHEDULER_TYPE != CONFIG_OS_SCHEDULER_COOPERATIVE
	/* Setup the scheduler */
	os_setup_scheduler(ref_hz);
#endif
	/* Launch the scheduler */
	os_yield();
	/* Idle loop. This loop will control the application process but also
	 * the event process.
	 * They both run inside an infinite loop. They cannot run at the same
	 * time.
	 */
	while (true) {
		/* If the event process is running, call the event scheduler */
		if (__os_process_is_event(__os_process_get_current())) {
			__os_event_scheduler();
		}
		/* Else it means the application process is running. In other
		 * word, not other processes are actives or pending for events.
		 */
		else {
			HOOK_OS_IDLE();
		}
	}
}

/*! \brief Get the current version of the running operating system
 * \return A string containing the version of the OS.
 */
static inline char *os_get_version(void) {
	return OS_VERSION;
}

/*!
 * \}
 */

/* Includes *******************************************************************/

#include "os_interrupt.h"
#include "os_semaphore.h"
#include "os_mutex.h"
#include "os_statistics.h"

#endif // __OS_CORE_H__
