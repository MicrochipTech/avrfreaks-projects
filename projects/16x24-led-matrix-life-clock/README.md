# 16X24 LED Matrix Life Clock

Uploaded by H. Carl Ott on 2010-10-14 11:06:00 (rating 0 out of 5)

## Summary

We use a Mega48 to talk to an 16x24 led module from Sure Electronics.  

 [Link to data sheet](http://www.sure-electronics.net/mcu,display/DE-DP016.pdf)


This is an inexpensive 16x24 led module that uses a Holtec Ht1632 driver chip.


We display the time, day and date, and then on the minute we optionally run Conway's game of life.


From Wikipedia:  

The Game of Life is a cellular automaton devised by the British mathematician John Horton Conway in 1970. It is the best-known example of a cellular automaton. 


The clock features built in RTC functions, back up battery, and a single rotary encoder driven UI to set the time/date & additional options.


All in assembler, uses ~95% of the mega48 code space.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
