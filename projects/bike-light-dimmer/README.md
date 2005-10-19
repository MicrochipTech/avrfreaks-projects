# Bike Light Dimmer

Uploaded by marks on 2005-10-19 06:58:00 (rating 0 out of 5)

## Summary

The Bike Light Dimmer controls the brightness of one or two 6-12v bike lights.


Design:


The ATMega8’s PWM generator is connected to the gate of the MOSFET, which in-turn provides a voltage square wave to the light to control its brightness. It has been tested and I’m using it for controlling 12V 20W halogen bike lights.


It is based on the ATMega8. This is the smallest DIP chip that has reasonable usability & functionality – possibly over the top for a bike light dimmer, as it has more computing power than an Apollo rocket, but who cares!  

The design uses an external crystal. Although I’ve not tried it, I am sure that the internal AVR clock could be used and the crystal removed from the design.


By using a DIP version of the AVR, the electronics can be hand wired onto a tiny bit of Veroboard (or, just wired directly to the DIP pins). It can all be crammed into a matchbox-sized plastic box (or potted)


Power to the electronics comes from the lights battery, the circuit has a 5V regulator.


I’ve chosen a TTL compatible MOSFET so it can be directly connected to the AVR PWM pin – no need of interface glue. The chosen MOSFET has a low on-resistance so no power is wasted across the MOSFET.


The software will support two lights with independent control of each. The code to control the 2nd is present by commented out.


The schematic shows support for only one light unit, simply duplicate the switch, MOSFET and bits for the other.


The software is written in C, compiles with winAVR / avr-gcc (GCC) 3.4.1, and includes a Programmers Notepad project file.  

It does need AVRX, its IS included in the project files. you can download information on it from [www.barello.net](http://www.barello.net) or through the AVR Freaks web site.  

Operation:


Pushing and releasing the button brightens the light. The brightness will step up to the next set point. There are four brightness steps.  

Pushing and holding the button will dim the light.  

Pushing and holding the button will turn off the light if it’s at its dimmest.  

On power up, the light will turn on the light to full brightness.


The timings, number of steps etc can all be changed with very little change to the s/w.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
