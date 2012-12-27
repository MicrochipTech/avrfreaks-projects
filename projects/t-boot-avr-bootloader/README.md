# t-boot avr bootloader

Uploaded by Trolll on 2012-12-27 05:31:00 (rating 0 out of 5)

## Summary

**Description:** 


tboot loader is a ultra small (only 256 bytes) open-source boot loader for AVR microcontrollers, it is used for programming flash using a serial link between your computer and the microcontroller through any free serial port (or USB to serial adapter). tboot consists of two parts: 1. On chip boot loader, that resides at the boot section of the flash memory, this loader based on SSBL bootloader [http://www.hhhh.org/wiml/proj/av...](http://www.hhhh.org/wiml/proj/avr/ssbl.html) 2. software - a console utility, that has the responsibility of writing/reading data into/from the microcontroller Tested on devices: ATmega8, ATmega32, ATmega8535 If you used tboot loader with other devices, please inform me.


 


**Requirements**:


- Microsoft Windows (tested on XP and Vista), GNU Linux or Mac OS X


- A free serial port or USB port + USB to serial adapter (tested on CP2102-based adapter)


- WinAVR or GCC + AVR libc.


 


**Project page**: <http://trolsoft.ru/en/soft/tboot/>

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
- ATmega16
- ATmega32
- ATmega8
- ATmega8535
- tboot
- bootloader
