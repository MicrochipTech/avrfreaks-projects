#ifndef SST_TIMER_H_
#define SST_TIMER_H_

/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

/** \file sst_timer.h
 *  \brief A header file for time scheduled tasks functionality of the SST.
 *
 *  Requires \c sst.h (from sst folder) headers.
 */

#include <sst.h>


struct SST_Ticker_Tag;


/** \typedef SST_TimedTask_T
 *  \brief The datatype used to carry information about time scheduled SST task.
 *
 * If given task is time schedulable by SST ticker then some additional
 * information have to be stored for such task. The SST API uses this type
 * to do that.<br>
 * This type (fields of structure) is never directly used by user. The API uses it
 * internally and user is obliged to keep only pointers to object of this type
 * if there is a need to manipulate given task (e.g. reschedule it by
 * #SST_ScheduleTimedTask).
 */
typedef struct SST_TimedTask_Tag {
    bool is_enabled;
    CPU_Base_T current_val;
    CPU_Base_T reload_val;
    SST_TaskID_T task_id;
#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
    struct SST_Ticker_Tag *ticker;
#endif
} SST_TimedTask_T;


/** \typedef SST_Ticker_T
 *  \brief The datatype used to carry information about time scheduled SST tasks.
 *
 * This type (fields of structure) is never directly used by user. The API uses it
 * internally and user is obliged to only pass pointers to object of this type
 * (e.g. reschedule it by #SST_ScheduleTimedTask).
 */
struct SST_Ticker_Tag {
    CPU_Base_T no_tasks;
    SST_TimedTask_T* ttasks;
#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
    bool is_enabled;
    void (*ticker_busy)(void);
    void (*ticker_idle)(void);
#endif
};

typedef struct SST_Ticker_Tag SST_Ticker_T;


/** \def SST_TICKER_DECLARE
 *  \brief This macro is used when there is a need do declare externally visible
 * SST ticker object of a name \a _name_.
 *
 * This is only declaration (extern) and somewhere in C file it is needed a
 * definition by use of #SST_TICKER_DEFINE.
 */
#define SST_TICKER_DECLARE(_name_)      extern SST_Ticker_T _name_


/** \def SST_TICKER_DEFINE
 *  \brief This macro is used to define a SST ticker object of a name \a _name_
 * which is capable to manage \a _size_ number of timed SST tasks.
 *
 * This macro declares object which is used internally by SST API and thhis object
 * should be used only as a parameter to API functions.<br>
 * This macro will have different syntax when compiled when #SST_USE_LAZY_TIMER macro
 * is defined to nonzero value.<br>
 * Setting the callback function pointer to NULL value will result in blocking call to it.
 *
 *  \param _name_ - name of the created ticker object
 *  \param _size_ - max number of timed tasks associated with ticker object
 *  \param ticker_busy_callback - pointer to callback function executed when ticker is activated
 *  \param ticker_idle_callback - pointer to callback function executed when ticker is not
 *   active (no timed tasks enabled)
 */
#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
#define SST_TICKER_DEFINE(_name_, _size_, ticker_busy_callback, ticker_idle_callback)  \
    static SST_TimedTask_T _name_ ## _ttimers_ [(_size_)];          \
SST_Ticker_T _name_ = { 0, _name_ ## _ttimers_ , false, (ticker_busy_callback), (ticker_idle_callback) }
#else
#define SST_TICKER_DEFINE(_name_, _size_)                           \
    static SST_TimedTask_T _name_ ## _ttimers_ [(_size_)];          \
SST_Ticker_T _name_ = { 0, _name_ ## _ttimers_  }
#endif


/** \fn SST_TimedTask_T* SST_MakeTimedTask(SST_TaskID_T task_id, CPU_Base_T period, bool enabled, SST_Ticker_T *ticker)
 *  \brief Makes a SST task time schedulable.
 *
 * \warning Function does not check if there is available slot in the ticker object.<br>
 *
 * The value returmed by this function is only needed (should be preserved)
 * if there is planned some manipulation of given task e.g. by the #SST_ManageTimedTask.
 *
 *  \param task_id - an ID of the SST task (created by #SST_CreateTask)
 *  \param period - a period if task is periodic or \c 0 if task is "one shot" schedulable,
 * for periodic task the time unit is in ticker "ticks" see #SST_TimerTick
 *  \param enabled - determines if given task should be initially enabled for use by ticker
 *  \param ticker - pointer to SST ticker object, defined by #SST_TICKER_DEFINE
 *  \return pointer to the #SST_TimedTask_T type
 */
SST_TimedTask_T* SST_MakeTimedTask(SST_TaskID_T task_id, CPU_Base_T period, bool enabled, SST_Ticker_T *ticker);


/** \fn void SST_ManageTimedTask(SST_TimedTask_T* ttask, bool enabled)
 *  \brief Allows to enable or disable time schedulable SST task.
 *
 *  \param ttask - a pointer to the #SST_TimedTask_T type as returned by the
 * #SST_MakeTimedTask function
 *  \param enabled - true if task should be enabled, false otherwise
 */
void SST_ManageTimedTask(SST_TimedTask_T* ttask, bool enabled);


/** \fn void SST_ScheduleTimedTask(SST_TimedTask_T* ttask, CPU_Base_T delay)
 *  \brief Schedules execution of the time schedulable SST task after delay time.
 *
 *  Even if the timed task was previously disabled it will be enabled
 *  and its timer will start counting.<br>
 *  Call to this function will reload timer associated with this task. It
 *  means that if for given task there was 2 "ticks" left to schedule then
 *  after the call it will be \c delay "ticks".<br>
 *  If task was created as periodic (the \c period parameter for
 *  #SST_MakeTimedTask was not 0) then this function will start periodic
 *  scheduling (after \c delay) of this task every \c period.
 *
 *  \param ttask - a pointer to the #SST_TimedTask_T type as returned by the
 * #SST_MakeTimedTask function
 *  \param delay - time after which given task will be executed
 * the time unit is in ticker "ticks" see #SST_TimerTick
 */
void SST_ScheduleTimedTask(SST_TimedTask_T* ttask, CPU_Base_T delay);


/** \fn void SST_TimerTick(SST_Ticker_T *ticker)
 *  \brief A main function used for drive time schedulable tasks.
 *
 * This function should be called periodically (e.g. from timer ISR).<br>
 * The period between calls is a time unit for other API functions, e.g.
 * if \c SST_TimerTick will be called every 10 ms then SST timed task
 * prepared with call to #SST_MakeTimedTask function and with parameter period
 * equal to 10 will be executed every 100 ms.
 *
 *  \param ticker - pointer to SST ticker object, defined by #SST_TICKER_DEFINE
 */
void SST_TimerTick(SST_Ticker_T *ticker);


#endif /*SST_TIMER_H_*/

