# ATtiny12 Stepper Motor Driver

Uploaded by jambelly on 2008-05-26 21:46:00 (rating 0 out of 5)

## Summary

Two inputs: a negative-going-edge clock signalling "step" on PB1, and direction (Clockwise or CCW being high or low) on the Reset pin. The program uses the INT0 interrupt to sense the step signal.  

The outputs (PB0, PB2, PB3, PB4) go to the gates of four power MOSFETS. These transistors switch the motor's coils on and off.  

The common side of the motor is tied to 12v while each coil is tied to the drain of the MOSFET (with a diode to handle backlash.) When the output pin is low, the nMOS FET is cut-off. When high, current flows through the coil. The motor I used had 200 steps per revolution (1.8 deg), so with this program, it ended up having 400 steps (0.9 deg).  

I was able to pump clock pulses through it as high as 1550 Hz before the motor locked up, which is 3.875 rev/sec (=232.5 rpm).  

The motor used was a 4H4018S0106 out of an old Umax scanner.  

I aim to use the ATtiny12 and this code as the module which controls the motor, but which will in turn be driven by another AVR supplying step and directions signals.  

Alternatively, I will use the code in other apps to do one-chip-solutions.

## Links

- [Stepper Motor program for ATtiny12](http://enghead.com/tiny12stepper.asm)

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
