# Epson Graphic LCD Driver

Uploaded by mpanetta on 2004-04-24 03:21:00 (rating 0 out of 5)

## Summary

This file contains code to drive an epson SED133X or S1D13700 type graphic LCD controller. Its not specific to any type of LCD, just the controller. The tar file contains an example main.c that prints "System Initialized" in the upper left hand corner of the display, and then plots a circle and 2 "vectors" that go around inside it like a clock, one clockwise and one counterclockwise. The code was compiled and tested on custom ATMega128 based boards, so for now its specific to the ATMega128. Eventually I will add code to let it run on any AVR with an expansion bus.


Eventually the SED133X and S1D13700 drivers will become unified as they are almost the same code (the SED chips are about 3-5x slower then the S1D13700 though).


Any questions on the code should be posted to the avr projects forum.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
