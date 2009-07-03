# Dual Hexadecimal Display Driver

Uploaded by saundby on 2009-07-03 01:30:00 (rating 0 out of 5)

## Summary

Use dual LED 7 or 10/15 segment displays with line-selectable character sets to display the hexadecimal value of a data byte. This is a complete AVR Studio 4 projects with full assembly code, data sheet, wiring and timing information and notes on modifying the program to serve other needs.


The program as provided will work with common cathode displays. Common anode displays can be used by simple code modifications described in the source, or through the use of external inverters. The program works with 7 segment LEDs, 10 segment LEDs, or 15 segment "British Flag" LED displays used as 10 segment displays.


A line is used as a latch, which when held high will cause the IC to ignore all changes to the data lines, hold and display the last read value on the display, until the latch line is dropped. When the latch line is low, the display will follow the input value on the data lines.


[App notes](http://saundby.com/electronics/M8DualHexDispDriver/appnotes.pdf) are available at [my personal website](http://saundby.com/) since files cannot be added to avrfreaks at present (July 3rd, 2009.) The app notes include use examples with different displays, as well as an example of use with an external buffer for high speed data sources.


[My Electronics Page](http://saundby.com/electronics.html)

## Links

- [General description & pics](http://catsonkeyboards.blogspot.com/2009/04/avr-project-posted-dual-hexadecimal.html)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
