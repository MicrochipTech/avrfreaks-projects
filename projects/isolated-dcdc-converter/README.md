# Isolated DC/DC Converter

Uploaded by eskoilola on 2008-05-11 08:07:00 (rating 0 out of 5)

## Summary

This module is actually a part of a bigger project - the tube tester.


This device runs on any voltage between 9 - 16 volts. Voltages beyond that will make the requlator really hot and exceeding 18 volts will fry the circuit. The intended operational voltage is 15 volts.


The converter is really simple with flyback topology and an Atmel doing the PWM and protection stuff. The MCU in this case is AT90LS2323-4PC which lacks support for PWM.


The PWM is done by software and IMHO it works just fine.


The output is short proof and the output voltage is 15-16 volts (not really well regulated). Output power is not too much but if more juice is needed the four 47R resistors may be changed to smaller ones. Beware though that there is a limit to the FET.


As can be seen in the project pictures I succeeded in connecting the primary the wrong way. It seems that no matter how hard I try to figure the rotation out it will always be the wrong way around...

## Links

- [DC/DC Converter @ furpile.com](http://furpile.com/Projects/isolateddcdcconverter/)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
