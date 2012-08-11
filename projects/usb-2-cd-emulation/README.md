# USB 2 CD Emulation

Uploaded by creno on 2012-08-11 05:18:00 (rating 5 out of 5)

## Summary

usb - to - cdrom(emulation) / sd card bridge firmware in AVR CPUs  

(supporting AVR32 and AVR8 chips, capacitive touch, LCD graphics, 4x40 LCD)


3 versions of hardware:Â   

Â  1) Â  EVK1104 (fast CD emulation / LCD graphics)  

Â  2) Â  Custom board (fastest CD emulation / 4x40 LCD)  

Â  3) Â  Teensy++ 2.0 (slowest but cheapest, 1 led feedback)


Â  Fast versions use SD card for storage and 4 button input (touch buttons with the EVK1104), Teensy version uses Minisd in spi mode. All firmware is available in full as source code using avr studio (various versions).


The slowest #3 is the cheapest in hardware cost. 1 teensy++ 2.0 and 1 Minisd interface = $36 plus Minisd card


project info: <http://renosite.com>  

project forum: <http:renosite.com/forum>  

svn repo: [https://usbcdemulation.svn.sourc...](https://usbcdemulation.svn.sourceforge.net/svnroot/usbcdemulation/)


Take all your iso files with you on an external usb cd emulator, able to select iso files from built in menu, once selected the iso file is emulated as an external usb cdrom drive. I'm now using one on the job all the time. No more missing Cd's!

## Compilers

- AVR Studio 5

## Tags

- Complete code with HW
- AVR Studio 5
