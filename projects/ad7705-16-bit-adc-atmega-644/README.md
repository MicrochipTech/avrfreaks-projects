# AD7705 16 bit ADC to Atmega 644

Uploaded by NickPenny on 2010-04-19 06:37:00 (rating 0 out of 5)

## Summary

An external 16bit ADC is interfaced with an Atmega 644 as only a 10 bit ADC is available on board. 


The program requires a reference number and as the voltage applied becomes greater the program branches to flashing LED routine. 


To test if a voltage becomes lower than the reference, use brlo instead of brsh.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
