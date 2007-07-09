# Simple Task Scheduler

Uploaded by admin on 2007-07-09 12:19:00 (rating 0 out of 5)

## Summary

Simple Scheduler,is an assembly implementation of atmel's app.note:CompactScheduler in C language.  

with this Scheduler u can Crate tasks,enble or disble tasks , or reuse a task when u done ur job with it.  

each task, uses 7bytes of sram as a SchedTask structure.  

task satatus: 1byte  

current value of task: 2bytes  

preset value of task: 2bytes  

pointer to function: 2bytes  

using pointers in this Scheduler(ZRAMP) makes easy to use a task again just by changing the pointer at runtime.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
