# Simple RC servo C code

Uploaded by stiant04 on 2007-05-06 12:14:00 (rating 0 out of 5)

## Summary

This code is developed for the ATmega16 on tha AVR-stk500 trainer and can only control two servos with a ten step resolution (but this can all easily be changed). It is ment to show newbies (like me) how they can make a C program that controls RC servos... 


(Remeber that RC servoes needs a 1-2 millisecond high pulse at 50Hz) 


PORTA=switches , 0=S1++,1=S1center,2=S1-- , 3=S2++,4=S2center,5=S2--  

PORTB=Leds ,shows ten times the ms value(1-2ms -> 10-20 bin) for servo1  

PORTC=Servo signal , PC0 & PC1


Just push a button to move your servos.  

The servo vil return to center position when max or min value is reached.(sorry that most comments are in norwegian)  

Enjoy!  

(Please feel free to send me a message if you have any comments to this project)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
