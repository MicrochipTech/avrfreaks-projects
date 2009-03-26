# gos

Uploaded by tcg on 2009-03-26 14:15:00 (rating 0 out of 5)

## Summary

GOS (Günters OS ;-)  

simple multitasker for atmel avr and winavr (gcc 4.1.2)  

(c) 2007 by günter greschenz ([g@greschenz.de](mailto:g@greschenz.de))


*) V0.000 2001 (?)  

 first working version for 68000 (PalmPilot)  

 (but i lost my old code ...)  

*) V0.001 2007-05  

 first working version for atmel avrs  

 new functions:  

 void taskCreate(Task *t, TaskFunction f, TaskPrio prio);  

 void taskSwitch();  

*) V0.002 2007-06  

 new (optional) functions:  

 void taskSignal(TaskSignal *s);  

 void taskWait(TaskSignal *s);  

*) V0.003 2007-07-20  

 added atmega32  

*) V0.004 2007-07-21  

 added atmega163


download on my homepage: <http://www.greschenz.de>


i included gos in my [AvrWizOnline](http://greschenz.dyndns.org/awo.php)


---EXAMPLE---


#include "tasks.h"


#ifdef TASK\_USE\_SIGNAL  

TaskSignal s;  

Task ts; void fs() { for (;;) { taskWait(&s); PORTB = 0x00; } }  

Task tw; void fw() { for (;;) { PORTB = 0xff; taskSignal(&s); } }  

#else  

Task t0; void f0() { for (;;) { PORTB = 0x00; } }  

Task t1; void f1() { for (;;) { PORTB = 0x01; } }  

Task t2; void f2() { for (;;) { PORTB = 0x02; } }  

Task t3; void f3() { for (;;) { PORTB = 0x04; } }  

Task t4; void f4() { for (;;) { PORTB = 0x08; } }  

Task t5; void f5() { for (;;) { cli(); PORTB = 0xff; taskSwitch(); } }  

#endif


int main()  

{  

#ifdef TASK\_USE\_SIGNAL  

 taskCreate(&ts, fs, TaskPrioHighHigh);  

 taskCreate(&tw, fw, TaskPrioHighHigh);  

#else  

 taskCreate(&t0, f0, TaskPrioLowLow);  

 taskCreate(&t1, f1, TaskPrioLow);  

 taskCreate(&t0, f0, TaskPrioLowLow);  

 taskCreate(&t2, f2, TaskPrioMedium);  

 taskCreate(&t0, f0, TaskPrioLowLow);  

 taskCreate(&t3, f3, TaskPrioHigh);  

 taskCreate(&t0, f0, TaskPrioLowLow);  

 taskCreate(&t4, f4, TaskPrioHighHigh);  

 taskCreate(&t0, f0, TaskPrioLowLow);  

 taskCreate(&t5, f5, TaskPrioLowLow);  

 taskCreate(&t0, f0, TaskPrioLowLow);  

#endif  

 taskSwitch();  

 return 0;  

}


---KNOWN PROBLEMS---


*) "taskSwitch();" without created tasks (loops for ever ...)  

 int main()  

 {  

 taskSwitch();  

 return 0;  

 }  

*) "returning tasks" fail (tasks must loop for ever...)  

 Task te; void fe() {}  

 int main()  

 {  

 taskCreate(&te, fe, TaskPrioLowLow);  

 taskSwitch();  

 return 0;  

 }

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
