# Conway's AI-Life for an AVR

Uploaded by SmokingBowl on 2008-05-19 00:29:00 (rating 0 out of 5)

## Summary

Not really a game, see the wikipedia article for further reference to the rules:


[http://en.wikipedia.org/wiki/Con...](http://en.wikipedia.org/wiki/Conway%27s_Game_of_Life)


this was written and debugged in C using a Mega32, but can obviously be rewritten accordingly. Programming is done via the ISP, and the required components are one 3:8 demultiplexer (i am using a 74138), 8x 330Ω resistors (i am using an 8 pack), one microprocessor with at least 11 i/o lines (i am using a mega32), one small cap and a 1KΩ resistor for powering and running the μP (read: microprocessor)... oh and a bunch of wires.


The following video is the program set up to display a "glider" on a breadboard build of the circuit:

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
