#ifndef OS_H
   #define OS_H

   // definitions used by the bool type
   #define true      1
   #define false     0
   #define bool      _Bool

   // constant definitions for each task run / enable flag
   typedef enum {taskA, taskB, taskC} tasks;

   extern void SetAsyncFlag (tasks task);
   extern void EnableSyncTask (tasks task, bool enable);
   extern void SetTaskInterval (tasks task, uint8_t interval);

#endif

