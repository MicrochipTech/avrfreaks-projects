#include <sst_timer.h>

#define TIMER_TASK_PRIO        (SST_MIN_TASK_PRIO + 1)
#define EVENT_TASK_PRIO        (SST_MIN_TASK_PRIO + 2)

SST_TaskID_T timer_task_id;
SST_TaskID_T event_task_id;

SST_TICKER_DEFINE(ticker, 1);

// this is an ISR function for periodic timer interrupt
void Timer_ISR(void)
{
    SST_DECLARE_ISR;
    
    // do some timer HW housekeeping (if needed)

    SST_ISR_ENTRY(SST_MIN_INT_PRIO+1);

    SST_TimerTick(&ticker);

    SST_ISR_EXIT();
}


void timer_task_func(SST_Event_T e)
{
    static CPU_Base_T ticks;

    ticks++;

    // do some other processing
    
    // sometime start event triggered task
    if (0 == (ticks % 3))
    {
        SST_PostEvent(event_task_id, 3, ticks);
    }
    else if (0 == (ticks % 8))
    {
        SST_PostEvent(event_task_id, 8, ticks);
    }
    else
    {
    }
}


void event_task_func(SST_Event_T e)
{
    printf("divident = %lu,\tticks = %lu\n", (uint32_t)e.sig, (uint32_t)e.par);
}


void SST_Start(void)
{
    // perform some IO anf HW initialization
    ioinit();

    timer_task_id = SST_CreateTask(TIMER_TASK_PRIO, timer_task_func);
    event_task_id = SST_CreateTask(EVENT_TASK_PRIO, event_task_func);

    // create a periodic task -  period is 10 calls of ticker
    (void) SST_MakeTimedTask(timer_task_id, 10, true, &ticker);

    // enable interrupts
    SST_INT_UNLOCK();
}


void SST_OnIdle(void)
{
    // put microcontroller into "idle" mode
}


int main(void)
{
    SST_Run();
}
