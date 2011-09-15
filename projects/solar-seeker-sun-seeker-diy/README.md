# Solar seeker / Sun seeker. DIY

Uploaded by Cuningan on 2011-09-15 12:07:00 (rating 0 out of 5)

## Summary

This project is intended to be a part of one more bigger destinated to control a DIY solar plant, here in Spain we have a lot of sun and i want to use it.


This small code is for ATTiny13 MCU, the main function is to move toward the sun, for this it uses two Light dependant resistor in serie, this form a resistive voltage divisor and the MCU read the voltage value in the middle.


If the voltage moves up or down, the MCU turn on the motor, in one or other direction using a full H bridge, only on schematic 1.


The H bridge are made with power transistor, TIP31 and TIP32 for the power and BC337, BC 327 for the signal part, because this the motor can be powered with 12 or 24 volts.


On schematic 2 we have 2 axes that can be controlled with 8 relays, i use 8 relays because is cheaper than using 8 power transistors, and the relays can support to 10 amps.


The potentiometer in the schema is for adjusting the threshold of sensitivity before powering on the motor.


All the info, images and schema are in me webpage in English and Spanish, fully downlodable and freely used, if you found any bug please let me know.


[http://cuningan.wordpress.com/20...](http://cuningan.wordpress.com/2011/09/09/seguidor-solar-con-attiny13-solar-seeker-diy/)

## Compilers

- AVR Studio 5

## Tags

- Complete code with HW
- AVR Studio 5
