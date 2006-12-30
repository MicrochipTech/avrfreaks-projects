#ifndef SST_H_
#define SST_H_

/*******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *******************************************************************************/

/** \file sst.h
 *  \brief A header file for core functionality of the SST.
 *
 *  Requires \c bool.h (user supplied) and \c sst_intern.h (from sst folder) headers.
 */

#include <bool.h>
#include <sst_intern.h>


#if SST_MAX_EV_QUEUE_LEN == 0
/** \def SST_EVENTQ_DEFINE
 *  \brief This macro is used to define a SST event queue object of a name \a _name_
 * which is capable to keep \a _size_ number of SST events.
 *
 * This macro declares event queue object which is used internally by SST API and this object
 * should be used only as a parameter to #SST_CreateTask function.<br>
 * This macro will be available only when #SST_MAX_EV_QUEUE_LEN is defined to 0.<br>
 *
 *  \param _name_ - name of the created event queue object
 *  \param _size_ - max number of timed tasks associated with ticker object
 */
#define SST_EVENTQ_DEFINE(_name_, _size_)  \
static SST_Event_T _name_ ## _events_ [(_size_)];          \
SST_EventQ_T _name_ = { 0, 0, 0, (_size_), _name_ ## _events_ }
#endif


/** \def SST_MIN_TASK_PRIO
 *  \brief The lowest priority of a user task.
 *
 *  User tasks should have priority equal to or greather than value
 *  of this macro.
 */
#define SST_MIN_TASK_PRIO           ((CPU_Base_T) SST_IDLE_TASK_PRIO + 1)

/** \def SST_IS_VALID_TASK
 *  \brief Tests if the \a _task_id_ is a valid task ID.
 *
 *  User can use this macro to test if task ID returned by #SST_CreateTask is
 *  valid and if task was cucessfully created.
 */
#define SST_IS_VALID_TASK(_task_id_)   ((SST_TaskID_T)(_task_id_) < SST_BAD_TASK_ID)

// user SST entry
#if defined(SST_USE_COND_RUN) && SST_USE_COND_RUN != 0
#define SST_Run(_cond_) \
    _SST_Init();        \
    while (_cond_)      \
    {                   \
        SST_OnIdle();   \
    }
#else
/** \def SST_Run
 *  \brief A macro with initializes and starts SST scheduler.
 *
 *  This the main entry point for the SST functionality. In fact it can be
 *  the only content of the \c main function.<br>
 *  It is important to not enable interrupts before using this macro. The whole
 *  inititialization (including interrupts) should placed in the user supplied
 *  function #SST_Start
 */
#define SST_Run()       \
    _SST_Init();        \
    for (;;)            \
    {                   \
        SST_OnIdle();   \
    }
#endif

#if SST_MAX_EV_QUEUE_LEN == 0

/** \fn SST_TaskID_T SST_CreateTask(CPU_Base_T prio, void (*task)(SST_Event_T), SST_EventQ_T* ev_queue)
 *  \brief Creates a new SST task.
 *
 *  User can use #SST_IS_VALID_TASK macro to test result of this function to detect
 *  if task creation was sucessfull.
 *
 *  \param prio - priority of the created task, it should be greather than
 *  or equal to #SST_MIN_TASK_PRIO
 *  \param task - pointer to a task function
 *  \param ev_queue - pointer to an event queue which will be associated with created task,
 *    this parameter is available only when the macro #SST_MAX_EV_QUEUE_LEN
 *    is defined to value 0 (zero)
 *  \return ID of created task or invalid ID (#SST_BAD_TASK_ID) if task cannot be created
 *  (e.g. no available TCBs because of too small #SST_MAX_NO_OF_TASKS).
 */
SST_TaskID_T SST_CreateTask(CPU_Base_T prio, void (*task)(SST_Event_T), SST_EventQ_T* ev_queue);
#else
SST_TaskID_T SST_CreateTask(CPU_Base_T prio, void (*task)(SST_Event_T));
#endif


/** \fn bool SST_PostEvent(SST_TaskID_T id, SST_Signal_T sig, SST_Param_T par)
 *  \brief Sends an avent to the SST task.
 *
 *  This is a main communication mechanism between tasks.<br>
 *  It is also the only method to schedule given task to run.<br>
 *  When code is compiled with macro SST_USE_SIGNAL_MASK defined to nonzero value
 *  then id parameter has a special meaning. In such case when it has value of
 *  #SST_TASK_BROADCAST_ID then event will be posted to all task with signal mask which
 *  matches with signal value. Matching is performed by bitwise \c AND.
 *
 *  \param id - ID of the event receiver task(s)
 *  \param sig - signal value for event
 *  \param par - parameter value for event
 *  \return \c true if event was send sucessfully or \c false if not (e.g.
 *  there is no space left in the receiver event queue)
 */
bool SST_PostEvent(SST_TaskID_T id, SST_Signal_T sig, SST_Param_T par);

/** \fn bool SST_ScheduleTask(SST_TaskID_T id)
 *  \brief A helper function to schedule given task to run.
 *
 *  This function send to the task with id an event with signal 0 and parameter 0.<br>
 *
 *  \param id - ID of the task required to schedule
 *  \return \c true if schedule was sucessfull or \c false if not (e.g.
 *  there is no space left in the receiver event queue)
 */
static inline bool SST_ScheduleTask(SST_TaskID_T id)
{
    return SST_PostEvent(id, (SST_Signal_T) 0, (SST_Param_T) 0);
}


/** \fn void SST_SetSignalMask(SST_TaskID_T id, SST_Signal_T signal_mask)
 *  \brief Defines signal mask for given task.
 *
 *  User can assign signal mask for task. When SST_PostEvent is executed with
 *  id value equal to #SST_TASK_BROADCAST_ID then event will be posted for each task
 *  which \c signal_mask matches with signal value. matching is performed
 *  by bitwise \c AND.<br>
 *  This function is available only when code is compiled with macro
 *  \c SST_USE_SIGNAL_MASK defined to nonzero value.
 *
 *  \param id - ID of the task for which signal mask will be assigned
 *  \param signal_mask - value of signal mask
 */
void SST_SetSignalMask(SST_TaskID_T id, SST_Signal_T signal_mask);


/** \def SST_Wait_Busy_While
 *  \brief A macro which performs busy wait until \a _cond_ becomes true.
 *
 * Waiting is performed by calling user callback function #SST_OnIdle.
 * It means that \a _cond_ evaluation will be executed \b only if #SST_OnIdle
 * ends.<br>
 * Typical usage is to wait for IO event (which generates interrupt), in such
 * case the #SST_OnIdle will return immediately and \a _cond_ can be reevaluated.
 */
#define SST_Wait_Busy_While(_cond_) \
    while (_cond_)                  \
    {                               \
        SST_OnIdle();               \
    }


/** \fn void SST_Start(void)
 *  \brief A user supplied callback used for initialization.
 *
 * This function will be called by the #SST_Run macro before it enters
 * infinite loop.<br>
 * Typically it should be used to setup IO, create SST tasks, initialize
 * timers and interrupts. <br>
 * Usually at the end user should enable interrupts by using the #SST_INT_UNLOCK macro.
 */
void SST_Start(void);

/** \fn void SST_OnIdle(void)
 *  \brief A user supplied callback used for signaling the scheduler idle time.
 *
 * This function will be called by the SST scheduler when there is no tasks
 * ready to run.<br>
 * Usually this function will lower power consumption and inhibit CPU
 * until next interrupt by switching microcontroller to "sleep" mode.<br>
 * Keep in mind that there is no "wake up" phase - after receiving interrupt
 * the microcontroller should automatically (or by executing special code in ISR)
 * switch to normal mode.
 */
void SST_OnIdle(void);


#endif /*SST_H_*/

