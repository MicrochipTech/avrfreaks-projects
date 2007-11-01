# RC5 decoder

Uploaded by bennyb on 2007-11-01 05:50:00 (rating 0 out of 5)

## Summary

Yes, another RC5 decoder but now fully interrupt driven.


For my time-critical project I needed a RC5 decoding routine. The BASCOM GetRC5 routine could not be used so I ported an old C-code program to BASCOM AVR.  

The code is tested and part of a bigger project, I only copied the RC5 code, you can incorporate this into your program.


It uses INT0 and Timer1 and can be modified to satisfy your own needs.


-TooN

## Compilers

- BASCOM-AVR

## Tags

- Complete code
- BASCOM-AVR
