# OSEK AVR - a little OS like OSEK

Uploaded by Kermitderfrosch on 2009-12-18 08:51:00 (rating 0 out of 5)

## Summary

SCHWED-OSEK 2.0 Draft  

supported compilers(latest versions): IAR,GCC,IMAGECRAFT,CODEVISION and CROSSWORKS


Hi,  

this is a little OS, like a small OSEK (OSEK: is a common RTOS for automotive apps.).


Focus of this OS is scheduling cooperative and preemptive tasks without heavy stack usage.


MAIN FILES:


OS.C/OS.H: kernel and support functions  

target.h: compiler and device specific stuff  

Atmegaxxx.c: port to specific AVR device  

ostypes.h: OS typedefinitions  

task.c/h: user task definitions and task profile  

os\_cnf.h: OS options:  

 #define OS\_USE\_PREEMP\_TASK\_OPTION  

 /* Preemptive tasks option is active */


 #define OS\_USE\_16BIT\_TIMETABLE\_OPTION  

 /* tasks can be auto scheduled between 1ms..65535ms */  

 /* otherwise scheduling is 8Bit-> 1ms..255ms */


 #define OS\_USE\_MAX\_16TASKS\_OPTION  

 /* A maximum of 16 tasks are possible */  

 /* otherwise only 8 tasks are supported */


See task.c how to define tasks and the options for.  

Also task priority and preemp handling is explained in task.c .  

Included in this package is a port for ATmega8/16/32 running at 8Mhz. It is easy to migrate to other devices, because  

you only have to define a timebase interrupt with 1ms occurrence.


Supported OSEK API:


TASK: BCC1 type supported with TASK(x) definition  

ACTIVATION: In the task profile(e.g task.c) you can define tasks as auto scheduled (ms)  

 or as manual scheduled. Manual scheduled tasks are activated with API  

 ActivateTask(TASK\_ID)  

TERMINATION: Light termination supported.  

 --> last statement of a task must be always TerminateTask()  

 or ChainTask(TASK\_ID) which defines next task to be in ready state


Atomic: SuspendAllInterrupts() -> disables interrupts  

 ResumeAllInterrupts() -> enables interrupts


TIME: get time (ms) with GetStopWatch()


18.12.2009 fixed Crossworks issue with write to sfrw register


 Also you are welcome for feedback on SCHWED-OSEK bugs and improvements.  

 Please mail to:


 [SCHWED-OS@arcor.de](mailto:SCHWED-OS@arcor.de)

## Tags

- Complete code
