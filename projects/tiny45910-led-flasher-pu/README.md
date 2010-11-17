# Tiny4/5/9/10 LED Flasher with Pu

Uploaded by bhammill on 2010-11-17 14:34:00 (rating 0 out of 5)

## Summary

On an STK600 with the Tiny10 top module installed connect PB0 and PB1 to LEDs. Connect PB2 to a switch. Program the hex file into the Tiny device. This should work on Tiny4, Tiny5, Tiny9, and Tiny10. Put the clock switch in the INT position.


The LEDs will toggle approximately every 8 seconds when the timer overflow interrupt occurs.


The LEDs will immediately toggle if you press the switch.


Project settings in EWAVR 5.5 have to be changed from the default since the stack and heap settings are too big for these Tiny devices. But this project works and is a good starting point. Enjoy!


Brian Hammill  

Atmel Corporation  

November 17, 2010

## Compilers

- IAR Embedded Workbench for AVR

## Tags

- Complete code
- IAR Embedded Workbench for AVR
