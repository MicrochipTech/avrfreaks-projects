# RC5 Control of PWM Servo

Uploaded by dylan85 on 2009-07-29 09:10:00 (rating 0 out of 5)

## Summary

This project was my first 'real' application of an AVR and 'C' coding. It may not be too pretty but it certainly works as intended.  

The code was originally developed on a Mega16 and was then ported across to a ATtiny24.  

The original RC5 decode is courtesy of Peter Fleury and has been 'tweaked' to suit the application and device used.  

The IR code is demodulated using a standard 3 pin 38kHz receiver and its output is fed into Pin 5 (PB2) of the tiny 24.  

The PWM output is taken off of pin 7 (OC1A).  

The RC5 decoder uses Timer/Counter 0 (8 bit) and the Overflow interrupt for the timing whilst Timer/Counter 1 (16 bit) is used to generate the PWM output for the servo. The servo algorithm contains checks such that only certain keys on the remote control are accepted.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
