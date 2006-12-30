/** \file sst_base_def.h
 *  \brief A header file specific for given microcontroller and project.
 *
 * It is used for porting SST to other microcontrollers and to tune
 * SST paramaeters for a given project (e.g. maximum number of tasks).
 */


/** \def SST_USE_SIGNAL_MASK
 *  \brief This macro conditionally enables or disables event broadcast functionality.
 *
 * If this macro is defined to nonzero value then code is compiled with additional
 * functionality which allows to define signal mask attribute for each task
 * (#SST_SetSignalMask) and to post event to more than one task by single function
 * (#SST_PostEvent) call (broadcast like).<br>
 * The choice if given task should receive event is made basing on the value of
 * task attribute called signal mask. If signal part of event matches
 * event mask of given task then event is posted to this task. Matching is
 * implemented by performing simple bitwise \c AND operation e.g. if signal
 * has value \c 0x31 and evet mask is \c 0x10 then event matches.
 */
#define SST_USE_SIGNAL_MASK 1


/** \def SST_USE_LAZY_TIMER
 *  \brief This macro conditionally enables "lazy" timer functionality.
 *
 * If this macro is defined to nonzero value then code is compiled with additional
 * functionality which allows to define (trough #SST_TICKER_DEFINE) callback functions
 * associated with ticker (#SST_Ticker_T). Those functions will be called in two cases:
 *    - when all timed tasks associated with given ticker are disabled - ticker becomes idle
 *    - when any of timed task has been enabled and ticker was previously idle
 *
 * This functionality allows to automatically disable/enable HW timer and/or
 * associated ISR only when needed.
 */
#define SST_USE_LAZY_TIMER 1


/** \def SST_MAX_NO_OF_TASKS
 *  \brief This macro sets maximum number of tasks in the system.
 *
 * The space needed for task information is allocated fully staticaly
 * and it is impossible to extend (change) it at runtime.
 */
#define SST_MAX_NO_OF_TASKS 5


/** \def SST_MAX_EV_QUEUE_LEN
 *  \brief This macro sets a events queue depth for task.
 *
 * Depending on the value of this macro there can be three different cases:
 *    - \c SST_MAX_EV_QUEUE_LEN is 0 - the events queue (and its depth) is
 *       separately defined for each task, there should be used special API
 *       for defining queue - #SST_EVENTQ_DEFINE and attaching it to
 *       given task (#SST_CreateTask)
 *    - \c SST_MAX_EV_QUEUE_LEN is 1 - in this case there is generated
 *       the simplest (and shortest) code, for each task only one event
 *       can be placed in a slot and in fact, there is no real queueing
 *    - \c SST_MAX_EV_QUEUE_LEN is greather than 1 - in this case for each
 *       task there is allocated the same space (\c SST_MAX_EV_QUEUE_LEN)
 *       for events queue<br>
 *
 * The space needed for queue is allocated fully statically
 * and it is impossible to extend (change) it at runtime.
 */
#define SST_MAX_EV_QUEUE_LEN 0


/** \def SST_MAX_TASK_PRIO
 *  \brief This macro sets upper limit for tasks priority value.
 *
 * \warning This value should be less than maximum possible value of
 * the #CPU_Base_T type.<br>
 * Preferably it should be defined in the middle of range of
 * the #CPU_Base_T type.
 */
#define SST_MAX_TASK_PRIO


/** \typedef CPU_Base_T
 *  \brief The datatype defined by user used as a basic type by the SST.
 *
 * This should be the most effective one (in terms of speed) for given CPU.<br>
 * It is used in many places in the SST code as a basic integer type - mainly
 * for indexing and counting purposes.
 */
typedef uint32_t CPU_Base_T;


/** \typedef CPU_SR_T
 *  \brief The datatype defined by user and used to keep status of global
 * interrupt enable flag.
 *
 * It is used by the SST API like #SST_INT_LOCK_AND_SAVE to keep state of interrupts.
 */
typedef uint32_t CPU_SR_T;


/** \def SST_Start_Nested_Ints
 *  \brief A macro defined by user to enable interrupts nesting.
 *
 * Usually this macro deals with interrupt controller (if any) and enables
 * interrupts globally (if they are automatically diabled at the entry to ISR).<br>
 * It is used by the #SST_ISR_ENTRY
 */
#define SST_Start_Nested_Ints()


/** \def SST_Stop_Nested_Ints
 *  \brief A macro defined by user to stop interrupts nesting.
 *
 * Usually this macro disables interrupts globally.<br>
 * It is used by the #SST_ISR_EXIT
 */
#define SST_Stop_Nested_Ints()


/** \def SST_disable_ints_save
 *  \brief A macro defined by user whis save state of global interrupt enable flag then
 * disable interrupts.
 *
 * It is used by the #SST_INT_LOCK_AND_SAVE
 */
#define SST_disable_ints_save(_old_)


/** \def SST_disable_ints
 *  \brief A macro defined by user to disable interrupts (globally).
 *
 * It is used by the #SST_INT_LOCK
 */
#define SST_disable_ints()


/** \def SST_enable_ints
 *  \brief A macro defined by user to anable interrupts (globally).
 *
 * It is used by the #SST_INT_UNLOCK
 */
#define SST_enable_ints()


/** \def SST_restore_ints
 *  \brief A macro defined by user to restore interrupts status depending on value
 * of the \a _old_ parameter.
 *
  * It is used by the #SST_INT_LOCK_RESTORE
 */
#define SST_restore_ints(_old_)
