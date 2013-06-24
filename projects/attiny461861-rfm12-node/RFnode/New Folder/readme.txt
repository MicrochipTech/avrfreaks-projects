Kicad PCB/Schematic files and AVR-studio-4/6 project files in assembly language.

This is a RFM12B node using an ATtiny861.  
I mainly use these as a temperature sensor with a 1-wire
Dallas DS18B20.

Its well suited to battery operations as most of the time, its asleep.

The packet structure is detailed in the RFM12.asm file.
Data packets have 16 bit CRC checksums x-modem style.

The software is by no means optimised so it it offends you, modify it to suit yourself.
Its an offshoot of an earlier project, when I first started to play aroud with RFM12's


Enjoy :) (or not....)

StewieTee.

