# ATMega 16  1 WIRE Control Panel

Uploaded by davidrs on 2009-09-08 12:37:00 (rating 0 out of 5)

## Summary

Use an ATMega with ADC to identify and return the switch code using only  

1 pin of the ATMega for any number of switches.


Included is a Dip Trace diagram of my set along with the source code.  

I had used this arrangement on an HC11 some time ago, so i needed  

on an AVR project... This one is fully tested as a standalone...


R1 1k, R2 2.2k, R3 3.3k, R4 4.7k, R5 10k pullup, C1 .01uf...  

returns sw1 1, sw2 2, sw3 3, sw4 5...


This system uses a wired NOR circuit connected to INT0...  

The internal resistance of the 1N914, 1N4148 or equivelent will not allow the voltage at INT0 to go to ZERO, so the small cap, C1, will help with this... 


I have left my test code in the scource file. You may find it useful...


Good Luck with this...

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
