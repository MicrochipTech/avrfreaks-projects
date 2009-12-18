# Text LCD interface to AVR

Uploaded by AMKhan on 2009-12-17 19:07:00 (rating 0 out of 5)

## Summary

The AVR Project Board (included with the AVR Project Book) contains a space for the Text LCD Display. The Display is not included in the Book and is left for the user.


Below is the details regarding adding a 20 character x 2 Line Text LCD to the AVR Project Board.


The Component required are:


1. AVR Project Board  

2. 20 x 2 Text LCD  

3. 14 wire IDC Cable  

4. 7 x 2 IDC Pins (U3)  

5. 14 x 1 (or 11 x 1) Connector (J4)  

6. 50k Variable Resistor (R10)  

7. 100uF Capacitor (C15)  

8. 10 Ohm Resistor (R4)


Below is the part of Main Board Schematic that needs to be populated


![](http://www.digisoft.com.pk/Projects/text-lcd-on-project-book/TLCD%20Sch.GIF)


After Soldering the Components described above on the AVR Project Board, you need to connect 7 wires to complete the circuit.


PB0 <-> RS (Pin 4 of J4)  

PB1 <-> RW (Pin 5 of J4)  

PB2 <-> E (Pin 6 of J4)  

PB4 <-> D4 (Pin 11 of J4)  

PB5 <-> D5 (Pin 12 of J4)  

PB6 <-> D6 (Pin 13 of J4)  

PB7 <-> D7 (Pin 14 of J4)


After stuffing components and connecting the wires, the Board will look like below.


![](http://www.digisoft.com.pk/Projects/text-lcd-on-project-book/DSC07310.JPG)

## Links

- [Link to AVR project Book Page](http://www.digisoft.com.pk/products/avr-project-book)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
