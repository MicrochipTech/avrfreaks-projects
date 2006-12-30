#ifndef SST_INTERN_H_
#define SST_INTERN_H_

/*******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *******************************************************************************/

/** \file sst_intern.h
 *  \brief A header file with definitions used by teh SST implementation.
 *
 * It is used for some user macros but mainly for internal purposes of the
 * SST and should \b not be direcly included by user code.
 */


#include <sst_event.h>

#define SST_IDLE_TASK_PRIO          ((CPU_Base_T) 0U)

/** \def SST_MIN_INT_PRIO
 *  \brief The lowest priority for an ISR.
 *
 * ISRs should have priority equal to or greather than value of this macro.
 * It is achieved by using it as a \a isr_prio parameter
 * for the #SST_ISR_ENTRY macro.
 */
#define SST_MIN_INT_PRIO            ((CPU_Base_T) ((SST_MAX_TASK_PRIO) + 1U))

/** \def SST_TASK_BROADCAST_ID
 *  \brief A special task ID used as a broadcast ID (e.g. #SST_PostEvent).
 *
 * Value of this macro can be used to inform #SST_PostEvent that it should
 * post events to group of tasks with matching signal mask.
 */
#define SST_TASK_BROADCAST_ID       SST_BAD_TASK_ID

/** \def SST_BAD_TASK_ID
 *  \brief A bad task ID used to communicate problems (e.g. #SST_CreateTask).
 *
 * Value of this macro can be used to test return value of the #SST_CreateTask
 * function.
 */
#define SST_BAD_TASK_ID             ((SST_TaskID_T) SST_MAX_NO_OF_TASKS)

#define SST_INT_STATE               _int_state_

/** \def SST_DECLARE_INT_USAGE
 *  \brief This macro declares variables needed to keep interrupt state.
 *
 * The macros #SST_INT_LOCK_AND_SAVE and #SST_INT_LOCK_RESTORE needs
 * some memory keep state of interrupts. This macro perform all needed definitions.
 */
#define SST_DECLARE_INT_USAGE       CPU_SR_T SST_INT_STATE


/** \def SST_DECLARE_ISR
 *  \brief This macro is used to declare that given ISR will use SST interrupt
 * API like # SST_ISR_ENTRY  and #SST_ISR_EXIT.
 *
 * It declares variables used used internally by SST for keeping
 * interrupt state and task priority.
 */
#define SST_DECLARE_ISR             \
    CPU_Base_T _pin_;               \
    CPU_SR_T _int_state_;


/** \def SST_INT_LOCK
 *  \brief This macro unconditionally disables interrupts.
 */
#define SST_INT_LOCK()              do { SST_disable_ints(); } while (0)


/** \def SST_INT_UNLOCK
 *  \brief This macro unconditionally enables interrupts.
 */
#define SST_INT_UNLOCK()            do { SST_enable_ints(); } while (0)


/** \def SST_INT_LOCK_AND_SAVE
 *  \brief This macro saves global interrupt enable flag and unconditionally
 * disables interrupts.
 */
#define SST_INT_LOCK_AND_SAVE()     do { SST_disable_ints_save(_int_state_); } while (0)


/** \def SST_INT_LOCK_RESTORE
 *  \brief This macro restores state of the global interrupt enable flag (previously
 * saved by #SST_INT_LOCK_AND_SAVE).
 */
#define SST_INT_LOCK_RESTORE()      do { SST_restore_ints(_int_state_); } while (0)


/** \def SST_ISR_ENTRY
 *  \brief This macro reenables interrupts inside ISR.
 *
 * This allows to "nest" interrupts. Any hadware related (e.g. timer reinitialization)
 * processing should be done \b before usage of this macro.
 */
#define SST_ISR_ENTRY(isr_prio)     \
    do {                            \
        _pin_ = _SST_CurrPrio;      \
        _SST_CurrPrio = (isr_prio); \
        SST_Start_Nested_Ints();    \
    } while (0)


/** \def SST_ISR_EXIT
 *  \brief This macro marks the end of an ISR.
 */
#define SST_ISR_EXIT()              \
    do {                            \
        SST_INT_LOCK_AND_SAVE();    \
        _SST_CurrPrio = _pin_;      \
        _SST_Schedule(SST_INT_STATE); \
        SST_Stop_Nested_Ints();     \
    } while (0)


/** \typedef SST_TaskID_T
 *  \brief A type to store ID of a task (created by #SST_CreateTask).
 */
typedef CPU_Base_T SST_TaskID_T;


#if !defined(SST_MAX_EV_QUEUE_LEN) || SST_MAX_EV_QUEUE_LEN < 0
#error "SST_MAX_EV_QUEUE_LEN has to be defined with value grether than or equal to zero"
#endif


#if SST_MAX_EV_QUEUE_LEN == 0

typedef struct {
    CPU_Base_T head;
    CPU_Base_T tail;
    CPU_Base_T used;
    CPU_Base_T size;
    SST_Event_T *entries;
} SST_EventQ_T;

#elif SST_MAX_EV_QUEUE_LEN == 1

typedef struct {
    bool is_used;
    SST_Event_T event;
} SST_EventQ_T;

#else

typedef struct {
    CPU_Base_T head;
    CPU_Base_T tail;
    CPU_Base_T used;
    SST_Event_T entries[SST_MAX_EV_QUEUE_LEN];
} SST_EventQ_T;

#endif

typedef struct {
    CPU_Base_T prio;
    SST_TaskID_T next;
#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
    SST_Signal_T msg_signal_mask;
#endif
    void (*task)(SST_Event_T e);
#if SST_MAX_EV_QUEUE_LEN == 0
    SST_EventQ_T *queue;
#else
    SST_EventQ_T queue;
#endif
} TaskCB_T;

extern volatile CPU_Base_T _SST_CurrPrio;

void _SST_Schedule(SST_DECLARE_INT_USAGE);
void _SST_Init(void);

#endif /*SST_INTERN_H_*/
