# lcd_44780

Uploaded by tymm on 2004-12-04 17:33:00 (rating 0 out of 5)

## Summary

Another LGPL lcd driver library.


Supports screens of different sizes (up to 4 rows x 20 columns), 3 different interface types (standard 8-bit & 4-bit, plus 3-wire interface used by some modules that have CLK, DATA and SEL -- they use 74x164 or similar shift registers for a minimal interface).


For interfaces that support full R/W of display memory, scrolling is supported as well, and "virtual display width", which will scroll display sideways; like having a moving window over a larger text display.


* Dec 7 '04 -- minor update for 4-bit interface, needed for correct operation -- had done minor rev & forgotten to mask bytes read from display properly  

* Dec 8 '04 -- bigger update; ended up using code extensively in a testing project and realizing it had a number of issues. New, improved bits to tweak timing/size tradeoff, some hand optimization and most importantly I actually tested it with a 4-line display and worked out bugs on that front.


As always, feel free to ask questions if you need help.

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
