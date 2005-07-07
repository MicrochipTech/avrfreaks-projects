# Routines to use Interrupt-Driven writes to EEPROM

Uploaded by Spamiam on 2005-07-07 09:45:00 (rating 0 out of 5)

## Summary

Here is the GCC source code to use interrupt driven writes to EEPROM. This will allow the foreground task to return to regular operations while the EEPROM gets written as fast as the hardware allows in the background.


An example test program is given in the comments in the C source code.


This was developed and tested on an ATMega128, but ought to to easily portable to most AVR devices (which have EEPROM and an interrupt for it).


The syntax is the same as for the EEPROM access in the AVR library.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
