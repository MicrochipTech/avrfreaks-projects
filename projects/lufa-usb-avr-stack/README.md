# LUFA - USB AVR Stack

Uploaded by abcminiuser on 2013-09-01 11:33:00 (rating 0 out of 5)

## Summary

LUFA (Lightweight USB Framework for AVRs, formerly known as MyUSB) is my first foray into the world of USB. It is an open-source complete USB stack for the USB-enabled Atmel AVR8 and (some of the) AVR32 microcontroller series, released under the permissive MIT License (see documentation or project source for full license details). The complete line of Atmel USB AVRs and USB AVR boards are supported by the library, as are any custom user boards, via custom board hardware drivers supplied by the user.


 


![Official LUFA Logo](http://www.fourwalledcubicle.com/img/LUFA/LUFA.png)


 


 


The library is currently in a stable release, suitable for download and incorporation into user projects for both host and device modes. For information about the project progression, check out [my blog](http://www.fourwalledcubicle.com/blog).


LUFA is written specifically for the free AVR-GCC compiler, and uses several GCC-only extensions to make the library API more streamlined and robust. You can download AVR-GCC for free in a convenient windows package, from the the [Atmel website](http://www.atmel.com/tools/ATMELAVRTOOLCHAINFORWINDOWS.aspx).


Included with the library source are many demonstration applications showing off the use of the library. Currently the library includes the following demonstration applications (most in both Low Level API and Library USB Class Driver variants):


 


* Android Accessory Host
* Audio In Device
* Audio In Host
* Audio Out Device
* Audio Out Host
* Bulk Vendor Device
* Dual MIDI Device
* Dual Virtual Serial Device
* Generic HID Device
* Generic HID Host
* Joystick Device
* Joystick Host
* Keyboard Device
* Keyboard/Mouse Device
* Keyboard Host
* Keyboard Host/Device Dual Mode
* Mass Storage Device
* Mass Storage/Keyboard Device
* Mass Storage Host
* MIDI Device
* MIDI Host
* Mouse Device
* Mouse Host
* Printer Host
* RNDIS (CDC) Ethernet Device
* RNDIS (CDC) Ethernet Host
* Still Image Host
* Virtual Serial Device
* Virtual Serial Host
* Virtual Serial/Mass Storage Device
* Virtual Serial/Mouse Device

 


LUFA also contains USB bootloaders for the following USB classes:


* CDC Class, AVR109 protocol compatible ([AVRDude](http://www.nongnu.org/avrdude/))
* DFU Class, Atmel DFU protocol compatible ([Atmel FLIP](http://www.atmel.com/tools/FLIP.aspx), [dfu-programmer](http://dfu-programmer.sourceforge.net/))
* HID Class, with an included custom cross-platform loader application
* Printer Class, using the OS's native printer drivers
* Mass Storage Class, using the OS's native USB storage drivers

## Links

- [Project Webpage, with download link](http://www.lufa-lib.org)
- [Development Blog](http://www.fourwalledcubicle.com/blog/)
- [Support Mailing List](http://www.lufa-lib.org/support)

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
- AT90USBxxx
- USB
