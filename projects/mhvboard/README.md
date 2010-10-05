# MHVBoard

Uploaded by evildeece on 2010-10-04 20:26:00 (rating 0 out of 5)

## Summary

Full details, assembly instructions & the latest version will always be available at:  

[http://www.makehackvoid.com/grou...](http://www.makehackvoid.com/group-projects/mhvboard-arduino-clone)


The MHVBoard is a low cost Arduino clone, based off the Metaboard.


It offers a TTL serial port, USB programming, ICSP, breadboard compatible headers and hardware already set up for software USB via V-USB.


Features  

 * Supports native USB direct to the microcontroller via V-USB  

 * TTL level serial port  

 * Programming via USB  

 * Onboard prototyping area for additional hardware  

 * Shield compatible - Revision 4+ only  

 * Plugs into double-width breadboards (Revision 1-3), and single width breadboards (Revision 4+)  

 * Atmega328P microcontroller with 30kB flash, 2kB RAM, 1kB EEPROM available  

 * ICSP header to update the bootloader  

 * All through-hole construction for easy hand assembly  

 * Supports Arduino and MHVLib runtimes, as well as raw AVR-GCC  

 * 20MHz clock - your code runs faster than the Arduino boards  

 * Pins labelled both with Arduino pin numbers and port/pin numbers  

 * Jumper selectable power source - USB or external power  

 * 4 mounting holes for extra stability  

 * AREF decoupled for improved ADC accuracy  

 * Onboard LED can be disabled to reduce loading on the line


Differences to the Arduino series


 * Native USB interface for uploading (USBasp), no serial ports involved  

 * Serial port brought out on a TTL Header  

 * Arduino pins 2 and 4 are used for USB communication. If you connect additional hardware to these pins, your program should not use the USB port. This hardware should also be disconnected before connecting to your computer  

 * Arduino pins 0 and 1 are normally used for the Serial port, if your program does not use the Serial port (since USB is available), these pins are available for use  

 * Breadboard compatible pin headers (no shields) - Revision 1-3  

 * Arduino shield and breadboard compatible headers - Revision 4+  

 * No 3.3V source onboard  

 * 20MHz clock vs 16MHz on the Arduino  

 * Due to the additional filtering on AREF, it takes time for AREF to stabilize after a different reference voltage has been selected. The first reading after the reference has changed should be discarded.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
