# SPI Interface to a Maxim DS1306 Real-Time Clock

Uploaded by jbleecker on 2009-08-20 16:50:00 (rating 0 out of 5)

## Summary

Simple project, using an ATmega8 (and ATMega168 on newer Arduinos) as found in an Arduino board to interface with a real-time clock produced by Maxim/Dallas called the DS1306. Demonstrates the use of the alarm function of the DS1306 to cause an interrupt on the ATMega8 and ATMega168 to handle the alarm. Resets the alarm so that it occurs every 15 seconds starting at 0 seconds.


N.B. That the code is "Wiring", a superset of avr-gcc. There are two methods used in the code that can be easily ported to raw avr-gcc: Serial.print, digitalWrite and pinMode. These are just abstractions of code to write to the serial port, set a pin to either on or off, and set the direction of a pin â€” all functions that are easily translated to avr-gcc.


Here is the URL for the code. For some technical reason, my changes to the "files" section here do not work.


[http://www.nearfuturelaboratory....](http://www.nearfuturelaboratory.com/2006/12/14/arduino-and-ds1306-real-time-clock/)

## Links

- [Arduino and the Maxim DS1306 Real-Time Clock](http://research.techkwondo.com/blog/julian/276)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
