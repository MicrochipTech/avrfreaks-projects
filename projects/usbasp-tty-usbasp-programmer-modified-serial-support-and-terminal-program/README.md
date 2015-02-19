# USBASP-tty - USBASP programmer modified with serial support and terminal program

Uploaded by emklaus on 2015-02-19 02:28:00 (rating 5 out of 5)

## Summary

usbasp+ is a modification to the wonderful usbasp programmer (<http://www.fischl.de/usbasp/>) with added support   

for the on chip UART for communicating with your AVR projects through the programmer device.


 


When combined with the PC host component USBASP\_tty.exe you wil be able to send and  

receive TTL level serial data to/from your microcontroller project via the programmer.


 


Note: this requires the modification of the USBASP programmer to add the necessary RXD & TXD connections.


   as far as I know none come with these connections in place.  Typically pins 4&6 on the 10pin connector are


   connected to ground but can be made available for these connections with some careful soldering.


 


The firmware is targeted for the ATMega8 but could be ported to other devices  

with more than 4K of flash


 


Source for the programmer modifications and PC host/terminal program are included.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
- ATmega8
- usbasp
- ilbusb-win32
- MinGW
