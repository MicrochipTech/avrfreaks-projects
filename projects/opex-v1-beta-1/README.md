# OPEX v1 beta 1

Uploaded by stevech on 2004-07-19 00:00:00 (rating 0 out of 5)

## Summary

EXTRACTED FROM THE PDF FILE DESCRIBING OPEX  

beta 1. Testing to date with demo program is reliable with long run times. 


OPEX Is …  

C software for WinAVR (GCC) optionally using the Atman IDE  

For an embedded system (microprocessor) programmer who is using the C language. 


A control program  

To oversee multiple “thread-like” processes. Each process or “task” uses only ~22 bytes of overhead


A Scheduler  

To cause processes to run when desired, such as:·  

At specific dates and/or times, optionally recurring·  

At time intervals, either fixed or variable, optionally recurring·;  

 When the serial port receives a byte or an end-of-line character·;  

When inter-process signals are received


An Easy Way To Implement Multiple State Machines  

Using the Scheduler’s functions which include, among others·  

Task Create and Quit·  

Task time delay or wait for specific date/time·  

Task wait-for-flag, set-flag (semaphore)


A clock and calendar  

With time zone, daylight savings and leap year, maintained using a microprocessor timer such as the 32768KHz asynchronous oscillator of the Atmel megas.


Serial port support  

Functions for interrupt driven input/output, optionally non-blocking, for binary or string (ASCII 8 bit) data


Monitoring and Debugging Aids  

Functions to format and display a list of all processes, the detailed state and status of each, plus RAM used and unused, and maximum observed stack depth


Automatic Memory Allocation Housekeeping A task may use the OPEX forms of malloc() to obtain temporary use of a block of RAM. When the task quits, OPEX automatically frees this memory if the task did not do so. A modified version of the standard malloc() is included in OPEX.


Complete with Examples  

Dallas 1-wire reader for DS1820 temperature chips  

I/O bit sensing and switch debouncingSerial I/O for host commands  

and set clock/calendar

## Compilers

- AtmanAvr C/C++ IDE

## Tags

- Complete code
- AtmanAvr C/C++ IDE
