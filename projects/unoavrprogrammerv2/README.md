# UNO_AVR_Programmer_V2

Uploaded by osbornema on 2020-10-21 13:52:00 (rating 0 out of 5)

## Summary

UNO\_AVR\_Programmer is code that runs on the UNO and enables it to be used to program Atmega328 family devices plus the Atmega644 (posted on 9 September 2020).


 


Version two of this project also enables ATtiny44 and 461 family devices to be programmed and calibrated so that they can be reliably clocked using their internal RC oscillators.


This development enables devices with 14, 20, 28 and 40 pins containing up to 64kB of flash to be programmed.


 


These devices also offer a Universal Serial Interface USI in place of the more usual UART. Atmel provide an application note and code for driving the USI that has been used in this project.


 


See [http://becomingmaker.com/usi-ser...](http://becomingmaker.com/usi-serial-uart-attiny85/) for the links to this Atmel documentation.


(Note: The author of this web page is also an Osborne, a different Osborne however from the one posting this project).


 


Two projects are posted here:


 


Version 2 of the programmer which is supplied as an  Arduino project.


Cal\_ATtiny\_44\_461 which can be used to calibrate the internal RC clocks of ATtiny44 and 461 family devices supplied as an Atmel Studio 7 project


 


 


Version 2 of Cal\_ATtiny\_44\_461


 


With ATtiny461 and 861 devices there is an option to use the default ports  PB0-2 or PA0-2 for the USI or to select ports PA0-2.  When ports PA0-2 are chosen there is no


conflict between the USI and programming pins.  This is a big improvement since there is no longer any need to disconnect the USI when programming.


 


Version 2 does this, but of course the ATtiny44 devices do not offer this option and the USI must still be disconnected when the UNO\_AVR\_Programmer is running.

## Compilers

- Arduino IDE

## Tags

- Complete code with HW
- Arduino IDE
