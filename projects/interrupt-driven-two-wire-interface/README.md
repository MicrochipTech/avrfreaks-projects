# Interrupt driven Two Wire Interface

Uploaded by kkempeneers on 2003-11-11 12:36:00 (rating 0 out of 5)

## Summary

This is a compact interrupt driven two wire interface, as my other projects not so much a library function but an example on how to use interrupt driven I2C. The function main calls the interface driver to set up a DS1631 stand alone I2C thermometer / thermostat, in the endless loop it reads the temperature and outputs the high byte on portb. The software is written for a ATMega8 but the code can be easely adapted for any AVR with a two wire interface

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
