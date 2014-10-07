// Code module for the A task

#include    <avr/io.h>
#include    "OS.h"

extern void Atask (void)
{
   SetAsyncFlag(taskB);    // example of asynchronous start of task
}

extern void Ainit (void)
{
   SetTaskInterval(taskA, 10);
   EnableSyncTask(taskA, true);
}
