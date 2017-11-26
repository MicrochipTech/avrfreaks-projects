# V-USB HID Macro-keyboard

Uploaded by vlkon on 2017-11-25 23:14:00 (rating 0 out of 5)

## Summary

Main purpose of this project is to automate keboard presses with external tool that doesn't  

require install aditional software (so it works even on login screen of Windows OS or in BIOS).  

It uses default HID keyboard drivers.


Sequence of keyboard signals is stored in eeprom. It can be uploaded via commandline tool or  

via standard SPI programming interface (although it might not be easy to get a proper addressing  

and also the attached schematic doesn't implement any programming connector on SPI interface).


Device = attiny85 (or 45 if some features of the code are removed to make it smaller) but it shouldn't be a problem to port it to any other 8-bit avr with enough flash/ram/eeprom.


Aditional library is V-USB for the device and libusb-1.0 for commandline tool.


 


All programing was done in Ubuntu OS so I don't know how well will be this code working under other OS.


The target device is designed for Windows OS (it realy means that only some of the keyboard keys are mapped to different values).


The commandline tool works only on Linux (maybe OS X as well but I never tested it).


 


Attached archive contains source code (with hopefuly working Makefile) and schematic in KiCad software.

## Links

- [V-USB library](https://www.obdev.at/products/vusb/index.html)
- [libusb-1.0](http://libusb.info/)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
- PC Accessories
