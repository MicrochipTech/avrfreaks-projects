# optiboot

Uploaded by westfw on 2013-03-18 06:44:00 (rating 0 out of 5)

## Summary

Optiboot is a very small (512 byte) bootloader, written in C, that implements a subset of the STK500v1 protocol for communications (a big enough subset to communicate with AVRDude.) It is the bootloader used on the Arduino Uno, and currently supports the following AVRs: ATmega328p, ATmega168, ATmega88, ATmega8, ATmega32, ATmega644, ATmega1284, ATmega1280.


In addition to its utility as a bootloader, optiboot is a good illustration of how to minimize the size of an avr-gcc program. It uses gcc attributes "OS\_main" and "naked", alternate sections (.init9 and .version), and link options to supress the standard startup code and libraries.


Optiboot is maintained under Mercurial version control at Google Code:  

<http://code.google.com/p/optiboot/>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
