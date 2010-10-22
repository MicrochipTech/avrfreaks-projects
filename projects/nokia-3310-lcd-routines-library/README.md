# Nokia 3310 LCD routines library

Uploaded by DHARMANI on 2010-10-22 10:25:00 (rating 0 out of 5)

## Summary

Hi friends,  

I'm posting here the library of Nokia 3310 LCD routines which I've used in my projects related to that LCD, including the one which I've recently posted here on the design of thermometer using this LCD.


Here I've used ATmega8, but it can be used on any of the AVRs having SPI interface, with change in only the #include  line in the program as the SPI register names are same among all AVRs (as far as I know)


Hope it is useful to you!


>Update: Nokia3310\_routines V2.0 (AVR-GCC version)  

 updated by Michael Spiceland (<http://tinkerish.com>) to pixel level functions with lcd\_buffer[][]. (Jan 2009)


>Update 22-Oct-2010:  

Bug fixed: In version 2.0 (avr-gcc) code , the defines SET\_DC\_PIN & CLEAR\_DC\_PIN were not matching the schematic (PB1 was used in the code, while PB0 in the schematic). New file is of version 2.0.1  

Thanks to Egert Lehtla who pointed it out.


Thanks.

## Links

- [Check out here my project with the 3310 LCD using this library](http://www.dharmanitech.com/2008/10/thermometer-design-using-ds1621.html)
- [Here is how I started using this LCD](http://www.dharmanitech.com/2008/09/nokia-3310-lcd-interfacing-with-atmega8.html)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Part-specific Lib.functions
- WinAVR (GNU GCC)
