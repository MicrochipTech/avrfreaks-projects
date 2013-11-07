# 32 bit timer int.c

Uploaded by colldecarrera on 2013-11-07 11:05:00 (rating 0 out of 5)

## Summary

This code shows you how to concatenate two timers through the event system, in order to create a 1 second timer. Timers used are C and E. When timer C overflows, it generates an event which increments by one timer E. When this timer reaches 61, interrupts the program toggling bit C1. This program also uses the 32 Mhz internal clock. To check the timing you can connect an oscilloscope on C1 pin or a led instead.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
