# DC Servo Motor Control

Uploaded by AMKhan on 2010-05-13 04:02:00 (rating 0 out of 5)

## Summary

DC Servo Motor Control  

----------------------


![](http://www.digisoft.com.pk/Projects/dc-servo-motor-control/RC-servo-small.jpg)


The R/C Servo Motors are used in model aircraft, R/C Trucks, R/C motorboats and robots.


Radio Control Servo motors are position servo systems that are controlled digitally (through PWM). The usual range is about 180 degrees.


The connections to the motor are


1. GND (Black)  

2. VCC (Red)  

3. Control (White)


The PWM signal to control the position should have a pulse duration between 10-30 msec. and the on time determines the position of servo. A 1.5 msec on-time makes the Servo position at the middle (picture of the the pulse is given on the website)


In order to Interface the R/C Servo to the AVR Project board, three wires are used to connect the Servo to the Board as ststed above. Two of the wire can be connected to the any VCC and GND signal (provided it could support the current requirement of the motor). The control wire is connected to the PORTD PIN5 (OC1A/PD5). The Timer1 of the ATMEGA16 can produce a PWM output on this pin. Two Switches are also connected to the PC0 and PC1 to make it rotate Clockwise and Counter Clockwise.


Pictures and source code are given on [website.](http://www.digisoft.com.pk)

## Links

- [DC Servo Motor attached to AVR Project Board](http://www.digisoft.com.pk/Projects/dc-servo-motor-control/Servo.jpg)
- [Pictures and Code](http://www.digisoft.com.pk/Projects/dc-servo-motor-control)
- [Link to AVR project Book Page](http://www.digisoft.com.pk/products/avr-project-book)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
