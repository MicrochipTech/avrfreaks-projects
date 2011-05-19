# Universal Digital Panel Voltmete

Uploaded by UCSchmidt on 2011-05-19 02:46:00 (rating 0 out of 5)

## Summary

Universal Digital Panel Voltmeter


I occasionally build variable output power supplies. I have never been able to find suitable panel voltmeters. For example if I build a 0 - 25 Volt supply, a 0 - 30 Volt meter would be appropriate. The meters that I  

have been able to find can only be configured for 2, 20, or 200 volts.


So I decided to develop my own.


It can be configured for 0 - 5V to 0 - 50V in 5V increments very easily.  

There is a spreadsheet with resistor values and scaling constants for each range. The required scaling constants need to be put into the source file and assembled. It uses 4 large 7 segment displays and an Atmel  

Atmega48 processor. It uses 2 small PCBs piggybacked on each other. One  

is the display board and the other is the processor board.


The code is in assembler for AVR Studio. It is very heavily commented to allow easy modification.


There is a paper by R. Gordon which explains the fixed point math. It is the most lucid description that I could find. I also included Atmel's Ap Notes.


There are 2 PCBs ..... a display and a processor board.


The schematics and PCBs are are done in KiCAD, a free open source CAD package. All schematic and necessary Lib files are included. I also generated PDFs of the schematics. I was not able to generate usable  

component layout images .... a bug in PCBnew. However all Gerber and drill files are there. 


I added a few pics to make it clearer how I assembled the module.

## Tags

- Complete code with HW
