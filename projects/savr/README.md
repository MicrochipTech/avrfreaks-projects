# SAVR

Uploaded by srfilipek on 2011-09-08 12:04:00 (rating 0 out of 5)

## Summary

Hosted on Google Code: <http://code.google.com/p/savr/>


A C++ library for a handful of different AVR microcontrollers. This is primarily developed on the Arduino, but modified to be cross-platform (cross-micro) to other AVRs I have laying around.


It also has a handful of different peripheral interfaces implemented, such as the SD card protocol over SPI.


Library includes:


* Pin-based GPIO interface  

* Interfaces for various buses:  

** SPI  

** SCI (UART)  

** TWI (I2C)  

** 1-Wire  

* SCI/UART binding to stdin and stdout  

* Terminal interface  

** Simple command interface


Various Peripherals:  

* ST7066/HD44780 based character LCDs  

* W5100 (Arduino ethernet shield, not fully developed)  

* SD Cards over SPI  

* DS18B2x and DS182x 1-Wire temp sensors


This library relies on avr-libc and AVR GCC, such as distributed through WinAVR and CrossPack for AVR.

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
