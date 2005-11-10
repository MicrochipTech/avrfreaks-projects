# LCD Control for mega 8515

Uploaded by markquarless on 2005-11-10 04:57:00 (rating 0 out of 5)

## Summary

This collection of routines writes commands and data to the Hitachi HD44780 LCD controller. Please refer to it's instruction set for a description.  

PORTC is the 8 DATA lines to the LCD and PORTD pins 5-7 are the control. PORTD PIN4 is used for +5V to the LCD.  

Becuase stack operations are used, this code only needs one register.  

For the HD44780 LCD controller it is possible to use 4-line DATA mode, therefore saving 4 AVR pins which I will implement later.

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
