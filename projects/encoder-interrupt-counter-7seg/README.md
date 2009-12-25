# Encoder Interrupt Counter 7seg

Uploaded by mjmg910 on 2009-12-25 00:26:00 (rating 0 out of 5)

## Summary

encoder is incremented or decremented by 1 pulse of encoder depending on rotation. In this particular apps, it uses encoder with 1000pulses/rev. with each pulse = 2 microns. The display uses 6 digit with 1 micron resolution. The system uses 5phase stepper motor in a linear slide bearing apps. The 8Mhz internal oscillator is use since it can still detect pulses from the encoder. If you will have to apply in diff. system, take note of the following:  

1. The max speed of your motor (check if it can catch those pulses)  

2. The length of 1 pulse of your encoder should be factored in your program if your lucky you'll get 1:1, totally depend on your system. You have to measure 1 pulse or take the average of 10 pulses. You need to find this for proper display.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
