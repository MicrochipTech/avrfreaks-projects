# RPM meter for car (tachometer)

Uploaded by bitfox on 2007-05-02 05:26:00 (rating 0 out of 5)

## Summary

I got an idea of making a digital RPM-meter for my car. First I made a filter for the signal which comes from the ignition coil and tested the code with a dev-board. 


Signal comes straight 


CAR: Honda Civic 1988 Dual Carburetor  

MCU: Mega32


The layout is a bit messy, but I think the pcb will tell the rest. 


The filters main part is 7414 cause it has a Schmitt Trigger to make almost any kind of pulse to square wave. 


Code uses external interrupt to count the rising edges of the pulse in a time unit, which is around one second.


LOTS OF PICTURES

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
