# Color Clock using two RGB LEDs

Uploaded by rickardg on 2009-03-06 22:57:00 (rating 0 out of 5)

## Summary

This is an ATMEGA88 based color clock that displays the time using two RGB LEDs, one for hours and one for minutes, showing time by using the standard color coding used for resistors (with two additional colors as the standard color coding only has ten colors and twelve is needed). With 12 steps for both hours and minutes it is possible to tell the time with an accuracy of five minutes, and this accuracy is often good enough.


To make the clock always showing the time correctly I've also added a receiver for the German DCF77 time code transmitter in Frankfurt. If no DCF77 signal is present it is also possible to set the time using a separate switch.


The zip-file contains all source and the schematic, but it you want to read more about the project on the projects homepage:


[http://www.rickard.gunee.com/pro...](http://www.rickard.gunee.com/projects/mixed/colorclock)

## Links

- [colorclock.zip](http://www.rickard.gunee.com/projects/mixed/colorclock/colorclock.zip)
- [Project's homepage](http://www.rickard.gunee.com/projects/mixed/colorclock)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
