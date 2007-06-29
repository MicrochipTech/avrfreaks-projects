# csRTOS port to ATmega32

Uploaded by davef on 2007-06-29 06:57:00 (rating 0 out of 5)

## Summary

A port of Glen Worstell's csRTOS to the ATmega32.


csRTOS is a priority-based co-operative scheduler that handles up to 8 tasks. This can be expanded to 16 tasks with little effort.


All user tasks, including ones run as the result of an ISR are assigned a fixed priority at compile time.


Semaphores are provided for protecting shared data.


Glen's code has several useful examples in setting up different types of tasks. This 2004 AVR design project is very well documented, which should enable you to easily modify the code for your application. 


Corrected functions below thanks to curtvm. See AVRfreaks thread "RTOS for AVR" for details.


csRTOS\_ATmega32v4.zip has corrected saveTask(), restoreTask() and ADC example.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
