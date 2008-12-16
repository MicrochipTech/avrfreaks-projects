# ATMEGA8 Development Board

Uploaded by stockerc on 2008-12-15 22:51:00 (rating 0 out of 5)

## Summary

The board runs on a 9 volt battery using a 5 volt voltage reguator for power. The controller is a Atmel ATMEGA8-16PU, it has 8 input switches and eight output LEDs. This is the first version of my desighn and is on a single layer PCB. Future version will be 2 sided PCB's and will probably at the MAX232 chip for serial communication. There is also a 10-pin jtag header for easier programing.


PORTB is connected to the DIP switches as well as a pin for each bit. The switches should be off for programing. With the switches of the pins can be connected to an exteral sourse as either an input or output.


PORTD is connected to the LEDs as well as pins for exteranal use. the LEDs are connected to a 330ohm sip resistor with a switch to ground. with the switch of PORTD can e used by external sourses.


PORTC is connected to pins and can be used externaly.


I listed AVR Studio 4 but anything will do.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
