# AVR Scope Clock shield V3.A

Uploaded by jrseattle on 2008-11-23 02:28:00 (rating 0 out of 5)

## Summary

A new Arduino shield: Dutchtronix AVR Scope Clock shield V3.A


The Arduino system is great for small experiments with relative easy programming using the IDE and its support libraries.


The hardware can also be used for other applications using the Arduino shields. The shields are a very convenient form factor for small projects, avoiding the usual bundle of wires. For experiments, use one of the blank shields, or order a ready made project. Both are available at [www.adafruit.com](http://www.adafruit.com).


A more ambitious undertaking is to design your own shield and to have it made at a PCB house. For small boards in small quantities, batchpcb is the best option, provided you're not in a hurry.


I wasn't, so I made a shield version of the Dutchtronix AVR Scope Clock, based on the new Hardware V3.1, which uses the advanced AD7302 DAC. Besides speed (parallel interface) and image quality (simultaneous channel updates), this DAC provides low output impedance (about 40 Ohms) which means that BNC cables can be used to connect the clock to a scope (or X-Y monitor); no probes needed. Other features of the V3.1 hardware are the availability of multiple push button switches, on board 5V regulator and true RS-232 level converter. See [www.oscilloscopeclock.com](http://www.oscilloscopeclock.com) for details.


The Arduino board provides some of these functions already (5V regulator, Serial interface through USB) so the Clock Shield doesn't have to do these things. It hosts the DAC and the RTC (Real Time Clock), 2 BNC connectors (plus 2 turret terminals for probes), 2 push button switches, the image sizing trimpot, Intensity Control and the backup battery.


One thing that is not possible with the Arduino is to connect to other projects using a serial port, which can be done with the official Dutchtronix clock. The USB connection on the Arduino requires a USB host. This means that the Arduino Scope Clock shield cannot be connected to a GPS device for automatic time and date setting.


Finally, the Arduino bootloader is relatively big, 2KB of code. To be honest, it doesn't have to be since the code can be squeezed into 1KB with a little effort, but that's a little late for the many thousands of Arduino systems out in the field. This means that there is only 14KB code space available for the Scope Clock application. To make the application fit, the GPS functionality, as well as the Demo mode, have been disabled.


The latest incarnation of the Dutchtronix AVR Oscilloscope clock, the Hardware 3.1 Enhanced kit, uses the Atmega328p AVR with twice the code space. Using a 1KB bootloader (Arduino compatible) makes 31KB of application space available. This extra code space makes it possible to support multiple pre-loaded applications (clock, terminal, function generator).


Once the Arduino system has been officially upgraded to use the Atmega328p (or at least is an official option), I'll make the Enhanced kit code available for the Arduino Scope Clock shield.


Programming the Arduino using the bootloader outside the IDE is possible using the batch file "uploadm168.bat". If there is a way within the IDE to uploaded external files, I'd like to know. Hitting the reset button at the right time with the Diecimila is sometimes a challenge.


Attached are the Eagle Design Files for this Scope Clock Shield, as well as the source code, executable code and upload tools.


This shield has been tested with the older NG type Arduino, as well as with the Diecimila. The pictures show that some corrections were made; these corrections have been applied to the Eagle design files. Don't forget that you will need an analog scope or XY monitor to enjoy this gadget.


jrseattle

## Links

- [Project site](http://www.dutchtronix.com/Arduino-ClockShield.htm)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
