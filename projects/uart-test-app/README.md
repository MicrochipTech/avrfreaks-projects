# UART Test App

Uploaded by Microcas on 2005-10-05 12:52:00 (rating 0 out of 5)

## Summary

This project was created for being able to quickly test and debug the functionality of MCU hardware and wiring of UART applications. This has been fully tested on the STK500 and is fully functional. It should be pretty easy for anyone to alter the speed at which the AVR transmits characters to the PC (even for the newbies). It simply repeatedly cycles through the extended ASCII character set starting from 0x21(!) through 0xFF. Probably not the most efficient code, but I was in a hurry. 


Using the 2-wire jumpers, plug the PD0 and PD1 pins into the RXD and TXD pins respectively.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
