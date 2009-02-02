# Butterfly_RTC_demo

Uploaded by jremington on 2009-02-02 00:31:00 (rating 0 out of 5)

## Summary

I've added a new feature to Dean Camera's elegant LCD\_Driver routines to allow individual flashing digits, and included general-purpose drivers for the joystick and RTC. The routines are demonstrated by implementing a real-time clock which can be set in an intuitive fashion using the joystick. The RTC is implemented as a BCD character buffer which can be accessed as a six-byte global. This gives access to digit-change flags for accurate timing of data loggers in 1 second, 10 second, 1 minute, 10 minute, 1 hour and 10 hour intervals. The joystick input routine getkey() is debounced and has an autorepeat function controllable by a global flag.


Update 2/1/2009: 


Corrected a problem with joystick initialization routine, which set PORTE to be all input.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
