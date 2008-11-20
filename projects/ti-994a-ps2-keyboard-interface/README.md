# TI-99/4A PS/2 Keyboard Interface

Uploaded by Tursi on 2008-11-20 06:15:00 (rating 0 out of 5)

## Summary

This was my first AVR project, and it allows interfacing of a PS/2 or AT keyboard to the TI-99/4A home computer, a classic machine from 1981. It's designed for the ATMega16 (which is overkill, but that's what I had when I started!), and runs with very few external parts (just a clock and a filter cap).


My goal for this project was to prove that a single-part microcontroller could provide PS/2 keyboard interface to the TI. The TI uses a scan-matrix with 48 keys for it's keyboard. I was able to handle the scanning in real time with the Atmel, while the PS/2 side is handled via interrupts triggered by the clock line. Using the internal pull-ups I was able to interface the PS/2 keyboard without external parts.


I did cheat, I overclock the Atmel to 20MHz (ATMega16 max rating is 16MHz). I tested extensively at this clock rate, and have distributed a dozen or so boards with no problems.


Source isn't currently available, although that may change in the near future. The full hex file and schematics are available, though. I did the board layout using PCBExpress' software, so the formats are theirs.


My official project page is [http://harmlesslion.com/software...](http://harmlesslion.com/software/adapter)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
