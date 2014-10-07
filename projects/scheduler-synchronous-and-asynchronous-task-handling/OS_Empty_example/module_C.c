// Code module for the C task

#include    <avr/io.h>
#include    "OS.h"

extern void Ctask (void)
{
   SetAsyncFlag(taskA);   // example of asynchronous start of task
}

extern void Cinit (void)
{
   SetTaskInterval(taskC, 30);
   EnableSyncTask(taskC, true);   
}
