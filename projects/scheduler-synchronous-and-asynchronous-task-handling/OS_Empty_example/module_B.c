// Code module for the B task

#include    <avr/io.h>
#include    "OS.h"

extern void Btask (void)
{
   SetAsyncFlag(taskC);    // example of asynchronous start of task
}

extern void Binit (void)
{
   SetTaskInterval(taskB, 20);
   EnableSyncTask(taskB, true);

}
