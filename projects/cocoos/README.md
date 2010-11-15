# cocoOS

Uploaded by ptrxtrnd on 2010-11-14 19:24:00 (rating 0 out of 5)

## Summary

Edit 2010-11-14: cocoOS 2.0.0 released.  

This release is a major rework of cocoOS internals. A new API, implementation of message queues and updated documentation are some of the news.


Please visit the project web page: [www.cocoos.net](http://www.cocoos.net)


cocoOS is a highly portable, extremely easy to use, scheduler for microcontrollers with limited resources.


Watch this video: [http://www.youtube.com/watch?v=p...](http://www.youtube.com/watch?v=pmu_Vp-1KvY) that demonstrates a brick game implemented using cocoOS running on an ATmega16 on the STK500. The application consists of 6 tasks managed by the scheduler. The UART is used to send game board data (ball positions, racket position, brick details) to a PC that draws the game board graphics.


cocoOS is a cooperative task scheduler, based on coroutines.  

Task procedures scheduled by cooperative kernels are so called RTC's, run to completion tasks. They execute from the first line of the procedure to the last line. 


The use of coroutines enables us to implement task procedures that does not have to execute all the way down to the last line of code. Instead execution can end in the middle e.g waiting for a semaphore to be released. When execution is resumed, it continues at that line. 


The fine thing with coroutines, is that this can be done without having to save the complete task context when switching to another task.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
