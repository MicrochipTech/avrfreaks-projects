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


#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
static void TickerBusy(SST_Ticker_T *ticker);
#endif


/******************************************************************************\
\******************************************************************************/
SST_TimedTask_T* SST_MakeTimedTask(SST_TaskID_T task_id, CPU_Base_T period, bool enabled, SST_Ticker_T *ticker)
{
    SST_TimedTask_T *ttask;

    ttask = &ticker->ttasks[ticker->no_tasks];
    ttask->is_enabled = ((period != 0) ? enabled : false);
    ttask->current_val = period;
    ttask->reload_val = period;
    ttask->task_id = task_id;
    ticker->no_tasks++;

#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
    ttask->ticker = ticker;
    if (ttask->is_enabled)
    {
        TickerBusy(ticker);
    }
#endif

    return ttask;
}


/******************************************************************************\
\******************************************************************************/
void SST_ManageTimedTask(SST_TimedTask_T* ttask, bool enabled)
{
    ttask->is_enabled = enabled;

    if (enabled)
    {
        ttask->current_val = ttask->reload_val;

#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
        TickerBusy(ttask->ticker);
#endif

    }
}


/******************************************************************************\
\******************************************************************************/
void SST_ScheduleTimedTask(SST_TimedTask_T* ttask, CPU_Base_T delay)
{
    ttask->current_val = delay;
    ttask->is_enabled = true;

#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
    TickerBusy(ttask->ticker);
#endif
}


/******************************************************************************\
\******************************************************************************/
void SST_TimerTick(SST_Ticker_T *ticker)
{
    CPU_Base_T i;
    SST_TimedTask_T *ttask;

#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
    if (!ticker->is_enabled)
    {
        return;
    }

    ticker->is_enabled = false;
#endif

    for (i = 0; i < ticker->no_tasks; i++)
    {
        ttask = &ticker->ttasks[i];

        if (!ttask->is_enabled)
        {
            continue;
        }

        if (ttask->current_val)
        {
            ttask->current_val--;
        }

        if ((CPU_Base_T) 0 == ttask->current_val)
        {
            if (ttask->reload_val)
            {
                ttask->current_val = ttask->reload_val;
            }
            else
            {
                ttask->is_enabled = false;
            }
            (void) SST_ScheduleTask(ttask->task_id);
        }

#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
        if (ttask->is_enabled)
        {
            ticker->is_enabled = true;
        }
#endif

    }

#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0
    if (!ticker->is_enabled && (ticker->ticker_idle != NULL))
    {
        ticker->ticker_idle();
    }
#endif
}



#if defined(SST_USE_LAZY_TIMER) && SST_USE_LAZY_TIMER != 0

/******************************************************************************\
\******************************************************************************/
static void TickerBusy(SST_Ticker_T *ticker)
{
    if (!ticker->is_enabled)
    {
        ticker->is_enabled = true;
        if (ticker->ticker_busy != NULL)
        {
            ticker->ticker_busy();
        }
    }
}

#endif

