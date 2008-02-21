# AVR Oscilloscope Terminal

Uploaded by jrseattle on 2008-02-20 22:15:00 (rating 0 out of 5)

## Summary

The Dutchtronix AVR Oscilloscope Terminal


The [Dutchtronix AVR Oscilloscope Terminal](http://www.dutchtronix.com/ScopeTerm.htm) is an **Open Source** Application allowing you to use your existing (analog) oscilloscope as a display device through the AVRs serial interface, using the Dutchtronix AVR Oscilloscope Clock hardware or a similar board. It can display 13 lines, each showing 20 characters, accepts the full ASCII character set, has automatic scrolling and more. There also is a separate command mode to configure the AVR Oscilloscope Terminal.


If you ever experiment with microcontrollers, you know you need a way to get status information from your microcontroller and you don't want to use a lot of precious pins to do that. Serial devices are by far the easiest way to do this since most microcontrollers (at least AVRs) have a serial device built-in. Simple code is minimal and you only need to sacrifice 1 pin to be able to send output.


The problem with a plain serial port is that you need a terminal device, usually a computer, but the computer isn't always conveniently located near the work bench.


LCD displays are very nice but require quite a few pins and added code. Some people buy or build a serializer board for their LCD display, including its own microcontroller to provide an effective display device.


A device that frequently is located on your workbench, but mostly doing nothing, is an oscilloscope. Maybe you got yourself a Dutchtronix AVR Oscilloscope Clock to make it more interesting, possibly to keep an eye on the time. What if you could use that oscilloscope as a display device for your microcontroller projects?


Enter the Dutchtronix AVR Oscilloscope Terminal.


* 13 lines, 20 characters per line
* Displays full ASCII character set including upper and lower case
* Automatic superfast scrolling
* Flashing cursor
* Delayed CR/LF control
* Current time is displayed and updated at the bottom of the screen
* Separate command mode to configure the AVR Terminal (short  

 push on push button switch)
* Multiple Baud Rate support. Current Baud Rate saved in EEProm.
* AVR Assembly Source Code so you can customize the terminal

This terminal is a new application running on the Dutchtronix AVR Oscilloscope Clock board. All you need is a serial cable and a PC to reprogram the AVR Clock and it will turn into an AVR Terminal. This application is provided free of charge  

so if you already have the Dutchtronix AVR Oscilloscope Clock, you now also have the Dutchtronix AVR Oscilloscope Terminal.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
