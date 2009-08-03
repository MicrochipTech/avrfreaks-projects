# ATTiny2313 Servo controller

Uploaded by LenHolgate on 2009-08-03 07:03:00 (rating 0 out of 5)

## Summary

I'm teaching myself about electronics and embedded programming so that I can build a hexapod robot. This is the first part of the robot's firmware; a PWM servo controller. 


The hardware uses multiple CD74HCT238E 3 to 8 line demultipexor chips to expand the number of I/O pins that we can address with the ATTiny2313. I then use Timer1 in CTC mode to generate up to 64 PWM signals by hand. The serial interface conforms to the simple and popular 3 byte SSC protocol.


You can find details of the design decisions and the journey I took from a simple 8 channel servo controller that uses hard coded timing loops to the 64 channel version using interrupts here:


<http://www.lhexapod.com/blog/2009/08/a-timer-driven-pwm-servo-controller---part-4.html> 


Comments, suggestions, bug reports and code critique all welcome; this is my first assembler project and I want to learn!

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
