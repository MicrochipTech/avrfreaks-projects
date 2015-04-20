# USB Bulk Vendor example without ASF for the XMEGA series

Uploaded by tominsf on 2015-04-19 18:24:00 (rating 0 out of 5)

## Summary

For those using the standalone toolchain, without the ASF, with a text editor and a makefile for development, this is a simple example of a USB bulk vendor implementation that doesn't use a third-party stack like LUFA. The USART i/o header file with its printf capability that's included here is optional – this software uses it to output status info to a terminal, but the code referencing it can be removed if you aren't interested in the status output.


 


A few weeks ago, I uploaded a similar project for the AT32UC3B series. This xmega example works pretty much the same way.


 


All the program does is accept data – two bytes at a time – on OUT endpoint 1 and immediately echoes them back on IN endpoint 1. But it shouldn't be too hard to extend this to do useful tasks.


The major part of the program is the interrupt routine that does setup and enumeration. That out of the way, getting data to and from the controller over USB is almost trivial.


I'd recommend these two guides for understanding what's going on during setup:


 


<http://www.usbmadesimple.co.uk/>


 


<http://www.beyondlogic.org/usbnutshell/usb1.shtml>



 


I've provided a very simple host-based program, based on libusb, to demonstrate the controller software. Its command line takes two byte-sized numbers as arguments, and the program sends them via USB to the controller, then retrieves them and prints them on screen. I use Linux and I don't know how easily it can be compiled under Windows (or OS X); I tried to keep it simple so problems would be minimal.


 


I've posted a discussion of this in the xmega forum: <https://www.avrfreaks.net/forum/xmega-usb-bulk-transfer-without-asf-or-lufa>

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
