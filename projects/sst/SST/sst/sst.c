/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

#include <stddef.h>
#include <sst.h>

volatile CPU_Base_T _SST_CurrPrio;

static CPU_Base_T no_sst_tasks;

static volatile TaskCB_T sst_tasks[SST_MAX_NO_OF_TASKS];
static volatile SST_TaskID_T hp_task;

#if SST_MAX_EV_QUEUE_LEN != 1
static bool get_event(SST_EventQ_T * q, SST_Event_T *e_ptr);
static CPU_Base_T put_event(SST_EventQ_T * q, SST_Signal_T s, SST_Param_T p);
#endif

static void add_task_as_ready(SST_TaskID_T id);


#define Remove_First_Ready_Task(_tcb_)      \
    do {                                    \
        hp_task = (_tcb_)->next;            \
        (_tcb_)->next = SST_BAD_TASK_ID;    \
    } while (0)


/******************************************************************************\
\******************************************************************************/
void _SST_Schedule(SST_DECLARE_INT_USAGE)
{
    CPU_Base_T cprio;
    TaskCB_T *tcb;
    SST_Event_T e;

    // do not schedule during ISR or when there are no tasks in the ready list
    if ((_SST_CurrPrio >= SST_MIN_INT_PRIO) || !SST_IS_VALID_TASK(hp_task))
    {
        return;
    }

    // save the initial priority
    cprio = _SST_CurrPrio;

    do
    {
        // get from the ready list the task with the highest priority
        tcb = (TaskCB_T *) &sst_tasks[hp_task];

        // is the new task priority higher than the initial?
        if (tcb->prio <= cprio)
        {
            break;
        }

        // get the event out of the queue
#if SST_MAX_EV_QUEUE_LEN == 0

        if (get_event(tcb->queue, &e))
        {
            if (0 == tcb->queue->used)

#elif SST_MAX_EV_QUEUE_LEN == 1

        if (tcb->queue.is_used)
        {
            e = tcb->queue.event;
            tcb->queue.is_used = false;

#else

        if (get_event(&tcb->queue, &e))
        {
            if (0 == tcb->queue.used)

#endif

            {
                // if no more tasks in queue then delete it from the ready list
                Remove_First_Ready_Task(tcb);
            }
            // this becomes the current task priority
            _SST_CurrPrio = tcb->prio;
            SST_INT_LOCK_RESTORE();
            tcb->task(e);
            SST_INT_LOCK();
        }
        else
        {
            // safety check, it should not happen -
            // task without events on the ready list
            Remove_First_Ready_Task(tcb);
        }
    } while (SST_IS_VALID_TASK(hp_task));

    // restore the initial priority
    _SST_CurrPrio = cprio;
}


/******************************************************************************\
\******************************************************************************/
bool SST_PostEvent(SST_TaskID_T id, SST_Signal_T sig, SST_Param_T par)
{
    SST_DECLARE_INT_USAGE;
    CPU_Base_T no_events;


#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
    no_events = 0;

    if (id != SST_TASK_BROADCAST_ID)
    {
#endif

        if (!SST_IS_VALID_TASK(id))
        {
            return false;
        }

#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
    }
#endif

    SST_INT_LOCK_AND_SAVE();

#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
    if (SST_TASK_BROADCAST_ID == id)
    {
        // if id is equal to SST_TASK_BROADCAST_ID then broadcast event
        for (id = 0; id < no_sst_tasks; id++)
        {
            if (sst_tasks[id].msg_signal_mask & sig)
            {
#if SST_MAX_EV_QUEUE_LEN == 0

                no_events = put_event(sst_tasks[id].queue, sig, par);

#elif SST_MAX_EV_QUEUE_LEN == 1

                if (sst_tasks[id].queue.is_used)
                {
                    no_events = 0;
                }
                else
                {
                    no_events = 1;
                    sst_tasks[id].queue.event.sig = sig;
                    sst_tasks[id].queue.event.par = par;
                }

#else

                no_events = put_event((SST_EventQ_T *) &sst_tasks[id].queue, sig, par);

#endif

                // add to the ready list only if it is a first event for this task
                if ((CPU_Base_T) 1 == no_events)
                {
                    add_task_as_ready(id);
                }
            }
        }
    }
    else
    {
#endif


#if SST_MAX_EV_QUEUE_LEN == 0

        no_events = put_event(sst_tasks[id].queue, sig, par);

#elif SST_MAX_EV_QUEUE_LEN == 1

        if (sst_tasks[id].queue.is_used)
        {
            no_events = 0;
        }
        else
        {
            no_events = 1;
            sst_tasks[id].queue.event.sig = sig;
            sst_tasks[id].queue.event.par = par;
        }

#else

        no_events = put_event((SST_EventQ_T *) &sst_tasks[id].queue, sig, par);

#endif

        // add to the ready list only if it is a first event for this task
        if ((CPU_Base_T) 1 == no_events)
        {
            add_task_as_ready(id);
        }

#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
    }
#endif

    _SST_Schedule(SST_INT_STATE);

    SST_INT_LOCK_RESTORE();

    return (no_events != 0);
}


#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0

/******************************************************************************\
\******************************************************************************/
void SST_SetSignalMask(SST_TaskID_T id, SST_Signal_T signal_mask)
{
    if (SST_IS_VALID_TASK(id))
    {
        sst_tasks[id].msg_signal_mask = signal_mask;
    }
}

#endif


/******************************************************************************\
\******************************************************************************/
void _SST_Init(void)
{
    SST_DECLARE_INT_USAGE;

    hp_task = SST_BAD_TASK_ID;

    // setup tasks and interrupts handling
    SST_Start();

    SST_INT_LOCK_AND_SAVE();
    // set the priority for the SST idle loop
    _SST_CurrPrio = SST_IDLE_TASK_PRIO;

    // process all events produced so far
    _SST_Schedule(SST_INT_STATE);

    SST_INT_LOCK_RESTORE();
}


/******************************************************************************\
\******************************************************************************/
#if SST_MAX_EV_QUEUE_LEN == 0
SST_TaskID_T SST_CreateTask(CPU_Base_T prio, void (*task)(SST_Event_T), SST_EventQ_T* ev_queue)
#else
SST_TaskID_T SST_CreateTask(CPU_Base_T prio, void (*task)(SST_Event_T))
#endif
{
    SST_TaskID_T id;

    id = SST_BAD_TASK_ID;

    if ((prio > SST_IDLE_TASK_PRIO) &&
        (prio < (CPU_Base_T) SST_MAX_TASK_PRIO) &&
        (no_sst_tasks < SST_MAX_NO_OF_TASKS))
    {
        id = no_sst_tasks;
        sst_tasks[id].prio = prio;
        sst_tasks[id].next = SST_BAD_TASK_ID;
#if defined(SST_USE_SIGNAL_MASK) && SST_USE_SIGNAL_MASK != 0
        sst_tasks[id].msg_signal_mask = 0;
#endif
        sst_tasks[id].task = task;

#if SST_MAX_EV_QUEUE_LEN == 0
        sst_tasks[id].queue = ev_queue;
#elif SST_MAX_EV_QUEUE_LEN == 1
        sst_tasks[id].queue.is_used = false;
#else
        sst_tasks[id].queue.head = 0;
        sst_tasks[id].queue.tail = 0;
        sst_tasks[id].queue.used = 0;
#endif

        no_sst_tasks++;
    }

    return id;
}



#if SST_MAX_EV_QUEUE_LEN != 1


/******************************************************************************\
\******************************************************************************/
static bool get_event(SST_EventQ_T * q, SST_Event_T *e_ptr)
{
    if (q->used)
    {
        *e_ptr = q->entries[q->tail];

        q->used--;
        q->tail++;

#if SST_MAX_EV_QUEUE_LEN == 0
        if (q->size == q->tail)
#else
        if ((CPU_Base_T) SST_MAX_EV_QUEUE_LEN == q->tail)
#endif
        {
            q->tail = 0;
        }

        return true;
    }
    else
    {
        return false;
    }

}


/******************************************************************************\
\******************************************************************************/
static CPU_Base_T put_event(SST_EventQ_T * q, SST_Signal_T s, SST_Param_T p)
{
#if SST_MAX_EV_QUEUE_LEN == 0
    if (q->used < q->size)
#else
    if (q->used < (CPU_Base_T) SST_MAX_EV_QUEUE_LEN)
#endif
    {
        // insert the event at the head
        q->entries[q->head].sig = s;
        q->entries[q->head].par = p;

        q->used++;
        q->head++;

#if SST_MAX_EV_QUEUE_LEN == 0
        if (q->size == q->head)
#else
        if ((CPU_Base_T) SST_MAX_EV_QUEUE_LEN == q->head)
#endif
        {
            q->head = 0;
        }

        // event successfully posted
        return q->used;
    }
    else
    {
        // queue full, event posting failed
        return 0;
    }
}

#endif /* SST_MAX_EV_QUEUE_LEN != 1 */


/******************************************************************************\
\******************************************************************************/
static void add_task_as_ready(SST_TaskID_T id)
{
    SST_TaskID_T next_id;
    SST_TaskID_T prev_id;
    CPU_Base_T id_prio;

    prev_id = hp_task;

    if (SST_IS_VALID_TASK(prev_id))
    {
        id_prio = sst_tasks[id].prio;
        if (sst_tasks[prev_id].prio >= id_prio)
        {
            next_id = sst_tasks[prev_id].next;

            // while (SST_IS_VALID_TASK(next_id) && (sst_tasks[next_id].prio >= id_prio))
            do {
                if (!SST_IS_VALID_TASK(next_id))
                {
                    break;
                }
                if (sst_tasks[next_id].prio < id_prio)
                {
                    break;
                }
                prev_id = next_id;
                next_id = sst_tasks[next_id].next;
            } while (true);

            sst_tasks[id].next = next_id;
            sst_tasks[prev_id].next = id;
        }
        else
        {
            sst_tasks[id].next = hp_task;
            hp_task = id;
        }
    }
    else
    {
        hp_task = id;
        sst_tasks[id].next = SST_BAD_TASK_ID;
    }
}

