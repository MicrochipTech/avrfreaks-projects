/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

#include <sst_mutex.h>

/******************************************************************************\
\******************************************************************************/
SST_Mutex_T SST_MutexLock(CPU_Base_T prio_ceiling)
{
    SST_DECLARE_INT_USAGE;
    SST_Mutex_T p;

    SST_INT_LOCK_AND_SAVE();

    p = _SST_CurrPrio;

    if (prio_ceiling > _SST_CurrPrio)
    {
        _SST_CurrPrio = prio_ceiling;
    }

    SST_INT_LOCK_RESTORE();

    return p;
}


/******************************************************************************\
\******************************************************************************/
void SST_MutexUnlock(SST_Mutex_T mutex)
{
    SST_DECLARE_INT_USAGE;

    SST_INT_LOCK_AND_SAVE();

    if (mutex < _SST_CurrPrio)
    {
        _SST_CurrPrio = mutex;
        _SST_Schedule(SST_INT_STATE);
    }

    SST_INT_LOCK_RESTORE();
}
