# SST

Uploaded by arturl on 2006-12-30 16:47:00 (rating 0 out of 5)

## Summary

This is an implementation of a lightweight scheduler so called "Super Simple Tasker" - SST. The idea is taken from the Robert Ward's article - "Practical Real-Time Techniques" [[http://www.quantum-leaps.com/res...](http://www.quantum-leaps.com/resources/Ward03.pdf]).  

Conventional real-time kernels maintain relatively complex execution contexts (including separate stack spaces) for each running thread or task. This gives developer a nice design flexibility and scalability but it is not for free. For smaller devices the requirements for CPU resources can be prohibitive.  

The SST allows to significantly reduce needs for precious RAM and ROM and still allows to keep a real time characteristic of the scheduler (e.g. tasks prioritization and preemption).  

For detailed description of the SST concept look at the mentioned (above) article or at Embedded.com site [http://www.embedded.com/showArti...](http://www.embedded.com/showArticle.jhtml?articleID=190302110)  

Some numbers (using GCC 4.1.1):  

5 tasks with queue for 4 events each (SST RAM usage) 78 bytes  

API for scheduler (FLASH) 672 bytes  

Optional API for periodic and time scheduled tasks (FLASH) 192 bytes  

Optional API for mutex (FLASH) 72 bytes


There is also (optional) functionality which allows to post event to more than one task (broadcast like) by a single function call.


There are parts of the code (in the sst\_base\_def.h) which use GNU specific extensions to enable/disable interrupts but they are very small (a few lines) and easy to replace by constructions used by other toolsets e.g. IAR.  

This code is also ported to ARM (NXP LPC and Atmel AT91 families) and can be very easy adopted for AVR32.


HTML documentation and examples included.

## Tags

- Complete code
