# ATMega8Multiplex7segmentDriver

Uploaded by ali32768 on 2010-10-06 12:59:00 (rating 5 out of 5)

## Summary

(BASCOM & CODEVISION)  

LED\_based multiplexed 7-segment displays are always a problem for beginners to drive correctly, Exactly when main process & display process combined together.


So i decide to show you, how you can drive it as easy as possible at lowest price.  

This Example designed for 4digits(common anode) multiplexed display. See table below:


 7Segment | AVR I/O | I/O  

 digits | need | Condition  

 -----------|---------|----------  

 2 digits | 2+8=10 | output  

 | |  

 3 digits | 3+8=11 | output  

 | |  

 4 digits | 4+8=12 | output  

 | |  

 5 digits | 5+8=13 | output  

 . | . | .  

 . | . | .  

 . | . | .


For Example: If you have 4 digits multiplexed 7segment, therefore 12 pins will be occupied only for display.  

Consider ATMega8L (23 Total\_I/O - 12 used\_I/O) = 11 pins free to use for another works.


Depend on 7segment current dissipation; you may need current amplifier (transistor) to make more brightness.


How can i know, if i need "current amplifier" ?  

Ans: connect a 2mA source current(+5v series with 1.2k resistor) to one segment(One LED), if the brightness is enough, you don't need transistor for current amplifier.


Here it is the link, you can download the sources and some practical example:


******************************************************************  

****Please Rate me if my project is useful to you.****  

******************************************************************

## Tags

- Complete code
