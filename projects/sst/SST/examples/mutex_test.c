/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

#include <stddef.h>
#include <sst_timer.h>
#include <sst_mutex.h>

#include "example.h"
#include "ring_buf.h"

#define USE_SST_MUTEX   0


#define DEBOUNCE_TIME           ((CPU_Base_T) 10U)


#define KBD_TASK_PRIO           (SST_MIN_TASK_PRIO + 15)

#define TIMER_TASK_PRIO_CEIL    (SST_MIN_TASK_PRIO + 10)

#define TIMER1_TASK_PRIO        (SST_MIN_TASK_PRIO + 7)
#define TIMER2_TASK_PRIO        (SST_MIN_TASK_PRIO + 5)


SST_TaskID_T timer1_task_id;
SST_TaskID_T timer2_task_id;
SST_TaskID_T btn_task_id;
SST_TaskID_T interp_task_id;

#if SST_MAX_EV_QUEUE_LEN == 0
SST_EVENTQ_DEFINE(t1_queue, 2);
SST_EVENTQ_DEFINE(t2_queue, 2);
SST_EVENTQ_DEFINE(btn_queue, 2);
#endif /* SST_MAX_EV_QUEUE_LEN == 0 */

#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
SST_TICKER_DEFINE(ticker_10ms, 2, NULL, NULL);
SST_TICKER_DEFINE(ticker_100ms, 1, NULL, NULL);
#else
SST_TICKER_DEFINE(ticker_10ms, 2);
SST_TICKER_DEFINE(ticker_100ms, 1);
#endif /* defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0 */



SST_TimedTask_T* ttask2;

volatile RingBuffer_T tx_buf;


static void print(const char *str)
{
#if defined(USE_SST_MUTEX) && USE_SST_MUTEX > 0
    SST_Mutex_T mutex;
    mutex = SST_MutexLock(TIMER_TASK_PRIO_CEIL);
#endif

    while (*str)
    {
        SST_Wait_Busy_While(IS_BUF_FULL(tx_buf));
        put_buf(&tx_buf, *str);
        str++;
    }

#if defined(USE_SST_MUTEX) && USE_SST_MUTEX > 0
    SST_MutexUnlock(mutex);
#endif
}


void timer1_task_func(SST_Event_T e)
{
    LED_TOGGLE(LED0);
    print("Timer task 1 is alive and well feeling well\n\r");
}


void timer2_task_func(SST_Event_T e)
{
    if (e.sig)
    {
        SST_ManageTimedTask(ttask2, e.par);
        LED_OFF(LED1);

        print("Timer task 2 is now ");
        print(e.par ? "ON \n\r" : "OFF \n\r");
    }
    else
    {
        LED_TOGGLE(LED1);
        print("Timer task 2 is alive and feeling very well\n\r");
    }
}


void btn_task_func(SST_Event_T e)
{
    static CPU_Base_T counter;
    static bool t_enabled = false;

    if (IS_BTN_PRESS())
    {
        if (counter < DEBOUNCE_TIME)
        {
            counter++;
            if (DEBOUNCE_TIME == counter)
            {
                t_enabled = !t_enabled;
                SST_PostEvent(timer2_task_id, 1, t_enabled);
            }
        }
    }
    else
    {
        counter = 0;
    }
}

void SST_Start(void)
{
    ioinit();

#if SST_MAX_EV_QUEUE_LEN == 0
    timer1_task_id = SST_CreateTask(TIMER1_TASK_PRIO, timer1_task_func, &t1_queue);
    timer2_task_id = SST_CreateTask(TIMER2_TASK_PRIO, timer2_task_func, &t2_queue);
    btn_task_id = SST_CreateTask(KBD_TASK_PRIO, btn_task_func, &btn_queue);
#else
    timer1_task_id = SST_CreateTask(TIMER1_TASK_PRIO, timer1_task_func);
    timer2_task_id = SST_CreateTask(TIMER2_TASK_PRIO, timer2_task_func);
    btn_task_id = SST_CreateTask(KBD_TASK_PRIO, btn_task_func);
#endif

    ttask2 = SST_MakeTimedTask(timer2_task_id, 11, true, &ticker_10ms);
    (void) SST_MakeTimedTask(timer1_task_id, 30, true, &ticker_10ms);

    clear_buf(&tx_buf);

    SST_INT_UNLOCK();
}


int main(void)
{
    SST_Run();
}
