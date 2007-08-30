# T-Tris

Uploaded by admin on 2007-08-30 08:03:00 (rating 0 out of 5)

## Summary

This Tetris-Clone was assembled from one ATmega8, 240 red LEDs, a few BJTs, a few IC's and some push-buttons.


If you want to build something like that,  

feel free to use some parts of this projekt.


If you want to build this proj. with another AVR,  

you should be aware of following issue:


You don't have to stick by the given resistor values,  

because they are not critical.  

With these values for the current limiting resistors (150R),  

the current through one LED should be around 19mA.  

If one row is active, the AVR sources 190mA through PortB and PortD.  

According to the datasheet of the ATmega8, the sum of all IOH,  

for ports B and D should not exceed 200mA.  

I optimized for brightness, not for reliability... ;-)  

Due to variations of parameters of your specific AVR,  

you might damage your IC, if you draw to much current from the AVR.


If your not sure, how to calculate those things,  

it might be a good idea to build a test-circuit on a breadboard,  

to find the best value for these resistors.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
