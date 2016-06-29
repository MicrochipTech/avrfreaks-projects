# A8Eprog 1.00

Uploaded by Ntano on 2016-06-28 20:16:00 (rating 0 out of 5)

## Summary

A8Eprog is an 8bit MCU Electronic EPROM Reader/Programmer


Uses ATmega8515 as the main MCU and ATtiny2313 for some help...


Version 1.00 support UV-EPROM from 16kbit to 2MBit and will be expanded


to Serial/SPI/E-EPROM with the same hardware (whenever the free time is found!)


I'm still searching for that free time to implement the rest functions...


 


The package contain the firmware (.hex files), the control software (PC software)


in setup form and built instructions and firmware specifications in .pdf format.


 


 


### July 3, 2016 Update ###


Just found a semi-bug, if in 27x512 the chip is read more than 1 time, then


after the 2nd time it reports as blank. The first time is OK, but probably something


is left un-resetted and after the 1st time is considered blank.


Because i have only one 27x512 to check, i will probably have to get some before


"officially" announce it as a bug (maybe the chip is bad - it's rather old...)

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
- ATmega8515 ATtiny2313
- A8Eprog
