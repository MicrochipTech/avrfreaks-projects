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

#include "example.h"
#include "ring_buf.h"

#define DEBOUNCE_TIME           ((CPU_Base_T) 10U)


#define TIMER_10MS_TASK_PRIO    (SST_MIN_TASK_PRIO + 5)
#define TIMER_100MS_TASK_PRIO   (SST_MIN_TASK_PRIO + 5)
#define KBD_TASK_PRIO           (SST_MIN_TASK_PRIO + 6)
#define INTERPETER_TASK_PRIO    (SST_MIN_TASK_PRIO + 4)

#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
#define LED_MASK    8
#endif

SST_TaskID_T timer_10ms_tid;
SST_TaskID_T timer_100ms_tid;
SST_TaskID_T btn_task_id;
SST_TaskID_T interp_task_id;

#if SST_MAX_EV_QUEUE_LEN == 0
SST_EVENTQ_DEFINE(t_10ms_queue, 2);
SST_EVENTQ_DEFINE(t_100ms_queue, 2);
SST_EVENTQ_DEFINE(btn_queue, 2);
SST_EVENTQ_DEFINE(interp_queue, 10);
#endif /* SST_MAX_EV_QUEUE_LEN == 0 */

#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
void stop_timer100ms(void);
void start_timer100ms(void);
SST_TICKER_DEFINE(ticker_100ms, 1, start_timer100ms, stop_timer100ms);
SST_TICKER_DEFINE(ticker_10ms, 1, NULL, NULL);
#else
SST_TICKER_DEFINE(ticker_100ms, 1);
SST_TICKER_DEFINE(ticker_10ms, 1);
#endif /* defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0 */


SST_TimedTask_T* ttask_100ms;

volatile RingBuffer_T tx_buf;


#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
void stop_timer100ms(void)

{
   TIMER100MS_OFF();
}


void start_timer100ms(void)
{
   TIMER100MS_ON();
}
#endif


void timer_10ms_func(SST_Event_T e)
{
    LED_TOGGLE(LED0);
}


void timer_100ms_func(SST_Event_T e)
{
    if ((SST_Signal_T) 1 == e.sig)
    {
        SST_ManageTimedTask(ttask_100ms, e.par);
        LED_OFF(LED1);
    }
    else
    {
        LED_TOGGLE(LED1);
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
                SST_PostEvent(timer_100ms_tid, 1, t_enabled);
            }
        }
    }
    else
    {
        counter = 0;
    }
}


static void printchar(uint8_t byte)
{
    SST_Wait_Busy_While(IS_BUF_FULL(tx_buf));
    put_buf(&tx_buf, byte);
}


static void print(const char *str)
{
    while (*str)
    {
        SST_Wait_Busy_While(IS_BUF_FULL(tx_buf));
        put_buf(&tx_buf, *str);
        str++;
    }
}


typedef enum {
    IS_ENTER,
    IS_A,
    IS_T,
    IS_AT,
    IS_AT_EXT,
    IS_AT_EXT_C
} Interpreter_State_T;


void interpreter_task_func(SST_Event_T e)
{
    static Interpreter_State_T state = IS_A;
    static const char *cmd = NULL;

    printchar(e.par);
    if ('\r' == e.par)
    {
        printchar('\n');
    }

    switch (state)
    {
        case IS_ENTER:
            if ('\r' == e.par)
            {
                if (cmd)
                {
                    print(cmd);
                    cmd = NULL;
                }
                state = IS_A;
            }
            else
            {
                cmd = NULL;
            }
            break;

        case IS_A:
            if (('a' == e.par) || ('A' == e.par))
            {
                state = IS_T;
            }
            else
            {
                state = (('\r' == e.par) ? IS_A : IS_ENTER);
            }
            break;

        case IS_T:
            if (('t' == e.par) || ('T' == e.par))
            {
                state = IS_AT;
            }
            else
            {
                state = (('\r' == e.par) ? IS_A : IS_ENTER);
            }
            break;

        case IS_AT:
            switch (e.par)
            {
                case 'z':
                case 'Z':
                    // exec ATZ
                    cmd = "Command ATZ\n\r";
                    state = IS_ENTER;
                    break;

                case 'h':
                case 'H':
                    // exec ATH
                    cmd = "Command ATH\n\r";
                    state = IS_ENTER;
                    break;

                case '&':
                    state = IS_AT_EXT;
                    break;

                case ' ':
                    // ignore spaces between AT and command name
                    break;

                default:
                    state = (('\r' == e.par) ? IS_A : IS_ENTER);
                    break;
            }
            break;

        case IS_AT_EXT:
            switch (e.par)
            {
                case 'c':
                case 'C':
                    state = IS_AT_EXT_C;
                    break;

                default:
                    state = (('\r' == e.par) ? IS_A : IS_ENTER);
                    break;
            }
            break;

        case IS_AT_EXT_C:
            switch (e.par)
            {
                case '0':
                    // exec &C0
                    cmd = "Command AT&C0\n\r";
                    state = IS_ENTER;
                    break;

                case '1':
                    // exec &C1
                    cmd = "Command AT&C1\n\r";
                    state = IS_ENTER;
                    break;

                case '2':
                    // exec &C2
                    cmd = "Command AT&C2\n\r";
                    state = IS_ENTER;
                    break;

                default:
                    state = (('\r' == e.par) ? IS_A : IS_ENTER);
                    break;
            }
            break;

        default:
            state = IS_ENTER;
            break;
    }

#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
    SST_PostEvent(SST_TASK_BROADCAST_ID, LED_MASK, 0);
#endif
}

void SST_Start(void)
{
    ioinit();

#if SST_MAX_EV_QUEUE_LEN == 0
    timer_10ms_tid = SST_CreateTask(TIMER_10MS_TASK_PRIO, timer_10ms_func, &t_10ms_queue);
    timer_100ms_tid = SST_CreateTask(TIMER_100MS_TASK_PRIO, timer_100ms_func, &t_100ms_queue);
    btn_task_id = SST_CreateTask(KBD_TASK_PRIO, btn_task_func, &btn_queue);
    interp_task_id = SST_CreateTask(INTERPETER_TASK_PRIO, interpreter_task_func, &interp_queue);
#else
    timer_10ms_tid = SST_CreateTask(TIMER_10MS_TASK_PRIO, timer_10ms_func);
    timer_100ms_tid = SST_CreateTask(TIMER_100MS_TASK_PRIO, timer_100ms_func);
    btn_task_id = SST_CreateTask(KBD_TASK_PRIO, btn_task_func);
    interp_task_id = SST_CreateTask(INTERPETER_TASK_PRIO, interpreter_task_func);
#endif

#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
    SST_SetSignalMask(timer_10ms_tid, LED_MASK);
    SST_SetSignalMask(timer_100ms_tid, LED_MASK);
#endif

    ttask_100ms = SST_MakeTimedTask(timer_100ms_tid, 5, true, &ticker_100ms);
    (void) SST_MakeTimedTask(timer_10ms_tid, 20, true, &ticker_10ms);

    clear_buf(&tx_buf);

    SST_INT_UNLOCK();
}


int main(void)
{
    SST_Run();
}

