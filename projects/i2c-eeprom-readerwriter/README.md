# I2C EEPROM Reader/Writer

Uploaded by kdiggity08 on 2008-12-08 19:56:00 (rating 0 out of 5)

## Summary

In this project I have interfaced a I2C based EEPROM (Microchip 24LC32) to the Atmel Amega8 AVR. The program will read the first 4 bytes of an EEPROM and display the contents on the LCD. I have added external interrupts to enable erase and program of the EEPROM upon an external key press.


The external interrupts are driven by falling edge and have 10k pullup resisters tied to them. In the interrupt routine, erase or program mode flags are set.


This can then be used as a simple i2c EEPROM programmer with basic read, write, and program modes.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
