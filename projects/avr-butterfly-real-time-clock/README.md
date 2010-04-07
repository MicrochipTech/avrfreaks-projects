# AVR Butterfly  Real Time Clock

Uploaded by bigmattyd on 2010-04-07 00:44:00 (rating 0 out of 5)

## Summary

This is a project adapted from SmileyMicros' real time clock demo to display a real time clock on the Butterfly's built in LCD display in a 12-hour format. 


This project was created out of an attempt for me to learn how to program microcontrollers. It is self contained, needing no external connections or extra hardware other than the Butterfly itself, once it is programmed.


This was written for use in the AVR Butterfly, programmed using the simple onboard bootloader and serial PC interface.


It was programmed using AVRprog in AVR Studio 4 and AVR-GCC. (WinAVR) 


The clock is set by pressing the joystick button to enable the set mode. While in set mode, tapping the joystick left will increment the hours, pressing up will increment minutes, and right will increment seconds. Pressing the center button again will exit set mode and disable adjusting the clock.


It incorporates power saving features to prolong battery life. Power saving is accomplished by disabling the USART, SPI, and ADC completely, as well as going into Power Save mode between interrupts. The LCD screen is kept on during Power Save so that the display is always on.


Matt


Note: Updated Zip file on 4/6/2010 at 9:42 pm due to problems with the original. Should work now.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
