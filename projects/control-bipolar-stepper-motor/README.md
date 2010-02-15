# Control of bipolar stepper motor

Uploaded by dexil on 2010-02-15 14:52:00 (rating 1 out of 5)

## Summary

I trying to make control of Bipolar stepper motor(200 steps)  

through USB (with CP2102 USB to UART converter).  

I have code for ATmega48 microcontroller but i need to change 'em little bit.


command to start moving the motor look like this:  

"move [steps] [accel] [decel] [speed]"  

there is no STOP command


In Simple words i need to change  

"move [steps] [accel] [decel] [speed]" to  

"move [CW (or CCW)] [accel] [decel] [speed]"  

and i need "STOP" command.


This software was written for the IAR Embedded Workbench 4.11A  

(Atmel's code)


Please help me change the code!

## Compilers

- IAR Embedded Workbench for AVR

## Tags

- Complete code
- IAR Embedded Workbench for AVR
