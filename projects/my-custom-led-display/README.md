# My Custom LED Display

Uploaded by kdiggity08 on 2009-02-27 05:42:00 (rating 0 out of 5)

## Summary

I built this 5x6 LED display from scratch and based it off of the link on avrfreaks main page. The project utilizes a method call row-column addressing for controlling the most leds. My project controls 30 leds with just 11 pins on the Atmega8 AVR processor.


I have updated the software for the display. I implemented timers 0 and 1 for timeout functions. Timer 0 will strobe the led display 100us for every row and update the display. Timer 1 will update the data to be displayed every couple of seconds. This current project will count from 0 to 9 and repeat itself.

## Links

- [Link to Video of Working LED display](http://akadolph.blogspot.com)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
